#ifndef DUMMY_EVAL_HPP
#define DUMMY_EVAL_HPP

#include "Cortex.hpp"

namespace Dummy
{
	using namespace Cortex;

	bool setup();

	template<Stage _stage>
	void eval(const NetPtr _net);
}

#endif // DUMMY_EVAL_HPP
