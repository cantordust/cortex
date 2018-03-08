#include "Conf.hpp"
#include "Ecosystem.hpp"
#include "Species.hpp"
#include "Net.hpp"

namespace Cortex
{
	Ecosystem::Ecosystem(Conf& _conf)
		:
		  age(1),
		  conf(_conf),
		  solved(false),
		  champ(nullptr),
		  evals(0),
		  tp(_conf.sys.threads)
	{
//		dlog() << "Ecosystem created in thread " << std::this_thread::get_id();
	}

	bool Ecosystem::init()
	{
		conf.validate();

		species.clear();
		nets.clear();
		conf.reset_ids();

		uint spc_id(0);
		uint net_id(0);

		uint nets_per_spc( conf.eco.init.size / conf.spc.init.count );

		for (uint s = 0; s < conf.spc.init.count; ++s)
		{
			// Generate a species ID
			spc_id = conf.spc.next_id();

			// Generate a genotype for the species
			Genotype gen(conf);

			// Set the number of input receptors for the genome
			gen.set(NR::I, gen.count(NR::I) * conf.net.rf.size);

			// Set the number of hidden neurons for the genome
			gen.add(NR::H, s);

			// Insert the species into the environment
			insert_spc(spc_id, gen);

			// Generate networks
			for (uint n = 0; n < nets_per_spc; ++n)
			{
				net_id = conf.net.next_id();
//				dlog() << "Generating network " << net_id;
				insert_net(net_id, spc_id);
				nets.at(net_id)->init();
//				dlog() << *nets.at(net_id);
			}
		}

		if (nets.size() == 0 ||
			species.size() == 0)
		{
			return false;
		}
		return true;
	}

	const SpcPtr Ecosystem::get_spc(const Genotype& _gen)
	{
		for (const auto& s : species)
		{
			if (*s.second == _gen)
			{
				return s.second;
			}
		}

		if (conf.spc.max.count == 0 ||
			species.size() < conf.spc.max.count)
		{
			/// The species doesn't exist and we can create it.
			/// Register the new species with the ecosystem.
			return insert_spc(conf.spc.next_id(), _gen);
		}

		return nullptr;
	}

	bool Ecosystem::is_solved()
	{
		return solved;
	}

	void Ecosystem::mark_solved(const uint _net_id)
	{
		if (!solved)
		{
			champ = nets.at(_net_id);
			solved = true;
			tp.stop();
		}
	}

	const NetPtr Ecosystem::get_champ()
	{
		return champ;
	}

	void Ecosystem::inc_evals()
	{
		++evals;
	}

	uint Ecosystem::get_evals()
	{
		return evals;
	}

	void Ecosystem::print_champ()
	{
		if (champ)
		{
			dlog() << "Champion:\n" << champ->id;
		}
	}

	void Ecosystem::print_stats()
	{
		dlog() << "\n------------------------"
			   << "\nGeneration: " << age
			   << "\nSpecies count: " << species.size()
			   << "\nNetwork count: " << nets.size()
			   << "\nHighest fitness: " << (champ ? champ->fitness.abs.cur : 0.0)
			   << "\n\n*** Evaluating networks...";
	}

	uint Ecosystem::get_net_count() const
	{
		return nets.size();
	}

	uint Ecosystem::get_spc_count() const
	{
		return species.size();
	}

	const hmap<uint, NetPtr>& Ecosystem::get_nets()
	{
		return nets;
	}

	const NetPtr Ecosystem::get_net(const uint _net_id)
	{
		return (nets.find(_net_id) == nets.end()) ? nullptr : nets.at(_net_id);
	}

	const SpcPtr Ecosystem::get_spc(const uint _spc_id)
	{
		return (species.find(_spc_id) == species.end()) ? nullptr : species.at(_spc_id);
	}

	void Ecosystem::erase_net(const uint _net_id)
	{
		species.at(nets.at(_net_id)->spc->id)->erase_net(_net_id);
		nets.erase(_net_id);
	}

	void Ecosystem::evolve()
	{
		/// Statistics
		dlog() << "Total evaluations: " << evals;

		/// Increase the age of the ecosystem by 1
		++age;
		for (auto& net : nets)
		{
			++net.second->age;
		}

		if (solved)
		{
			print_champ();
		}
		else
		{
			dlog() << "*** Evolving ecosystem";

			find_champ();
			cleanup();
			update_fitness();

			if (conf.mating.enabled)
			{
				crossover();
			}
		}
	}

