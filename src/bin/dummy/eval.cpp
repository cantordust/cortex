#include "eval.hpp"

namespace Dummy
{
	bool setup()
	{
		//		uint samples(1000);
		//		for (uint s = 0; s < samples; ++s)
		//		{
		//			std::vector<real> input;
		//			for (uint i = 0; i < _conf.node.roles.at(NR::I); ++i)
		//			{
		//				input.emplace_back(_conf.rnd_nd(0.0, 1.0));
		//			}
		//			_conf.data->add_sample(input);
		//		}

		//		conf->validate();

		//		dlog{*conf};

		return true;
	}
}

namespace Cortex
{
	template<>
	real Task::evaluate<Stage::Train>(Net& _net)
	{
		dlog("Evaluating network ", _net.id);

		std::this_thread::sleep_for(std::chrono::milliseconds(rnd_int(50, 100)));
		//		_net.eval(_net.conf.data->rnd_sample());

		//		auto out(_net.get_output());

		//		real fit(std::accumulate(out.begin(), out.end(), 0.0));
		real fit(0.0);

		//		dlog("Network ", _net.id, " absolute fitness: ", _net.get_abs_fitness());

		return fit;
	}
}
