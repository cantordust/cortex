#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, EnvConf& _c)
	{
		load(_j, "opt", _c.opt);
		load(_j, "type", _c.type);
		load(_j, "search", _c.search);
		load(_j, "init.size", _c.init.size);
		load(_j, "max.size", _c.max.size);
		load(_j, "max.age", _c.max.age);
	}

	EnvConf::EnvConf(Conf& _conf)
		:
		  ConfBase(_conf, "Environment configuration")
	{
		set_defaults();
	}

	void EnvConf::set_defaults()
	{
		opt = OptMode::Evolution;
		type = EnvType::Population;
		search = Search::Fitness;
		init.size = 50;
		max.size = 200;
		max.age = 1000;
	}

	void EnvConf::validate()
	{
		if (search == Search::Fitness)
		{
			check(conf.fit.tgt > 0.0, "Invalid target fitness value (", conf.fit.tgt, ").");
		}
		check(init.size > 0, "Invalid initial environment size (", init.size, ").");
		check(init.size >= conf.spc.init.count, "Initial number of species > initial number of networks.");
		check(max.age > 0, "Invalid maximal environment age (", max.age, ").");
	}

	std::ostream& operator << (std::ostream& _strm, const EnvConf& _conf)
	{
		_strm << _conf.header()
			  << "\nopt: " << _conf.opt
			  << "\ntype: " << _conf.type
			  << "\nsearch: " << _conf.search
			  << "\ninit.size: " << _conf.init.size
			  << "\nmax.size: " << _conf.max.size
			  << "\nmax.age: " << _conf.max.age;

		return _strm << "\n";
	}
}
