#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, ParamConf& _c)
	{
		load(_j, "dist", _c.dist);
		load(_j, "val", _c.val);
		load(_j, "mean", _c.mean);
		load(_j, "sd", _c.sd);
		load(_j, "stat", _c.stat);
		load(_j, "lbound", _c.lbound);
		load(_j, "ubound", _c.ubound);
	}

	ParamConf::ParamConf(Conf& _conf)
		:
		  conf(_conf)
	{
		set_defaults();
	}

	void ParamConf::set_defaults()
	{
		dist = Dist::Normal;
		val = 0.0;
		mean = 0.0;
		sd = 1.0;
		lbound = std::numeric_limits<real>::min();
		ubound = std::numeric_limits<real>::max();
	}

	std::string ParamConf::validate()
	{
		std::stringstream problems;

		problems << stat.validate();

		if (dist == Dist::Normal &&
			sd <= 0.0)
		{
			problems << "\t - Invalid SD for initial distribution (" << sd << ").\n";
		}

		if (lbound > ubound)
		{
			problems << "\t - Lower bound > upper bound (lbound: " << lbound << ", ubound: " << ubound << ").\n";
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const ParamConf& _c)
	{
		return _strm << "\n--- Parameter ---"
					 << "\ndist: " << _c.dist
					 << "\nval: " << _c.val
					 << "\nmean: " << _c.mean
					 << "\nsd: " << _c.sd
					 << "\nlbound: " << _c.lbound
					 << "\nubound: " << _c.ubound
					 << "\nstat: " << _c.stat
					 << "\n";
	}
}
