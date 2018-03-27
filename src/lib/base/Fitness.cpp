#include "Conf.hpp"
#include "Param.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	Fitness::Fitness(Conf& _conf)
		:
		  conf(_conf),
		  abs(_conf.fit.abs),
		  rel(_conf.fit.rel),
		  eff(Eff::Undef)
	{}

	void Fitness::reset()
	{
		abs.reset();
		rel.reset();
	}

	bool Fitness::is_solved() const
	{
		return abs.last >= conf.fit.tgt;
	}

	void Fitness::add_param(Param& _p)
	{
		params.push_back(_p);
	}

	template<>
	void Fitness::feedback<ParamOpt::Anneal>()
	{
		for (const auto& param : params)
		{
			param.get().optimise<ParamOpt::Anneal>(*this);
		}
	}

	template<>
	void Fitness::feedback<ParamOpt::ES>()
	{
		for (const auto& param : params)
		{
			param.get().optimise<ParamOpt::ES>(*this);
		}
	}

	void Fitness::feedback(const real _val)
	{
		if (_val > abs.last)
		{
			eff = Eff::Inc;
		}
		else if (_val < abs.last)
		{
			eff = Eff::Dec;
		}
		else
		{
			eff = Eff::Undef;
		}
		abs.update(_val);

		switch (conf.mut.opt)
		{
		case ParamOpt::Anneal:
			feedback<ParamOpt::Anneal>();
			break;

		case ParamOpt::ES:
			feedback<ParamOpt::ES>();
			break;

		default:
			break;
		}

		/// Increase the SD if we haven't made any progress
		if (std::tanh(abs.get_offset()) <= 0.0)
		{
			for (const auto& param : params)
			{
				param.get().inc_sd();
			}
		}

		params.clear();
	}
}
