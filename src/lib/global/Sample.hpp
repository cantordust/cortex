#ifndef CORTEX_SAMPLE_HPP
#define CORTEX_SAMPLE_HPP

#include "Globals.hpp"

namespace Cortex
{
	struct Sample
	{
		/// Input variables
		std::vector<std::vector<real>> input;

		/// Label for this sample (optional)
		uint label;

		Sample(const std::vector<real>& _input,
			   const uint _label = 0);

		Sample(std::vector<real>&& _input,
			   const uint _label = 0);

		Sample(const std::vector<std::vector<real>>& _input,
			   const uint _label = 0);

		Sample(std::vector<std::vector<real>>&& _input,
			   const uint _label = 0);

		uint size() const;

	};
}

#endif // CORTEX_SAMPLE_HPP
