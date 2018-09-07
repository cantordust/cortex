#ifndef CORTEX_SAMPLE_HPP
#define CORTEX_SAMPLE_HPP

#include "Globals.hpp"

namespace Cortex
{
	struct Sample
	{
		/// Input variables
		Vec input;

		/// Label for this sample (optional)
		uint label;

		Sample();

		explicit Sample(const Vec& _input,
						const uint _label = 0);

		explicit Sample(Vec&& _input,
						const uint _label = 0);

		explicit Sample(const std::vector<real>& _input,
						const uint _label = 0);

		explicit Sample(std::vector<real>&& _input,
						const uint _label = 0);
	};
}

#endif // CORTEX_SAMPLE_HPP
