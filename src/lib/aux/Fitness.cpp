#include "Fitness.hpp"

namespace Cortex
{

	Fitness::Fitness()
		:
		  abs(conf->fitness.stat.alpha),
		  rel(conf->fitness.stat.alpha)
	{}

	void Fitness::add_param(Parameter& _p)
	{
		params.push_back(_p);
	}

	void Fitness::optimise(const real _new_value)
	{
		if (_new_value > abs.value)
		{
			effect = Effect::Inc;
		}
		else if (_new_value < abs.value)
		{
			effect = Effect::Dec;
		}
		else
		{
			effect = Effect::Undef;
		}
		abs.update(_new_value);

		for (const auto& param : params)
		{
			param.get().optimise(effect);
		}

		/// Increase the SD if we haven't made any progress
		if (abs.get_offset() <= 0.5)
		{
			for (const auto& param : params)
			{
				param.get().mutation.scale_sd(Action::Inc);
			}
		}

		params.clear();
	}

	void Fitness::set_target(const real _target)
	{
		target = _target;
	}

	os& operator <<(os& _os, const Fitness& _fitness)
	{
		_os << "\n\tAbsolute: " << _fitness.abs.value
			<< "\n\tRelative: " << _fitness.rel.value
			<< "\n\tTarget: " << _fitness.target;
		return _os;
	}
} // namespace Cortex
