#ifndef CORTEX_EXPERIMENT_HPP
#define CORTEX_EXPERIMENT_HPP

#include "Env.hpp"
#include "Net.hpp"

namespace Cortex
{
	class Experiment
	{
	private:

		/// @brief Configuration for this experiment
		Conf& conf;

		/// @brief Flag indicating whether the experiment is running.
		flag running;

		/// @brief Threads containing experiments.
		std::thread experiment;

		/// @brief Statistics about the number of evaluations
		Stat evals;

		/// @brief Statistics about the number of generations
		Stat gens;

		/// @brief Statistics about the number of hidden nodes
		Stat hidden_nodes;

	public:

		Experiment(Conf& _conf);

		/// @brief Set up the experiment with the provided
		/// evaluation function
		void setup(const EvalFunc& _ef);

		/// @brief Run the experiment
		void run();

		void print_summary();

	};
}

#endif // CORTEX_EXPERIMENT_HPP
