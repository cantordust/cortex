#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, FitConf& _c)
	{
		load(_j, "tgt", _c.tgt);
		load(_j, "stat", _c.stat);
	}

	FitConf::FitConf(Conf& _conf)
		:
		  conf(_conf)
	{
		set_defaults();
	}

	void FitConf::set_defaults()
	{
		tgt = 1.0;
	}

	std::string FitConf::validate()
	{
		std::stringstream problems;

		problems << stat.validate();

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const FitConf& _conf)
	{
		return _strm << "\n--- Fitness ---"
					 << "\ntgt: " << _conf.tgt
					 << "\nstat:\n" << _conf.stat
					 << "\n";
	}
}

