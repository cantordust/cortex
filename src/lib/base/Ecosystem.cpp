#include "Ecosystem.hpp"

namespace Cortex
{

	Ecosystem::Ecosystem(Config& _cfg)
		:
		  age(1),
		  cfg(_cfg),
		  tp(_cfg.threads)
	{
//		dlog() << "Ecosystem created in thread " << std::this_thread::get_id();
	}

	Ecosystem::~Ecosystem()
	{
//		dlog() << "Stopping threadpool...";
		tp.stop();
	}

	bool Ecosystem::init()
	{
		if (!cfg.get().validate())
		{
			return false;
		}

		species.clear();
		nets.clear();
		cfg.get().reset_ids();

		uint spc_id(0);
		uint net_id(0);

		uint nets_per_spc( cfg.get().ecosystem.init.size / cfg.get().species.init.count );

		for (uint s = 0; s < cfg.get().species.init.count; ++s)
		{
			// Generate a species ID
			spc_id = cfg.get().new_spc_id();

			// Generate a genotype for the species
			Genotype gen(cfg.get().node.roles);

			// Set the number of input receptors for the genome
			if (cfg.get().net.type == NT::Spiking)
			{
				gen.set(NR::I, s * cfg.get().net.spiking.receptors);
			}

			// Set the number of hidden neurons for the genome
			gen.add(NR::H, s);

			// Insert the species into the environment
			insert_species(spc_id, gen);

			// Generate networks
			for (uint n = 0; n < nets_per_spc; ++n)
			{
				net_id = cfg.get().new_net_id();
//				dlog() << "Generating network " << net_id;
				insert_net(net_id, spc_id);
				nets.at(net_id).init();
//				dlog() << nets.at(net_id);
			}
		}

		if (nets.size() == 0 ||
			species.size() == 0)
		{
			return false;
		}
		return true;
	}

	uint Ecosystem::get_species_id(const Genotype& _gen)
	{
		uint spc_id(0);
		for (auto& s : species)
		{
			if (s.second == _gen)
			{
				return s.first;
			}
		}

		if (cfg.get().species.max.count == 0 ||
			species.size() < cfg.get().species.max.count)
		{
			// The species doesn't exist and we can create it.
			// Get an ID for the new species.
			spc_id = cfg.get().new_spc_id();

			// Register the new species with the ecosystem.
			insert_species(spc_id, _gen);
		}

		return spc_id;
	}

	inline bool Ecosystem::is_solved()
	{
		glock lk(stat.mtx);
		return stat.solved;
	}

	inline void Ecosystem::mark_solved(const uint _net)
	{
		{
			glock lk(stat.mtx);
			if (!stat.solved)
			{
				stat.champ_id = _net;
				stat.solved = true;
			}
		}
		tp.stop();
	}

	inline uint Ecosystem::get_champ_id()
	{
		glock lk(stat.mtx);
		return stat.champ_id;
	}

	inline const Net&Ecosystem::get_champ()
	{
		glock lk(stat.mtx);
		return nets.at(stat.champ_id);
	}

	inline void Ecosystem::inc_evals()
	{
		glock lk(stat.mtx);
		++stat.total_evals;
	}

	inline uint Ecosystem::get_total_evals()
	{
		glock lk(stat.mtx);
		return stat.total_evals;
	}

	inline void Ecosystem::print_champ()
	{
		glock lk(stat.mtx);
		if (nets.find(stat.champ_id) != nets.end())
		{
			dlog() << "Champion:\n" << nets.at(stat.champ_id);
		}
	}

	inline uint Ecosystem::get_net_count() const
	{
		return nets.size();
	}

	inline uint Ecosystem::get_species_count() const
	{
		return species.size();
	}

	inline SpeciesRef Ecosystem::get_species(const uint _spc_id)
	{
		return std::ref(species.at(_spc_id));
	}

	inline std::vector<NetRef> Ecosystem::get_nets()
	{
		std::vector<NetRef> net_refs;
		for (auto&& net : nets)
		{
			net_refs.emplace_back(net.second);
		}
		return net_refs;
	}

	inline void Ecosystem::erase_net(const uint _net_id)
	{
		species.at(nets.at(_net_id).species.get().id).erase_net(_net_id);
		nets.erase(_net_id);
	}

	inline void Ecosystem::evolve()
	{
		find_champ();
		remove_empty_species();
		update_fitness();

		if (cfg.get().mating.enabled)
		{
			crossover();
		}
	}

