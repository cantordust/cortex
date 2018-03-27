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
		  ConfBase(_conf, "Parameter configuration"),
		  stat(_conf)
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

	void ParamConf::validate()
	{
		if (dist == Dist::Normal)
		{
			check(sd > 0.0, "Invalid SD for initial distribution (", sd, ")");
		}
		check (lbound <= ubound, "Lower bound > upper bound (lbound: ", lbound, ", ubound: ", ubound, ")");
		stat.validate();
	}

	std::ostream& operator << (std::ostream& _strm, const ParamConf& _conf)
	{
		_strm << _conf.header()
			  << "\ndist: " << _conf.dist
			  << "\nval: " << _conf.val
			  << "\nmean: " << _conf.mean
			  << "\nsd: " << _conf.sd
			  << "\nlbound: " << _conf.lbound
			  << "\nubound: " << _conf.ubound
			  << "\nstat: " << _conf.stat;

		return _strm << "\n";
	}
}
