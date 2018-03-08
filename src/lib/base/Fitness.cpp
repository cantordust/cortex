#include "Conf.hpp"
#include "Param.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	Fitness::Fitness(Conf& _conf)
		:
		  conf(_conf),
		  abs(_conf.fit.stat),
		  eff(Eff::Undef)
	{}

	void Fitness::reset()
	{
		abs.reset();
		rel.reset();
	}

	bool Fitness::is_solved() const
	{
		return abs.cur >= conf.fit.tgt;
	}

	void Fitness::add_param(Param& _p)
	{
		params.push_back(_p);
	}

	template<>
	void Fitness::feedback<Opt::Anneal>()
	{
		for (const auto& param : params)
		{
			param.get().optimise<Opt::Anneal>(*this);
		}
	}

	template<>
	void Fitness::feedback<Opt::Trend>()
	{
		for (const auto& param : params)
		{
			param.get().optimise<Opt::Trend>(*this);
		}
	}

	void Fitness::feedback(const real _val)
	{
		if (_val > abs.cur)
		{
			eff = Eff::Inc;
		}
		else if (_val < abs.cur)
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
		case Opt::Anneal:
			feedback<Opt::Anneal>();
			break;

		case Opt::Trend:
			feedback<Opt::Trend>();
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
