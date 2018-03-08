#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, MatingConf& _c)
	{
		load(_j, "enabled", _c.enabled);
		load(_j, "rate", _c.rate);
	}

	MatingConf::MatingConf(Conf& _conf)
		:
		  conf(_conf)
	{
		set_defaults();
	}

	void MatingConf::set_defaults()
	{
		enabled = true;
		rate = 0.1;
	}

	std::string MatingConf::validate()
	{
		std::stringstream problems;

		if (enabled)
		{
			if (rate == 0.0)
			{
				problems << "\t - Invalid mating rate (" << rate << ").\n";
			}
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const MatingConf& _conf)
	{
		return _strm << "\n--- Mating ---"
					 << "\nenabled: " << _conf.enabled
					 << "\nrate: " << _conf.rate
					 << "\n";
	}
}
