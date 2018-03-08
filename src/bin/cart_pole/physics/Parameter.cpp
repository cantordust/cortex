#include "Parameter.hpp"

namespace CartPole
{
	Parameter::Parameter(const real _init,
						 const real _max)
		:
		  cur(_init),
		  tmp(_init),
		  max(_max),
		  k1(0.0),
		  k2(0.0),
		  k3(0.0)
	{}

}
