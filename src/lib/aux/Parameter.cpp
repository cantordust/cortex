#include "Parameter.hpp"

namespace Cortex
{
	void Parameter::Mutation::scale_sd(const Action _act)
	{
		sd *= (_act == Action::Dec) ? (1.0 - conf->learning.mutation.scale)
									: (1.0 + conf->learning.mutation.scale);
	}

	void Parameter::Mutation::set_sd(const real _new_sd)
	{
		sd = _new_sd;
	}

	Parameter::Parameter(ParamConf& _conf, const real _value)
		:
		  value(_value),
		  //		  stat(_conf.stat.alpha),
		  mutation({_conf.mutation.sd, Action::Undef})
	{}

	bool Parameter::mutate(real& _value)
	{
		switch (mutation.action)
		{
		case Action::Inc:
			_value += std::fabs(rnd_nd(0.0, mutation.sd));
			break;

		case Action::Dec:
			_value -= std::fabs(rnd_nd(0.0, mutation.sd));
			break;

		case Action::Undef:
			{
				real delta(rnd_nd(0.0, mutation.sd));
				_value += delta;
				mutation.action = (delta > 0.0 ? Action::Inc : Action::Dec);
			}
			break;
		}

		return true;
	}

	void Parameter::optimise(const Effect _effect)
	{
		/// Flip the action if the fitness has decreased
		if (_effect == Effect::Dec)
		{
			mutation.action = (mutation.action == Action::Inc) ? Action::Dec
															   : (mutation.action == Action::Dec) ? Action::Inc
																								  : Action::Undef;

			mutation.scale_sd(Action::Dec);
		}
		else if (_effect == Effect::Undef)
		{
			mutation.action = Action::Undef;
		}
	}

	bool operator == (const Parameter& _lhs, const Parameter& _rhs)
	{
		return _lhs.value == _rhs.value;
	}

	bool operator != (const Parameter& _lhs, const Parameter& _rhs)
	{
		return !(_lhs == _rhs);
	}
} // namespace Cortex
