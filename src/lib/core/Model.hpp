#ifndef CORTEX_MODEL_HPP
#define CORTEX_MODEL_HPP

#include "Globals.hpp"

namespace Cortex
{
	class Model
	{

	protected:

		Net& net;

		struct Layer
		{
//			/// Synaptic weights (regular layers) or
//			/// filters (convolutional layers).
//			Mat weights;

//			/// Input matrix.
//			Mat input;

//			/// Output matrix.
//			Mat output;

//			/// Gradient matrix.
//			Mat gradient;

			Layer(const uint _nodes);
		};

		std::vector<Layer> layers;

		/// Evaluation scheduler (for spiking networks only).
		Spike::Scheduler scheduler;

	public:

		///=====================================
		/// Constructor and destructor
		///=====================================

		Model(Net& _net);

		///=====================================
		/// Structural operations
		///=====================================

		/// @brief Compile the model from a network description.
		void compile(const Net& _net);

		///=====================================
		/// Evaluation
		///=====================================

		/// @brief Dispatch function for network evaluation.
		/// For spiking networks, the input represents spike times
		/// in the case of direct encoding or relative order in case
		/// of rank order encoding.
		template<NetType = NetType::Undef>
		void evaluate(const Sample& _sample);

		/// @brief Evaluate	a single node.
		/// Used for evaluating spiking networks.
		template<SpikeEnc = SpikeEnc::Undef>
		void evaluate(const Spike& _spike);

//		/// @brief Extract the output of the network.
//		/// @return A real-valued vector.
//		Mat get_output();

		template<SensorType rf = SensorType::Undef>
		void convert(const Sample& _sample) const;

//		Mat stretch(const Mat& _image, const uint _max_fs);

		///=====================================
		/// Friends
		///=====================================

		friend class Net;
	};
}

#endif // CORTEX_MODEL_HPP
