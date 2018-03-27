#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, SysConf& _c)
	{
		load(_j, "runs", _c.runs);
		load(_j, "threads", _c.threads);
	}

	SysConf::SysConf(Conf& _conf)
		:
		  ConfBase(_conf, "System configuration")
	{
		set_defaults();
	}

	void SysConf::set_defaults()
	{
		runs = 1;
		threads = std::thread::hardware_concurrency();
	}

	void SysConf::validate()
	{
		check(runs > 0, "Invalid number of runs (", runs, ").");
	}

	std::ostream& operator << (std::ostream& _strm, const SysConf& _conf)
	{
		_strm << _conf.header()
			  << "\nruns: " << _conf.runs
			  << "\nthreads: " << _conf.threads;

		return _strm << "\n";
	}
}
