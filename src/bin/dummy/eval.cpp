#include "eval.hpp"

namespace Dummy
{
	bool setup()
	{
		Fitness::set_target(3.0);
		return true;
	}

	template<>
	void eval<Stage::Train>(const NetPtr _net)
	{
		_net->set_fitness(rnd_pos_nd(1.0));
	}

//	template<>
//	void eval<Stage::Dev>(const NetPtr _net)
//	{
//		_net.set_fitness(rnd_pos_nd(0.5));
//	}
}

namespace Cortex
{
	void Task::evaluate(const NetPtr _net)
	{
		dlog("Evaluating network ", _net->id);

		switch (_net->stage)
		{
		case Stage::Train:
			Dummy::eval<Stage::Train>(_net);
			break;

//		case Stage::Dev:
//			Dummy::eval<Stage::Dev>(_net);
//			break;

//		case Stage::Test:
//			Dummy::eval<Stage::Test>(_net);
//			break;

		default:
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(rnd_int(50, 100)));
	}
}
