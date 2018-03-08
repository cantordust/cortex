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

		Axon(Stat& _stat);
	};
}

#endif // CORTEX_AXON_HPP
