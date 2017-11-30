#include "Param.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	Param::Param(const ConfigRef& _cfg, const ParamConf& _pconf)
		:
		  cfg(_cfg),
		  pconf(_pconf),
		  importance(0.0)
	{
		switch (pconf.dist)
		{
		case InitDist::Normal:
			pconf.init = cfg.get().rnd_nd(pconf.mean, pconf.sd);
			break;

		case InitDist::Uniform:
			pconf.init = cfg.get().rnd_real(pconf.min, pconf.max);
			break;

		case InitDist::Fixed:
		default:
			break;
		}

		if (pconf.init < pconf.min)
		{
			pconf.init = pconf.min;
		}

		if (pconf.init > pconf.max)
		{
			pconf.init = pconf.max;
		}

		stat.update(pconf.init);
	}

	inline const Stat& Param::get_stat() const
	{
		return stat;
	}

	inline void Param::set(const real _cur)
	{
		stat.update(_cur);
	}

	inline void Param::update(const real _delta)
	{
		stat.update(stat.abs + _delta);
	}

	inline real Param::cur() const
	{
		return stat.abs;
	}

	inline void Param::inc_sd()
	{
		pconf.sd *= (1.0 + cfg.get().mutation.scale);
	}

	inline void Param::dec_sd()
	{
		pconf.sd *= (1.0 - cfg.get().mutation.scale);
	}

	inline void Param::set_trend(const Eff _eff)
	{
		// Flip the action if the fitness has decreased
		if (_eff == Eff::Dec)
		{
			flip_act();
		}
		else if (_eff == Eff::Undef)
		{
			act = Act::Undef;
		}
	}

	inline void Param::anneal(const real _abs_fit)
	{
		// Calculate the temperature to determine the coarseness of the mutation.
		// Add a small amount of Gaussian noise, otherwise the SD will end up very close
		// to 0 for individuals with fitness close to the target fitness.
		pconf.sd *= std::fabs(cfg.get().rnd_nd(0.0, 0.01)) + 1.0 - _abs_fit / cfg.get().fit.tgt;
	}

	bool Param::mutate(Fitness& _fit)
	{
		_fit.add_param(*this);

		real delta(0.0);

		do
		{
			delta = cfg.get().rnd_nd(0.0, pconf.sd);
		} while (delta == 0.0);

		switch (cfg.get().mutation.opt)
		{
		case Opt::Trend:
			switch (act)
			{
			case Act::Inc:
				stat.update(stat.abs + std::fabs(delta));
				break;

			case Act::Dec:
				stat.update(stat.abs - std::fabs(delta));
				break;

			case Act::Undef:
				stat.update(stat.abs + delta);
				act = (delta > 0.0 ? Act::Inc : Act::Dec);
				break;
			}
			break;

		case Opt::Anneal:
		default:
			stat.update(stat.abs + delta);

		}

		// @todo Can a mutation fail (return false;)?
		return true;
	}
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
}
