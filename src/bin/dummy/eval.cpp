#include "eval.hpp"

namespace Dummy
{
	bool setup()
	{
		Fitness::set_target(3.0);
		return true;
	}

	void evaluate(const NetPtr _net)
	{
		dlog("Evaluating network ", _net->id);

		_net->set_fitness(rnd_pos_nd(0.5));

		std::this_thread::sleep_for(std::chrono::milliseconds(rnd_int(50, 100)));
	}
}
