#include "eval.hpp"

namespace Dummy
{
	void eval(Net& _net)
	{
		dlog() << "Evaluating network " << _net.id;
		std::vector<real> in;
		for (uint i = 0; i < _net.node_count(NR::I); ++i)
		{
			in.emplace_back(i + 1.0);
		}
		_net.eval(in);

		auto out(_net.get_output());

		real fit(std::accumulate(out.begin(), out.end(), 0.0, [](real _sum, const real _x){ return _sum + _x; }));

		_net.set_abs_fitness(fit);

		//		dlog() << "Network " << _net.id << " absolute fitness: " << _net.get_abs_fitness();
	}

	bool setup(Config& _config)
	{
		return _config.validate();
	}
}
