#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, FitConf& _c)
	{
		load(_j, "tgt", _c.tgt);
		load(_j, "abs", _c.abs);
		load(_j, "rel", _c.rel);
	}

	FitConf::FitConf(Conf& _conf)
		:
		  ConfBase(_conf, "Fitness configuration"),
		  abs(_conf),
		  rel(_conf)
	{
		set_defaults();
	}

	void FitConf::set_defaults()
	{
		tgt = 0.0;
	}

	void FitConf::validate()
	{
		abs.validate();
		rel.validate();
	}

	std::ostream& operator << (std::ostream& _strm, const FitConf& _conf)
	{
		_strm << _conf.header()
			  << "\ntgt: " << _conf.tgt
			  << "\nabs:\n" << _conf.abs
			  << "\nrel:\n" << _conf.rel;

		return _strm << "\n";
	}
}

