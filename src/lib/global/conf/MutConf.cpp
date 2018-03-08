#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, MutConf::EliteConf& _c)
	{
		load(_j, "enabled", _c.enabled);
		load(_j, "prop", _c.prop);
	}

	template<>
	void load(const json& _j, MutConf& _c)
	{
		load(_j, "enabled", _c.enabled);
		load(_j, "adaptive", _c.adaptive);
		load(_j, "rate", _c.rate);
		load(_j, "opt", _c.opt);
		load(_j, "scale", _c.scale);
		load(_j, "prob", _c.prob);
		load(_j, "elite", _c.elite);
	}

	MutConf::MutConf(Conf& _conf)
		:
		  conf(_conf)
	{
		set_defaults();
	}

	bool MutConf::allowed(const Mut _mut)
	{
		return prob.find(_mut) != prob.end();
	}

	void MutConf::set_defaults()
	{
		enabled = true;
		adaptive = false;
		rate = 5;
		opt = Opt::Trend;
		scale = 0.05;

		prob[Mut::AddNode] = 5.0;
		prob[Mut::EraseNode] = 5.0;
		prob[Mut::AddLink] = 30.0;
		prob[Mut::EraseLink] = 30.0;
		prob[Mut::Tau] = 10.0;
		prob[Mut::TF] = 0.0;
		prob[Mut::Weight] = 1000.0;

		elite.enabled = true;
		elite.prop = 0.05;
	}

	std::string MutConf::validate()
	{
		std::stringstream problems;

		if (enabled)
		{
			if (rate == 0)
			{
				problems << "\t - Invalid mutation rate (" << rate << ").\n";
			}
			if (elite.enabled &&
				elite.prop <= 0.0)
			{
				problems << "\t - Invalid elite proportion (" << elite.prop << ").\n";
			}

		}
		else
		{
			rate = 1;
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const MutConf& _conf)
	{
		_strm << "\n--- Mutation ---"
			  << "\nenabled: " << _conf.enabled
			  << "\nadaptive: " << _conf.adaptive
			  << "\nrate: " << _conf.rate
			  << "\nopt: " << _conf.opt
			  << "\nscale: " << _conf.scale;

		for (const auto& p : _conf.prob)
		{
			_strm << "\nprob[" << p.first << "]: " << p.second;
		}
		_strm << "\nelite.enabled: " << _conf.elite.enabled
			  << "\nelite.prop: " << _conf.elite.prop
			  << "\n";

		return _strm;
	}
}
