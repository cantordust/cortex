#ifndef CORTEX_ECOSYSTEM_HPP
#define CORTEX_ECOSYSTEM_HPP

#include "Conf.hpp"

namespace Cortex
{
	class Ecosystem
	{
	public:

		/// = Number of generations
		uint age;

		Conf& conf;

	private:

		/// Threadpool for evaluating networks in parallel
		ThreadPool tp;

		/// Species ID : species pointer
		hmap<uint, SpcPtr> species;

		/// Network ID : network pointer
		hmap<uint, NetPtr> nets;

		/// A toggle variable indicating
		/// whether the task has been solved
		flag solved;

		/// The network with the highest fitness
		NetPtr champ;

		/// The total number of evaluations for all networks.
		auint evals;

//		/// Substrates which represent the layouts
//		/// of the input and output nodes.
//		/// For example, input nodes can be arranged into
//		/// a sensor grid (e.g., mimicking the retina), and
//		/// the output substrate can be mapped to robot effectors.
//		Substrate in;
//		Substrate out;

	public:

		Ecosystem(Conf& _conf);

		template<typename F, typename ... Args>
		void eval(F&& _f, Args&& ... _args)
		{
			print_stats();

			for (uint i = 0; i < conf.mut.rate; ++i)
			{
				dlog() << "\tRound " << i + 1;
				for (const auto& net : nets)
				{
					tp.enqueue(std::forward<F>(_f), std::ref(*net.second), std::forward<Args>(_args)...);
				}

				/// Wait for all networks to be evaluated.
				tp.wait();

				dlog() << "Mutating networks...";

				if (conf.mut.enabled)
				{
					mutate();
				}
			}

			evolve();
		}

		bool init();

		bool is_solved();

		void mark_solved(const uint _net);

		const NetPtr get_champ();

		void inc_evals();

		uint get_evals();

		void print_champ();

		void print_stats();

		uint get_net_count() const;

		uint get_spc_count() const;

		const hmap<uint, NetPtr>& get_nets();

		const NetPtr get_net(const uint _net_id);

		const SpcPtr get_spc(const Genotype& _g);

		const SpcPtr get_spc(const uint _spc_id);

		void erase_net(const uint _net_id);

	private:

		/// @brief Evolve the ecosystem by applying the
		/// crossover and mutation procedures.
		void evolve();

		/// @brief Find the network with the highest fitness.
		/// Used in case the current champion is erased.
		void find_champ();

		/// @brief Update the fitness values of networks
		/// and compute the diversity of species.
		void update_fitness();

		/// @brief Remove empty species.
		void cleanup();

		/// @brief Perform crossover.
		void crossover();

		/// @brief Mutate networks.
		void mutate();

		/// @brief Insert a new network into the ecosystem
		/// with ID @p _net_id and species ID @p _spc_id
		///
		/// @param _net_id The next available network ID.
		/// @param _spc_id ID of the species that this
		/// network belongs to.
		NetPtr insert_net(const uint _net_id, const uint _spc_id);

		/// @brief Insert a new species into the ecosystem
		/// with ID @p _spc_id and genotype @p _gen.
		/// @param _spc_id The next available species ID.
		/// @param _gen Genotype of the new species.
		SpcPtr insert_spc(const uint _spc_id, const Genotype& _gen);

	};
}

#endif // CORTEX_ECOSYSTEM_HPP
