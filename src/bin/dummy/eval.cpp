#include "eval.hpp"

namespace Dummy
{
	void eval(Net& _net)
	{
		dlog() << "Evaluating network " << _net.id;

		std::this_thread::sleep_for(std::chrono::milliseconds(_net.conf.rnd_int(50, 100)));
//		_net.eval(_net.conf.data->rnd_sample());

//		auto out(_net.get_output());

//		real fit(std::accumulate(out.begin(), out.end(), 0.0));

//		_net.set_fitness(fit);

		//		dlog() << "Network " << _net.id << " absolute fitness: " << _net.get_abs_fitness();
	}

	bool setup(Conf& _conf)
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

		_conf.validate();

		dlog() << _conf;

		return true;
	}
}
