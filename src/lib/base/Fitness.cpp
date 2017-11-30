#include "Param.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	Fitness::Fitness(Config& _cfg)
		:
		  cfg(_cfg),
		  eff(Eff::Undef)
	{
		stat.ema_coeff = cfg.fit.ema.coeff;
		stat.window_size = std::floor(2.0 / stat.ema_coeff) - 1;
	}

	void Fitness::set_cur(const real _cur)
	{
		if (_cur > stat.abs)
		{
			eff = Eff::Inc;
		}
		else if (_cur < stat.abs)
		{
			eff = Eff::Dec;
		}
		else
		{
			eff = Eff::Undef;
		}
		stat.update(_cur);
		feedback();
	}

	void Fitness::feedback()
	{
		switch (cfg.mutation.opt)
		{
		case Opt::Anneal:
			for (auto&& param : params)
			{
				param.get().anneal(stat.abs);
			}
			break;

		case Opt::Trend:
			for (auto&& param : params)
			{
				param.get().set_trend(eff);
			}
			break;

		default:
			break;
		}

		// Increase the SD if we haven't made any progress
		if (progress() <= 0.0)
		{
			for (auto&& param : params)
			{
				param.get().inc_sd();
			}
		}

		params.clear();
	}
}
