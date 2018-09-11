#ifndef CORTEX_EVALUATOR_HPP
#define CORTEX_EVALUATOR_HPP

#include "RField.hpp"

namespace Cortex
{
	class Evaluator
	{

	protected:

		/// Input matrix
		Mat input;

		/// Synaptic weights (regular layers) or
		/// filters (convolutional layers).
		Mat weights;

		/// Output matrix
		Mat output;

		/// Gradient matrix.
		Mat gradient;

		/// Receptive field.
		RField rfield;

		/// Evaluation scheduler (for spiking networks only).
		Spike::Scheduler scheduler;

	public:

		///=====================================
		/// Structural operations
		///=====================================

		/// @brief Compile the evaluator from a network description.
		void compile(const Net& _net);

		///=====================================
		/// Evaluation
		///=====================================

		/// @brief Dispatch function for network evaluation.
		/// For spiking networks, the input represents spike times
		/// in the case of direct encoding or relative order in case
		/// of rank order encoding.
		void evaluate(const Sample& _sample);

		/// @brief Evaluate	a single node.
		/// Used for evaluating spiking networks.
		void evaluate(const Node& _node);

		/// @brief Extract the output of the network.
		/// @return A real-valued vector.
		Mat get_output();

		///=====================================
		/// Friends
		///=====================================

		friend class Net;
	};
}

#endif // CORTEX_EVALUATOR_HPP
