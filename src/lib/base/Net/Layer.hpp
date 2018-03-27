#ifndef CORTEX_LAYER_HPP
#define CORTEX_LAYER_HPP

#include "Globals.hpp"

namespace Cortex
{
	class Layer
	{
	private:

		/// @brief Layer configuration
		LayerConf& conf;

		/// @brief Result of the forward pass
		vec output;

		/// @brief Synaptic weights
		mat weights;

	public:

		Layer(LayerConf& _conf);

	private:

		void init();

		template<LayerType lt>
		void init();

		friend class Evaluator;
	};
}

#endif // CORTEX_LAYER_HPP
