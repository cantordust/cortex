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
		  conf(_conf)
	{
		set_defaults();
	}

	void SysConf::set_defaults()
	{
		runs = 1;
		threads = std::thread::hardware_concurrency();
	}

	std::string SysConf::validate()
	{
		std::stringstream problems;

		if (runs == 0)
		{
			problems << "\t - Invalid number of runs (" << runs << ").\n";
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const SysConf& _conf)
	{
		return _strm << "\n--- System ---"
					 << "\nruns: " << _conf.runs
					 << "\nthreads: " << _conf.threads
					 << "\n";
	}
}
