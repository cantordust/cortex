#ifndef EVAL_HPP
#define EVAL_HPP

#include "Cortex.hpp"

using namespace Cortex;

namespace Xor
{
	bool setup(Conf& _conf);

	void eval(Net& _net);
}

#endif // EVAL_HPP
