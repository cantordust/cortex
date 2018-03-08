#include "Vars.hpp"

namespace CartPole
{
	real Rnd::jitter()
	{
		return nd(rng);
	}
}
