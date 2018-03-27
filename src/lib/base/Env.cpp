#include "Conf.hpp"
#include "Env.hpp"
#include "Species.hpp"
#include "Net.hpp"

namespace Cortex
{
	Env::Env(Conf& _conf, const EvalFunc& _ef)
		:
		  age(1),
		  conf(_conf),
		  tp(_conf.sys.threads),
		  ef(_ef),
		  solved(false),
		  champ(nullptr),
		  evals(0)
	{
		dlog() << "Environment created in thread " << std::this_thread::get_id();
	}

	template<>
	void Env::eval<OptMode::Evolution>()
	{
		print_stats();

		uint round(0);
		do
		{
			dlog() << "\tRound " << round + 1;
			for (const auto& net : nets)
			{
				tp.enqueue(std::forward<EvalFunc>(ef), std::ref(*net.second));
			}

			/// Wait for all networks to be evaluated.
			tp.wait();

			mutate();
		} while(conf.mut.rate > 0 &&
				++round < conf.mut.rate);

		evolve();
	}

	/// @todo Implementation
	template<>
	void Env::eval<OptMode::GD>()
	{
		for (const auto& net : nets)
		{
			tp.enqueue(std::forward<EvalFunc>(ef), std::ref(*net.second));
		}

		/// Wait for all networks to be evaluated.
		tp.wait();
	}

	/// @todo Implementation
	template<>
	void Env::eval<OptMode::Hybrid>()
	{
		for (const auto& net : nets)
		{
			tp.enqueue(std::forward<EvalFunc>(ef), std::ref(*net.second));
		}

		/// Wait for all networks to be evaluated.
		tp.wait();
	}

	template<>
	bool Env::init<EnvType::Population>()
	{
		uint spc_id(0);
		uint net_id(0);

		uint nets_per_spc( conf.env.init.size / conf.spc.init.count );

		for (uint s = 0; s < conf.spc.init.count; ++s)
		{
			/// Generate a species ID
			spc_id = conf.spc.next_id();

			/// Generate a genotype for the species
			Genotype gen(conf);

			/// Set the number of input receptors for the genome
			gen.set(NR::I, gen.count(NR::I) * conf.net.rf.size);

			/// Set the number of hidden neurons for the genome
			gen.add(NR::H, s);

			/// Insert the species into the environment
			insert_spc(spc_id, gen);

			/// Generate networks
			for (uint n = 0; n < nets_per_spc; ++n)
			{
				net_id = conf.net.next_id();
//				dlog() << "Generating network " << net_id;
				insert_net(net_id, species.at(spc_id));
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

	template<>
	bool Env::init<EnvType::Single>()
	{
		uint net_id(conf.net.next_id());

		dlog() << "Generating network " << net_id;
		insert_net(net_id);
		nets.at(net_id)->init();
//		dlog() << *nets.at(net_id);

		return true;
	}

	const SpcPtr Env::get_spc(const Genotype& _gen)
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
			/// Register the new species with the Environment.
			return insert_spc(conf.spc.next_id(), _gen);
		}

		return nullptr;
	}

	bool Env::is_solved()
	{
		return solved;
	}

	void Env::mark_solved(const uint _net_id)
	{
		if (!solved)
		{
			champ = nets.at(_net_id);
			solved = true;
			tp.stop();
		}
	}

	const NetPtr Env::get_champ()
	{
		return champ;
	}

	void Env::inc_evals()
	{
		++evals;
	}

	uint Env::get_evals()
	{
		return evals;
	}

	void Env::print_champ()
	{
		if (champ)
		{
			dlog() << "Champion:\n" << champ->id;
		}
	}

	void Env::print_stats()
	{
		dlog() << "\n------------------------"
			   << "\nGeneration: " << age
			   << "\nSpecies count: " << species.size()
			   << "\nNetwork count: " << nets.size()
			   << "\nHighest fitness: " << (champ ? champ->fitness.abs.last : 0.0)
			   << "\n\n*** Evaluating networks...";
	}

	uint Env::get_net_count() const
	{
		return nets.size();
	}

	uint Env::get_spc_count() const
	{
		return species.size();
	}

	const hmap<uint, NetPtr>& Env::get_nets() const
	{
		return nets;
	}

	const NetPtr Env::get_net(const uint _net_id)
	{
		return (nets.find(_net_id) == nets.end()) ? nullptr : nets.at(_net_id);
	}

	const SpcPtr Env::get_spc(const uint _spc_id)
	{
		return (species.find(_spc_id) == species.end()) ? nullptr : species.at(_spc_id);
	}

	void Env::erase_net(const uint _net_id)
	{
		species.at(nets.at(_net_id)->spc->id)->erase_net(_net_id);
		nets.erase(_net_id);
	}

	void Env::evolve()
	{
		/// Statistics
		dlog() << "Total evaluations: " << evals;

		/// Increase the age of the environment by 1
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
			dlog() << "*** Evolving environment";

			find_champ();
			cleanup();
			update_fitness();

			if (conf.mating.enabled)
			{
				crossover();
			}
		}
	}

	void Env::find_champ()
	{
//		dlog() << "Finding global champion...";

		real max_fit(0.0);
		champ = nullptr;

		for (const auto& net : nets)
		{
			if (champ == nullptr ||
				net.second->fitness.abs.last > max_fit)
			{
				champ = net.second;
				max_fit = champ->fitness.abs.last;
			}
		}

//		print_champ();
	}

	void Env::update_fitness()
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
										   Logistic(spc_stat.get_offset( spc.second->fitness.abs.last ) ) );
		}

		for (const auto& spc : species)
		{
			dlog() << "Species " << spc.second->id << ":"
				   << "\n\tAbsolute fitness: " << spc.second->fitness.abs.last
				   << "\n\tRelative fitness: " << spc.second->fitness.rel.last;
		}
	}

	void Env::cleanup()
	{
		/// Remove empty species
		for (auto it = species.begin(); it != species.end(); )
		{
			(it->second->is_empty()) ? it = species.erase(it) : ++it;
		}
	}

	void Env::crossover()
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
		/// portion of the current environment size.
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
			insert_net(new_net_id, species.at(spc_id));

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
		/// of the environment is reached.
		while (nets.size() > conf.env.max.size)
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

	void Env::mutate()
	{
		if (conf.mut.rate > 0)
		{
			dlog() << "Mutating networks...";
			for (const auto& spc : species)
			{
				spc.second->mutate();
			}
		}
	}

	SpcPtr Env::insert_spc(const uint _spc_id, const Genotype& _gen)
	{
		species[_spc_id] = std::make_shared<Species>(_spc_id, _gen, *this);
		return species[_spc_id];
	}

	NetPtr Env::insert_net(const uint _net_id, const SpcPtr _spc)
	{
		nets[_net_id] = std::make_shared<Net>(_net_id, _spc, *this);
		return nets[_net_id];
	}

	///=========================================
	///	Dispatch functions
	///=========================================

	bool Env::init()
	{
		conf.validate();

		species.clear();
		nets.clear();
		conf.reset_ids();

		switch (conf.env.type)
		{
		case EnvType::Population:
			return init<EnvType::Population>();

		case EnvType::Single:
			return init<EnvType::Single>();

		default:
			return false;
		}
	}

	void Env::eval()
	{
		print_stats();

		switch (conf.env.opt)
		{
		case OptMode::Evolution:
			return eval<OptMode::Evolution>();

		case OptMode::GD:
			return eval<OptMode::GD>();

		case OptMode::Hybrid:
			return eval<OptMode::Hybrid>();

		default:
			return;
		}
	}
}
