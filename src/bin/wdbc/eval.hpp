#ifndef EVAL_HPP
#define EVAL_HPP

#include "Cortex.hpp"

using namespace Cortex;

namespace WDBC
{
	bool setup(Conf& _conf);

	void eval(Net& _net);

	bool load_data(Conf& _conf);

	void test(Conf& _conf);
}

#endif // EVAL_HPP
