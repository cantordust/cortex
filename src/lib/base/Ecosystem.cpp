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
		if (!cfg.validate())
		{
			return false;
		}

		species.clear();
		nets.clear();
		cfg.reset_ids();

		uint spc_id(0);
		uint net_id(0);

		uint nets_per_spc( cfg.ecosystem.init.size / cfg.species.init.count );

		for (uint s = 0; s < cfg.species.init.count; ++s)
		{
			/// Generate a species ID
			spc_id = cfg.new_spc_id();
			Genotype gen(cfg.node.roles);
			gen.add(NR::H, s);
			insert_species(spc_id, gen);

			/// Generate networks
			for (uint n = 0; n < nets_per_spc; ++n)
			{
				net_id = cfg.new_net_id();
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

		if (cfg.species.max.count == 0 ||
			species.size() < cfg.species.max.count)
		{
			/// The species doesn't exist and we can create it.
			/// Get an ID for the new species.
			spc_id = cfg.new_spc_id();

			/// Register the new species with the ecosystem.
			insert_species(spc_id, _gen);
		}

		return spc_id;
	}

	void Ecosystem::evolve()
	{
		find_champ();
		remove_empty_species();
		update_fitness();

		if (cfg.mating.enabled)
		{
			crossover();
		}
	}

	void Ecosystem::find_champ()
	{
//		dlog() << "Finding global champion...";

		real max_fit(0.0);
		stats.champ_id = 0;

		for (auto& net : nets)
		{
			if (stats.champ_id == 0 ||
				net.second.get_abs_fitness() > max_fit)
			{
				stats.champ_id = net.first;
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

			/// Update the relative network fitness
			/// and compute the species fitness and diversity
			stats.species.add(spc.second.compute_fitness());
		}

		for (auto& spc : species)
		{
//			dlog() << "Updating the relative fitness of species " << spc.first;

			/// Update the relative network fitness
			/// and compute the species fitness and diversity
			spc.second.set_rel_fitness(sigmoid(spc.second.progress() * (spc.second.get_abs_fitness() - stats.species.avg) / (stats.species.sd == 0.0 ? 1.0 : stats.species.sd)));
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
		/// Remove empty species
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

		/// Select a species
		hmap<uint, real> mating_wheel;
		hmap<uint, real> culling_wheel;

		/// Mean fitness
		real old_fit_mean(0.0);
		real fit_mean(0.0);
		real fit_sd(0.0);

		uint spc_count(0);
		for (auto& spc : species)
		{
			mating_wheel[spc.first] = spc.second.mating_chance();
			culling_wheel[spc.first] = spc.second.culling_chance();
		}

		/// Produce offspring equal to the mating chance
		///	portion of the current ecosystem size.
		uint offspring(cfg.mating.rate * nets.size());
		while (offspring > 0)
		{
			uint spc_id(0);
			uint p1(0);
			uint p2(0);

			do
			{
				/// Select species for the two parents
				spc_id = cfg.w_dist(mating_wheel);

				/// Select parents
				p1 = species.at(spc_id).get_parent();
				p2 = species.at(spc_id).get_parent();

			} while (spc_id == 0 || p1 == 0 || p2 == 0);

			/// Get the next network ID.
			uint new_net_id(cfg.new_net_id());

			/// Create a new network.
			insert_net(new_net_id, spc_id);
			nets.at(new_net_id).crossover(nets.at(p1), nets.at(p2));

			/// Increase the offspring count
			--offspring;
		}

		/// Culling

		/// Erase old nets
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

		/// Cull networks until the size limit
		/// of the ecosystem is reached.
		while (nets.size() > cfg.ecosystem.max.size)
		{
			/// Erase a network from a random species
			/// picked from the cull roulette wheel.
			uint net_id(species.at(cfg.w_dist(culling_wheel)).erase_net());

			if (net_id != 0 &&
				net_id != stats.champ_id)
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
}
