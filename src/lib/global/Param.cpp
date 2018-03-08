#include "Conf.hpp"
#include "Param.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	Param::Param(ParamConf& _pc)
		:
		  pc(_pc),
		  stat(_pc.stat),
		  sd(_pc.sd),
		  importance(0.0)
	{
		init();
	}

	Param::Param(const Param& _other)
		:
		  pc(_other.pc),
		  stat(_other.stat),
		  sd(_other.sd),
		  importance(0.0)
	{
		stat.reset();
		stat.update(_other.stat.cur);
	}

	void Param::init()
	{
		real init_val(0.0);

		switch (pc.dist)
		{
		case Dist::Normal:
			init_val = pc.conf.rnd_nd(pc.mean, pc.sd);
			break;

		case Dist::Uniform:
			init_val = pc.conf.rnd_real(pc.lbound, pc.ubound);
			break;

		case Dist::Fixed:
			init_val = pc.val;
			break;

		default:
			break;
		}

		if (init_val <= pc.lbound ||
			init_val >= pc.ubound)
		{
			init_val = (pc.ubound - pc.lbound) / 2.0;
		}

		stat.update(init_val);
	}

	void Param::update(const real _new_val)
	{
		stat.update(_new_val);
	}

	real Param::val() const
	{
		return stat.cur;
	}

	void Param::inc_sd()
	{
		sd *= (1.0 + pc.conf.mut.scale);
	}

	void Param::dec_sd()
	{
		sd *= (1.0 - pc.conf.mut.scale);
	}

	template<>
	void Param::optimise<Opt::Anneal>(const Fitness& _fit)
	{
		/// Calculate the temperature to determine the coarseness of the mutation.
		/// Add a small amount of Gaussian noise, otherwise the SD will end up very close
		/// to 0 for individuals with fitness close to the target fitness.
		sd = std::fabs(pc.conf.rnd_nd(0.0, 0.05)) + 1.0 - _fit.abs.cur / pc.conf.fit.tgt;
	}

	template<>
	void Param::optimise<Opt::Trend>(const Fitness& _fit)
	{
		/// Flip the action if the fitness has decreased
		if (_fit.eff == Eff::Dec)
		{
			flip_act();
		}
		else if (_fit.eff == Eff::Undef)
		{
			act = Act::Undef;
		}
	}

	bool Param::mutate(Fitness& _fit)
	{
		_fit.add_param(*this);

		real delta(0.0);

		do
		{
			delta = pc.conf.rnd_nd(0.0, sd);
		} while (delta == 0.0);

		switch (pc.conf.mut.opt)
		{
		case Opt::Trend:
			switch (act)
			{
			case Act::Inc:
				stat.update(stat.cur + std::fabs(delta));
				break;

			case Act::Dec:
				stat.update(stat.cur - std::fabs(delta));
				break;

			case Act::Undef:
				stat.update(stat.cur + delta);
				act = (delta > 0.0 ? Act::Inc : Act::Dec);
				break;
			}
			break;

		case Opt::Anneal:
			stat.update(stat.cur + delta);
			break;

		default:
			break;
		}

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
