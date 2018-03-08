#ifndef EVAL_HPP
#define EVAL_HPP

#include "Cortex.hpp"

using namespace Cortex;

namespace WDBC
{
	bool setup(Conf& _conf);

	void eval(Net& _net);

	bool load_data(Conf& _config);

	bool split_data_sets(Conf& _config);

	void test(Conf& _config);
}

#endif // EVAL_HPP
