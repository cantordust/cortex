#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, StatConf& _stat)
	{
		load(_j, "track", _stat.track);
		load(_j, "ma", _stat.ma);
		load(_j, "alpha", _stat.alpha);
	}

	StatConf::StatConf(Conf& _conf)
		:
		  ConfBase(_conf, "Statistics configuration")
	{
		set_defaults();
	}

	void StatConf::set_defaults()
	{
		track = true;
		ma = MA::EMA;
		alpha = 0.25;
	}

	void StatConf::validate()
	{
		if (ma == MA::EMA)
		{
			check(alpha < 1.0, "EMA coefficient must be < 1 (alpha: ", alpha, ")");
			check(alpha > 0.0, "EMA coefficient must be > 0 (alpha: ", alpha, ")");
		}
	}

	std::ostream& operator << (std::ostream& _strm, const StatConf& _conf)
	{
		_strm << _conf.header()
			  << "\n\ttrack: " << std::boolalpha << _conf.track
			  << "\n\tma: " << _conf.ma
			  << "\n\talpha: " << _conf.alpha;

		return _strm << "\n";
	}
}
