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

	void Fitness::set_abs(const real _abs_fit)
	{
		if (_abs_fit > stat.abs)
		{
			eff = Eff::Inc;
		}
		else if (_abs_fit < stat.abs)
		{
			eff = Eff::Dec;
		}
		else
		{
			eff = Eff::Undef;
		}
		stat.add(_abs_fit);
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

		/// Increase the SD if we haven't made any progress
		if (progress() <= 0.5)
		{
			for (auto&& param : params)
			{
				param.get().inc_sd();
			}
		}

		params.clear();
	}
}
