#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, NodeConf& _c)
	{
		load(_j, "roles", _c.roles);
		load(_j, "tau", _c.tau);
		load(_j, "soma", _c.soma);
		load(_j, "axon", _c.axon);
	}

	NodeConf::NodeConf(Conf& _conf)
		:
		  ConfBase(_conf, "Node configuration"),
		  tau(_conf),
		  soma(_conf),
		  axon(_conf)
	{
		set_defaults();
	}

	TF NodeConf::rnd_tf(const NR _nr)
	{
		return conf.rnd_elem(tf.at(_nr));
	}

	void NodeConf::set_defaults()
	{
		roles[NR::B] = 0;
		roles[NR::I] = 0;
		roles[NR::O] = 0;
		roles[NR::H] = 0;

		tf[NR::B].emplace(TF::Const);
		tf[NR::I].emplace(TF::Sum);
		tf[NR::O].emplace(TF::Tanh);
		tf[NR::H].emplace(TF::Tanh);

		tau.val = 0.0;
		tau.mean = 5.0;
		tau.sd = 0.1;
		tau.lbound = 3.0;
		tau.ubound = 10.0;
	}

	void NodeConf::validate()
	{
		check(roles.at(NR::I) > 0, "Invalid number of input nodes (", roles.at(NR::I), ")");
		check(roles.at(NR::O) > 0, "Invalid number of output nodes (", roles.at(NR::O), ")");
		tau.validate();
		soma.validate();
		axon.validate();
	}

	std::ostream& operator << (std::ostream& _strm, const NodeConf& _conf)
	{
		_strm << _conf.header()
			  << "\nroles:\n";

		for (const auto& r : _conf.roles)
		{
			_strm << "\n\t" << r.first << ": " << r.second;
		}

		_strm << "\ntf:\n";

		for (const auto& r : _conf.tf)
		{
			_strm << "\n\t" << r.first << ":";
			for (const auto& t : r.second)
			{
				_strm << " " << t;
			}
		}

		_strm << "\ntau:\n " << _conf.tau
			  << "\nsoma:\n " << _conf.soma
			  << "\naxon:\n " << _conf.axon;

		return _strm << "\n";
	}
}
