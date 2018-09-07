#ifndef EVAL_HPP
#define EVAL_HPP

#include "Cortex.hpp"

using namespace Cortex;

namespace WDBC
{
	bool setup();

	void eval(Net& _net);

	bool load_data();

	void test();
}

#endif // EVAL_HPP
