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
		  ConfBase(_conf, "Mating configuration")
	{
		set_defaults();
	}

	void MatingConf::set_defaults()
	{
		enabled = true;
		rate = 0.1;
	}

	void MatingConf::validate()
	{
		if (enabled)
		{
			check(rate > 0.0,  "Invalid mating rate (", rate, ")");
		}
	}

	std::ostream& operator << (std::ostream& _strm, const MatingConf& _conf)
	{
		_strm << _conf.header()
			  << "\nenabled: " << _conf.enabled
			  << "\nrate: " << _conf.rate;

		return _strm << "\n";
	}
}
