#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include "Globals.hpp"

namespace CartPole
{
	using namespace Cortex;

	/// cur			Current value
	/// tmp			Temporary value
	/// max			Maximal value
	/// k1 ~ k3		Estimated slopes
	class Parameter
	{
	public:

		real cur;
		real tmp;
		real max;
		real k1;
		real k2;
		real k3;

	public:

		Parameter(const real _init = 0.0,
				  const real _max = 0.0);
	};
}

#endif // PARAMETER_HPP
