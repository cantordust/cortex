#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, SpcConf& _c)
	{
		load(_j, "init.count", _c.init.count);
		load(_j, "max.count", _c.max.count);
	}

	SpcConf::SpcConf(Conf& _conf)
		:
		  ConfBase(_conf, "Species configuration"),
		  id(0)
	{
		set_defaults();
	}

	uint SpcConf::next_id()
	{
		return ++id;
	}

	void SpcConf::reset_id()
	{
		id = 0;
	}

	void SpcConf::set_defaults()
	{
		init.count = 3;
		max.count = 15;
	}

	void SpcConf::validate()
	{
		check(init.count > 0, "Invalid initial species count (", init.count, ").");
	}

	std::ostream& operator << (std::ostream& _strm, const SpcConf& _conf)
	{
		_strm << _conf.header()
			  << "\ninit.count: " << _conf.init.count
			  << "\nmax.count: " << _conf.max.count;

		return _strm << "\n";
	}
}
