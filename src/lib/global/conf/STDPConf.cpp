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
		  ConfBase(_conf, "STDP configuration")
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

	void STDPConf::validate()
	{

	}

	std::ostream& operator << (std::ostream& _strm, const STDPConf& _conf)
	{
		_strm << _conf.header()
			  << "\nenabled: " << _conf.enabled
			  << "\nlr: " << _conf.lr
			  << "\ndp_ratio: " << _conf.dp_ratio
			  << "\ntau: " << _conf.tau
			  << "\ncutoff: " << _conf.cutoff;

		return _strm << "\n";
	}
}
