#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, EcoConf& _c)
	{
		load(_j, "search", _c.search);
		load(_j, "init.size", _c.init.size);
		load(_j, "max.size", _c.max.size);
		load(_j, "max.age", _c.max.age);
	}

	EcoConf::EcoConf(Conf& _conf)
		:
		  conf(_conf)
	{
		set_defaults();
	}

	void EcoConf::set_defaults()
	{
		search = Search::Fitness;
		init.size = 50;
		max.size = 200;
		max.age = 1000;
	}

	std::string EcoConf::validate()
	{
		std::stringstream problems;

		if (search == Search::Fitness &&
			conf.fit.tgt <= 0.0 )
		{
			problems << "\t - Invalid target fitness value (" << conf.fit.tgt << ").\n";
		}
		if (init.size == 0)
		{
			problems << "\t - Invalid initial ecosystem size (" << init.size << ").\n";
		}
		if (init.size < conf.spc.init.count)
		{
			problems << "\t - Initial number of species > initial number of networks.\n";
		}
		if (max.age == 0 )
		{
			problems << "\t - Invalid maximal ecosystem age (" << max.age << ").\n";
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const EcoConf& _conf)
	{
		return _strm << "\n--- Ecosystem ---"
					 << "\nsearch: " << _conf.search
					 << "\ninit.size: " << _conf.init.size
					 << "\nmax.size: " << _conf.max.size
					 << "\nmax.age: " << _conf.max.age
					 << "\n";
	}
}
