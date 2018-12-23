#ifndef CORTEX_SAMPLE_HPP
#define CORTEX_SAMPLE_HPP

#include "Tensor.hpp"
#include "Globals.hpp"

namespace Cortex
{
	struct Sample
	{
		/// Input tensor
		std::vector<real> input;

		/// Label for this sample (optional)
		uint label;

		Sample();

		explicit Sample(const std::vector<real>& _input,
						const uint _label = 0);

		explicit Sample(std::vector<real>&& _input,
						const uint _label = 0);
	};
}

#endif // CORTEX_SAMPLE_HPP
