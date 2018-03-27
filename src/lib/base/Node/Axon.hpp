#ifndef CORTEX_AXON_HPP
#define CORTEX_AXON_HPP

#include "Globals.hpp"
#include "Stat.hpp"

namespace Cortex
{
	struct Axon
	{
		real time;
		Stat val;

		Axon(StatConf& _conf);
	};
}

#endif // CORTEX_AXON_HPP
