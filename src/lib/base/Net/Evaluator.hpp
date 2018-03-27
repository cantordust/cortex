#ifndef CORTEX_EVALUATOR_HPP
#define CORTEX_EVALUATOR_HPP

#include "Layer.hpp"

namespace Cortex
{

	class Evaluator
	{
	private:

		/// Containing network
		Net& net;

		std::vector<Layer> layers;

	public:

		Evaluator(Net& _net);

		void make_layers();

	private:

		friend class Net;
	};
}

#endif // CORTEX_EVALUATOR_HPP
