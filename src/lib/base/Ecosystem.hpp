#ifndef ECOSYSTEM_HPP
#define ECOSYSTEM_HPP

#include "Net.hpp"

namespace Cortex
{
	class Ecosystem
	{
	public:

		// The age is equal to the
		// number of evolution rounds.
		uint age;

		ConfigRef cfg;

	private:

		// Threadpool for evaluating networks in parallel
		ThreadPool tp;

		// A mapping of species IDs to species objects.
		hmap<uint, Species> species;

		// A mapping of network IDs to network objects.
		hmap<uint, Net> nets;

//		// Substrates which represent the layouts
//		// of the input and output nodes.
//		// For example, input nodes can be arranged into
//		// a sensor grid (e.g., mimicking the retina), and
//		// the output substrate can be mapped to robot effectors.
//		Substrate sub_in;
//		Substrate sub_out;

		struct
		{
			// Synchronisation variables
			std::mutex mtx;
			std::condition_variable semaphore;

			// A toggle variable indicating
			// whether the task has been solved
			bool solved = false;

			// The network with the highest fitness
			uint champ_id = 0;

			// The total number of evaluations for all networks.
			uint total_evals = 0;

			// Statistics for species
			Stat species;

		} stat;

	public:

		Ecosystem(Config& _cfg);

		~Ecosystem();

		template<typename F, typename ... Args>
		inline void eval(F&& _f, Args&& ... _args)
		{
			dlog() << "\n------------------------"
				   << "\nGeneration: " << age
				   << "\nSpecies count: " << species.size()
				   << "\nNetwork count: " << nets.size()
				   << "\nHighest fitness: " << ((stat.champ_id == 0) ? 0.0 : nets.at(stat.champ_id).get_abs_fitness())
				   << "\n\n*** Evaluating networks...";

			for (uint i = 0; i < cfg.get().mutation.rate; ++i)
			{
				dlog() << "\tRound " << i + 1;
				for (auto&& net : nets)
				{
					tp.enqueue(std::forward<F>(_f), std::ref(net.second), std::forward<Args>(_args)...);
				}

				// Wait for all networks to be evaluated.
				tp.wait();

				if (cfg.get().mutation.enabled)
				{
					mutate();
				}
			}

			// Statistics
			dlog() << "Total evaluations: " << stat.total_evals;

			// Increase the age of the ecosystem by 1
			++age;
			for (auto&& net : nets)
			{
				++net.second.age;
			}

			if (is_solved())
			{
				print_champ();
			}
			else
			{
				// Evolve the ecosystem
				dlog() << "*** Evolving ecosystem";
				evolve();
			}
		}

		bool init();

		uint get_species_id(const Genotype& _g);

		bool is_solved();

		void mark_solved(const uint _net);

		uint get_champ_id();

		const Net& get_champ();

		void inc_evals();

		uint get_total_evals();

		void print_champ();

		uint get_net_count() const;

		uint get_species_count() const;

		SpeciesRef get_species(const uint _spc_id);

		std::vector<NetRef> get_nets();

		void erase_net(const uint _net_id);

	private:

		// Evolve the ecosystem by applying the
		// crossover and mutation procedures.
		void evolve();

		// Find the network with the highest fitness.
		// Used in case the current champion is erased.
		void find_champ();

		// Update the fitness values of networks
		// and compute the diversity of species.
		void update_fitness();

		// Remove empty species
		void remove_empty_species();

		// Perform crossover
		void crossover();

		// Mutate networks which were not culled.
		void mutate();

		void insert_net(const uint _net_id, const uint _spc_id);

		void insert_species(const uint _spc_id, const Genotype& _gen);

	};
}

#endif // ECOSYSTEM_HPP
