#ifndef ECOSYSTEM_HPP
#define ECOSYSTEM_HPP

#include "Net.hpp"

namespace Cortex
{
	class Ecosystem
	{
	public:

		/// The age is equal to the
		/// number of evolution rounds.
		uint age;

		Config& cfg;

	private:

		/// Threadpool for evaluating networks in parallel
		ThreadPool tp;

		/// A mapping of species IDs to species objects.
		emap<uint, Species> species;

		/// A mapping of network IDs to network objects.
		emap<uint, Net> nets;

//		/// Substrates which represent the layouts
//		/// of the input and output nodes.
//		/// For example, input nodes can be arranged into
//		/// a sensor grid (e.g., mimicking the retina), and
//		/// the output substrate can be mapped to robot effectors.
//		Substrate sub_in;
//		Substrate sub_out;

		struct
		{
			std::mutex mtx;
			std::condition_variable semaphore;

			/// A toggle variable indicating
			/// whether the task has been solved
			bool solved = false;

			/// The network with the highest fitness
			uint champ_id = 0;

			/// The total number of evaluations for all networks.
			uint total_evals = 0;

			/// Statistics for species
			Stat species;

		} stats;

	public:

		Ecosystem(Config& _cfg);

		~Ecosystem();

		bool init();

		uint get_species_id(const Genotype& _g);

		template<typename F, typename ... Args>
		inline void eval(F&& _f, Args&& ... _args)
		{
			dlog() << "\n------------------------"
				   << "\nGeneration: " << age
				   << "\nSpecies count: " << species.size()
				   << "\nNetwork count: " << nets.size()
				   << "\nHighest fitness: " << ((stats.champ_id == 0) ? 0.0 : nets.at(stats.champ_id).get_abs_fitness())
				   << "\n\n*** Evaluating networks...";

			for (uint i = 0; i < cfg.mutation.rate; ++i)
			{
				dlog() << "\tRound " << i + 1;
				for (auto& net : nets)
				{
					tp.enqueue(std::forward<F>(_f), std::ref(net.second), std::forward<Args>(_args)...);
				}

				/// Wait for all networks to be evaluated.
				tp.wait();

				if (cfg.mutation.enabled)
				{
					mutate();
				}
			}

			/// Statistics
			dlog() << "Total evaluations: " << stats.total_evals;

			/// Increase the age of the ecosystem by 1
			++age;
			for (auto& net : nets)
			{
				++net.second.age;
			}

			if (is_solved())
			{
				print_champ();
			}
			else
			{
				/// Evolve the ecosystem
				dlog() << "*** Evolving ecosystem";
				evolve();
			}
		}

		inline bool is_solved()
		{
			glock lk(stats.mtx);
			return stats.solved;
		}

		inline void mark_solved(const uint _net)
		{
			{
				glock lk(stats.mtx);
				if (!stats.solved)
				{
					stats.champ_id = _net;
					stats.solved = true;
				}
			}
			tp.stop();
		}

		inline uint champ_id()
		{
			glock lk(stats.mtx);
			return stats.champ_id;
		}

		inline const Net& champ()
		{
			glock lk(stats.mtx);
			return nets.at(stats.champ_id);
		}

		inline void inc_evals()
		{
			glock lk(stats.mtx);
			++stats.total_evals;
		}

		inline uint total_evals()
		{
			glock lk(stats.mtx);
			return stats.total_evals;
		}

		inline void print_champ()
		{
			glock lk(stats.mtx);
			if (nets.find(stats.champ_id) != nets.end())
			{
				dlog() << "Champion:\n" << nets.at(stats.champ_id);
			}
		}

		inline uint get_age() const
		{
			return age;
		}

		inline uint net_count() const
		{
			return nets.size();
		}

		inline uint species_count() const
		{
			return species.size();
		}

		inline SpeciesRef get_species(const uint _spc_id)
		{
			return std::ref(species.at(_spc_id));
		}

		inline std::vector<NetRef> get_nets()
		{
			std::vector<NetRef> net_refs;
			for (auto& net : nets)
			{
				net_refs.emplace_back(net.second);
			}
			return net_refs;
		}

		inline void erase_net(const uint _net_id)
		{
			species.at(nets.at(_net_id).species.get().id).erase_net(_net_id);
			nets.erase(_net_id);
		}

	private:

		/// Evolve the ecosystem by applying the
		/// crossover and mutation procedures.
		void evolve();

		/// Find the network with the highest fitness.
		/// Used in case the current champion is erased.
		void find_champ();

		/// Update the fitness values of networks
		/// and compute the diversity of species.
		void update_fitness();

		/// Remove empty species
		void remove_empty_species();

		/// Perform crossover
		void crossover();

		/// Mutate networks which were not culled.
		void mutate();

		inline void insert_net(const uint _net_id, const uint _spc_id)
		{
			nets.emplace(std::piecewise_construct,
						 std::forward_as_tuple(_net_id),
						 std::forward_as_tuple(_net_id, *this, species.at(_spc_id)));
		}

		inline void insert_species(const uint _spc_id, const Genotype& _gen)
		{
			species.emplace(std::piecewise_construct,
							std::forward_as_tuple(_spc_id),
							std::forward_as_tuple(_spc_id, _gen, cfg));
		}

	};
}

#endif // ECOSYSTEM_HPP