	inline void Ecosystem::find_champ()
	{
//		dlog() << "Finding global champion...";

		real max_fit(0.0);
		stat.champ_id = 0;

		for (auto& net : nets)
		{
			if (stat.champ_id == 0 ||
				net.second.get_abs_fitness() > max_fit)
			{
				stat.champ_id = net.first;
				max_fit = net.second.get_abs_fitness();
			}
		}

//		print_champ();
	}

	void Ecosystem::update_fitness()
	{
		for (auto& spc : species)
		{
//			dlog() << "Computing the absolute fitness of species " << spc.first;

			// Update the relative network fitness
			// and compute the species fitness and diversity
			stat.species.update(spc.second.compute_fitness());
		}

		for (auto& spc : species)
		{
//			dlog() << "Updating the relative fitness of species " << spc.first;

			// Update the relative network fitness
			// and compute the species fitness and diversity
			spc.second.set_rel_fitness(sigmoid(spc.second.progress() * (spc.second.get_abs_fitness() - stat.species.mean) / (stat.species.sd == 0.0 ? 1.0 : stat.species.sd)));
		}

		for (auto& spc : species)
		{
			dlog() << "Species " << spc.second.id << ":"
				   << "\n\tAbsolute fitness: " << spc.second.get_abs_fitness()
				   << "\n\tRelative fitness: " << spc.second.get_rel_fitness();
		}
	}

	void Ecosystem::remove_empty_species()
	{
		// Remove empty species
		for (auto it = species.begin(); it != species.end(); )
		{
			if (it->second.is_empty())
			{
//				dlog() << "Removing empty species " << it->second.id;
				it = species.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Ecosystem::crossover()
	{

		// Select a species
		hmap<uint, real> mating_wheel;
		hmap<uint, real> culling_wheel;

		// Mean fitness
		real old_fit_mean(0.0);
		real fit_mean(0.0);
		real fit_sd(0.0);

		uint spc_count(0);
		for (auto& spc : species)
		{
			mating_wheel[spc.first] = spc.second.mating_chance();
			culling_wheel[spc.first] = spc.second.culling_chance();
		}

		// Produce offspring equal to the mating chance
		//	portion of the current ecosystem size.
		uint offspring(cfg.get().mating.rate * nets.size());
		while (offspring > 0)
		{
			uint spc_id(0);
			uint p1(0);
			uint p2(0);

			do
			{
				// Select species for the two parents
				spc_id = cfg.get().w_dist(mating_wheel);

				// Select parents
				p1 = species.at(spc_id).get_parent();
				p2 = species.at(spc_id).get_parent();

			} while (spc_id == 0 || p1 == 0 || p2 == 0);

			// Get the next network ID.
			uint new_net_id(cfg.get().new_net_id());

			// Create a new network.
			insert_net(new_net_id, spc_id);
			nets.at(new_net_id).crossover(nets.at(p1), nets.at(p2));

			// Increase the offspring count
			--offspring;
		}

		// Culling

		// Erase old nets
		for (auto it = nets.begin(); it != nets.end(); )
		{
			if (it->second.is_old())
			{
				it = nets.erase(it);
			}
			else
			{
				++it;
			}
		}

		// Cull networks until the size limit
		// of the ecosystem is reached.
		while (nets.size() > cfg.get().ecosystem.max.size)
		{
			// Erase a network from a random species
			// picked from the cull roulette wheel.
			uint net_id(species.at(cfg.get().w_dist(culling_wheel)).erase_net());

			if (net_id != 0 &&
				net_id != stat.champ_id)
			{
				dlog() << "Erasing network " << net_id;
				nets.erase(net_id);
			}
		}
	}

	void Ecosystem::mutate()
	{
		for (auto& spc : species)
		{
			spc.second.mutate();
		}
	}

	inline void Ecosystem::insert_net(const uint _net_id, const uint _spc_id)
	{
		nets.emplace(std::piecewise_construct,
					 std::forward_as_tuple(_net_id),
					 std::forward_as_tuple(_net_id, *this, species.at(_spc_id)));
	}

	inline void Ecosystem::insert_species(const uint _spc_id, const Genotype& _gen)
	{
		species.emplace(std::piecewise_construct,
						std::forward_as_tuple(_spc_id),
						std::forward_as_tuple(_spc_id, _gen, cfg));
	}
}
