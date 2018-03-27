#ifndef CORTEX_ENV_HPP
#define CORTEX_ENV_HPP

#include "Conf.hpp"

namespace Cortex
{
	class Env
	{
	public:

		/// Number of generations
		uint age;

		Conf& conf;

	private:

		/// @brief Threadpool for evaluating networks in parallel
		ThreadPool tp;

		/// @brief Network evaluation function
		EvalFunc ef;

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

	public:

		Env(Conf& _conf, const EvalFunc& _ef);

		bool init();

		void eval();

		bool is_solved();

		void mark_solved(const uint _net);

		const NetPtr get_champ();

		void inc_evals();

		uint get_evals();

		void print_champ();

		void print_stats();

		uint get_net_count() const;

		uint get_spc_count() const;

		const hmap<uint, NetPtr>& get_nets() const;

		const NetPtr get_net(const uint _net_id);

		const SpcPtr get_spc(const Genotype& _gen);

		const SpcPtr get_spc(const uint _spc_id);

		void erase_net(const uint _net_id);

		/// @brief Evolve the environment by performing crossover.
		void evolve();

		/// @brief Perform crossover.
		void crossover();

		/// @brief Mutate networks.
		void mutate();

	private:

		/// @brief Dispatch based on
		/// optimisation mode, ecosystem type and layout.
		template<EnvType et>
		bool init();

		/// @brief Evaluate the networks contained
		/// in the environment
		template<OptMode om>
		void eval();

		/// @brief Find the network with the highest fitness.
		/// Used in case the current champion is erased.
		void find_champ();

		/// @brief Update the fitness values of networks
		/// and compute the diversity of species.
		void update_fitness();

		/// @brief Remove empty species.
		void cleanup();

		/// @brief Insert a new species into the environment
		/// with ID @p _spc_id and genotype @p _gen.
		/// @param _spc_id The next available species ID.
		/// @param _gen Genotype of the new species.
		SpcPtr insert_spc(const uint _spc_id, const Genotype& _gen);

		/// @brief Insert a new network into the environment
		/// with ID @p _net_id and species ID @p _spc_id
		///
		/// @param _net_id The next available network ID.
		/// @param _spc_id ID of the species that this
		/// network belongs to.
		NetPtr insert_net(const uint _net_id, const SpcPtr _spc = nullptr);
	};
}

#endif // CORTEX_ENV_HPP
