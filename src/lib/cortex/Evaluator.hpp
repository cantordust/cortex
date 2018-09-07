#ifndef CORTEX_EVALUATOR_HPP
#define CORTEX_EVALUATOR_HPP

#include "RField.hpp"

namespace Cortex
{
	class Evaluator
	{

	private:

		/// Input matrix
		Mat input;

		/// Synaptic weights (regular layers) or
		/// filters (convolutional layers).
		Mat weights;

		/// Output matrix
		Mat output;

		/// Gradient matrix.
		Mat grad;

		/// Evaluation scheduler (for spiking networks only).
		Spike::Scheduler scheduler;

		/// Receptive field.
		RField rfield;

	public:

		void compile(const Net& _net);

		///=====================================
		/// Evaluation
		///=====================================

		template<NetType nt = NetType::Undef>
		void eval();

		template<SpikeEnc enc = SpikeEnc::Undef>
		void eval(const Link& _link);

		/// @brief Extract the output of the network.
		/// @return A real-valued vector.
		Mat get_output();

		/// @brief Dispatch function for network evaluation.
		/// For spiking networks, the input represents spike times
		/// in the case of direct encoding or relative order in case
		/// of rank order encoding.
		void eval(const Sample& _sample);

		///=====================================
		/// Evaluation
		///=====================================

		template<NetType nt = NetType::Undef>
		void eval(const Mat& _sample);

		template<SpikeEnc enc = SpikeEnc::Undef>
		void eval();

		template<RFType rf = RFType::Undef>
		void convert(const Mat& _input);

		Mat eval();
	};
}

#endif // CORTEX_EVALUATOR_HPP
