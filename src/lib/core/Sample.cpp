#include "Sample.hpp"

namespace Cortex
{
	Sample::Sample()
	{}
	Sample::Sample(const std::vector<real>& _input, const uint _label)
		:
		  input(_input),
		  label(_label)
	{}

	Sample::Sample(std::vector<real>&& _input, const uint _label)
		:
		  input(std::move(_input)),
		  label(_label)
	{}
} // namespace Cortex
