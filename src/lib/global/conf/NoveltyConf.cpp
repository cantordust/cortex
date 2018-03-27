#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, NoveltyConf& _c)
	{
		load(_j, "enabled", _c.enabled);
		load(_j, "hist_size", _c.hist_size);
	}

	NoveltyConf::NoveltyConf(Conf& _conf)
		:
		  ConfBase(_conf, "Novelty configuration")
	{
		set_defaults();
	}

	void NoveltyConf::set_defaults()
	{
		enabled = false;
		hist_size = 100;
	}

	void NoveltyConf::validate()
	{
	}

	std::ostream& operator << (std::ostream& _strm, const NoveltyConf& _conf)
	{
		return _strm << _conf.header()
					 << "\nenabled: " << _conf.enabled
					 << "\nhist_size: " << _conf.hist_size
					 << "\n";
	}
}

