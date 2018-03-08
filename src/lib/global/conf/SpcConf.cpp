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
		  conf(_conf),
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

	std::string SpcConf::validate()
	{
		std::stringstream problems;

		if (init.count == 0)
		{
			problems << "\t - Invalid initial species count (" << init.count << ").\n";
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const SpcConf& _conf)
	{
		return _strm << "\n--- Species ---"
					 << "\ninit.count: " << _conf.init.count
					 << "\nmax.count: " << _conf.max.count
					 << "\n";
	}
}
