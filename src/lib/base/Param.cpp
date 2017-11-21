#include "Param.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	void Param::flip_act()
	{
		if (act == Act::Inc)
		{
			act = Act::Dec;
		}
		else if (act == Act::Dec)
		{
			act = Act::Inc;
		}
		dec_sd();
	}

	Param::Param(Config& _cfg, Config::ParamCfg& _stats)
		:
		  cfg(_cfg),
		  stats(_stats),
		  importance(0.0)
	{
		stats.val = cfg.rnd_nd(stats.mean, stats.sd);
	}

	void Param::set_trend(const Eff _eff)
	{
		/// Flip the action if the fitness has decreased
		if (_eff == Eff::Dec)
		{
			flip_act();
		}
		else if (_eff == Eff::Undef)
		{
			act = Act::Undef;
		}
	}

	void Param::anneal(const real _abs_fit)
	{
		/// Calculate the temperature to
		/// determine the coarseness of the mutation.
		/// Add a small amount of Gaussian noise,
		/// otherwise the SD will end up very close to 0
		/// for individuals with fitness close to the target fitness.
		stats.sd *= std::fabs(cfg.rnd_nd(0.0, 0.01)) + 1.0 - _abs_fit / cfg.fit.tgt;
	}

	bool Param::mutate(Fitness& _fit)
	{
		_fit.add_param(*this);

		real delta(0.0);

		do
		{
			delta = cfg.rnd_nd(0.0, stats.sd);
		} while (delta == 0.0);

		switch (cfg.mutation.opt)
		{

		case Opt::Trend:
			switch (act)
			{
			case Act::Inc:
				stats.val += std::fabs(delta);
				break;

			case Act::Dec:
				stats.val -= std::fabs(delta);
				break;

			case Act::Undef:
				stats.val += delta;
				act = (delta > 0.0 ? Act::Inc : Act::Dec);
				break;
			}
			break;

		case Opt::Anneal:
		default:
			stats.val += delta;

		}

		/// \todo Can a mutation fail (return false;)?
		return true;
	}
}
