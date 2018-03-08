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
		  conf(_conf)
	{
		set_defaults();
	}

	void NoveltyConf::set_defaults()
	{
		enabled = false;
		hist_size = 100;
	}

	std::string NoveltyConf::validate()
	{
		std::stringstream problems;

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const NoveltyConf& _conf)
	{
		return _strm << "\n--- Novelty ---"
					 << "\nenabled: " << _conf.enabled
					 << "\nhist_size: " << _conf.hist_size
					 << "\n";
	}
}

