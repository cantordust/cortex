#ifndef EVAL_HPP
#define EVAL_HPP

#include "Cortex.hpp"

using namespace Cortex;

namespace Xor
{
	// The boundary should be 0 for tanh
	// and 0.5 for the logistic function.
	// Changed in setup_config().
	static real boundary;

	bool setup(Config& _config);

	const std::vector<std::vector<real>> input =
	{
		{0.0, 0.0}, // 0
		{1.0, 0.0}, // 1
		{0.0, 1.0}, // 1
		{1.0, 1.0}  // 0
	};

	void eval(Net& _net);
}

#endif // EVAL_HPP
