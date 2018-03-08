#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, NodeConf& _c)
	{
		load(_j, "roles", _c.roles);
		load(_j, "tau", _c.tau);
		load(_j, "axon", _c.axon);
		load(_j, "soma", _c.soma);
	}

	NodeConf::NodeConf(Conf& _conf)
		:
		  conf(_conf),
		  tau(_conf)
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

	std::string NodeConf::validate()
	{
		std::stringstream problems;

		problems << tau.validate()
				 << axon.validate()
				 << soma.validate();

		if (roles.at(NR::I) == 0)
		{
			problems << "\t - Invalid number of input nodes (" << roles.at(NR::I) << ").\n";
		}

		if (roles.at(NR::O) == 0)
		{
			problems << "\t - Invalid number of output nodes (" << roles.at(NR::O) << ").\n";
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const NodeConf& _conf)
	{
		_strm << "\n--- Nodes ---"
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
			  << "\naxon:\n " << _conf.axon
			  << "\nsoma:\n " << _conf.soma
			  << "\n";

		return _strm;
	}
}
