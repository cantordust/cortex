#include "Sample.hpp"

namespace Cortex
{
	Sample::Sample(const std::vector<real>& _input, const uint _label)
		:
		  label(_label)
	{
		for (uint i = 0; i < _input.size(); ++i)
		{
			input.push_back({_input[i]});
		}
	}

	Sample::Sample(std::vector<real>&& _input, const uint _label)
		:
		  label(_label)
	{
		for (uint i = 0; i < _input.size(); ++i)
		{
			input.push_back({std::move(_input[i])});
		}
	}

	Sample::Sample(const std::vector<std::vector<real>>& _input,
				   const uint _label)
		:
		  input(_input),
		  label(_label)
	{}

	Sample::Sample(std::vector<std::vector<real>>&& _input,
				   const uint _label)
		:
		  input(std::move(_input)),
		  label(_label)
	{}

	uint Sample::size() const
	{
		return input.size();
	}
}
