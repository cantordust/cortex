#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, STDPConf& _c)
	{
		load(_j, "enabled", _c.enabled);
		load(_j, "lr", _c.lr);
		load(_j, "dp_ratio", _c.dp_ratio);
		load(_j, "tau", _c.tau);
		load(_j, "cutoff", _c.cutoff);
	}

	STDPConf::STDPConf(Conf& _conf)
		:
		  conf(_conf)
	{
		set_defaults();
	}

	void STDPConf::set_defaults()
	{
		enabled = false;
		lr = 0.005;
		dp_ratio = 1.05;
		tau = 5.0;
		cutoff = 1.0e-3;
	}

	std::string STDPConf::validate()
	{
		std::stringstream problems;

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const STDPConf& _conf)
	{
		return _strm << "\n--- STDP ---"
					 << "\nenabled: " << _conf.enabled
					 << "\nlr: " << _conf.lr
					 << "\ndp_ratio: " << _conf.dp_ratio
					 << "\ntau: " << _conf.tau
					 << "\ncutoff: " << _conf.cutoff
					 << "\n";
	}
}