	void Ecosystem::find_champ()
	{
//		dlog() << "Finding global champion...";

		real max_fit(0.0);
		champ = nullptr;

		for (const auto& net : nets)
		{
			if (champ == nullptr ||
				net.second->fitness.abs.cur > max_fit)
			{
				champ = net.second;
				max_fit = champ->fitness.abs.cur;
			}
		}

//		print_champ();
	}

	void Ecosystem::update_fitness()
	{
		Stat spc_stat(MA::SMA);

		for (const auto& spc : species)
		{
//			dlog() << "Computing the absolute fitness of species " << spc.first;

			/// Compute the relative network fitness
			/// and the absolute species fitness
			spc_stat.update(spc.second->update_fitness());
		}

		for (const auto& spc : species)
		{
//			dlog() << "Updating the relative fitness of species " << spc.first;

			/// Compute the relative species fitness
			spc.second->fitness.rel.update(Logistic(spc.second->fitness.abs.get_offset()) *
										   Logistic(spc_stat.get_offset( spc.second->fitness.abs.cur ) ) );
		}

		for (const auto& spc : species)
		{
			dlog() << "Species " << spc.second->id << ":"
				   << "\n\tAbsolute fitness: " << spc.second->fitness.abs.cur
				   << "\n\tRelative fitness: " << spc.second->fitness.rel.cur;
		}
	}

	void Ecosystem::cleanup()
	{
		/// Remove empty species
		for (auto it = species.begin(); it != species.end(); )
		{
			if (it->second->is_empty())
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
		/// Roulette wheels for mating and culling
		hmap<uint, real> mating_wheel;
		hmap<uint, real> culling_wheel;

		uint spc_count(0);
		for (auto& spc : species)
		{
			mating_wheel[spc.first] = spc.second->mating_chance();
			culling_wheel[spc.first] = spc.second->culling_chance();
		}

		///=========================================
		/// Mating
		///=========================================

		/// Produce offspring equal to the mating rate
		/// portion of the current ecosystem size.
		uint offspring(conf.mating.rate * nets.size());
		while (offspring > 0)
		{
			uint spc_id(0);
			uint p1(0);
			uint p2(0);

			do
			{
				/// Select a species for the two parents
				spc_id = conf.w_dist(mating_wheel);

				if (spc_id != 0)
				{
					/// Get random parents from that species.
					/// Networks with higher fitness have a
					/// greater chance of being parents.
					p1 = species.at(spc_id)->get_rnd_parent();
					p2 = species.at(spc_id)->get_rnd_parent();
				}
			} while (spc_id == 0 ||
					 p1 == 0 ||
					 p2 == 0);

			/// Create a new network with
			/// the next available network ID.
			uint new_net_id(conf.net.next_id());
			insert_net(new_net_id, spc_id);

			/// Perform crossover
			nets.at(new_net_id)->crossover(nets.at(p1), nets.at(p2));

			/// Increase the offspring count
			--offspring;
		}

		///=========================================
		/// Culling
		///=========================================

		/// Erase old nets
		for (auto it = nets.begin(); it != nets.end(); )
		{
			it->second->is_old() ? it = nets.erase(it) : ++it;
		}

		/// Cull networks until the size limit
		/// of the ecosystem is reached.
		while (nets.size() > conf.eco.max.size)
		{
			/// Erase a network from a random species
			/// picked from the culling  wheel.
			uint net_id(species.at(conf.w_dist(culling_wheel))->erase_net());

			if (net_id != 0)
			{
				dlog() << "Erasing network " << net_id;
				nets.erase(net_id);
			}
		}
	}

	void Ecosystem::mutate()
	{
		for (const auto& spc : species)
		{
			spc.second->mutate();
		}
	}

	SpcPtr Ecosystem::insert_spc(const uint _spc_id, const Genotype& _gen)
	{
		species[_spc_id] = std::make_shared<Species>(_spc_id, _gen, *this);
		return species[_spc_id];
	}

	NetPtr Ecosystem::insert_net(const uint _net_id, const uint _spc_id)
	{
		nets[_net_id] = std::make_shared<Net>(_net_id, species.at(_spc_id), *this);
		return nets[_net_id];
	}
}
