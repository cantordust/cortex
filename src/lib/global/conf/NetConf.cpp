#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, NetConf::RFConf& _c)
	{
		load(_j, "type", _c.type);
		load(_j, "size", _c.size);
		load(_j, "grf.cutoff", _c.grf.cutoff);
		load(_j, "grf.beta", _c.grf.beta);
	}

	template<>
	void load(const json& _j, NetConf::SpikeConf& _c)
	{
		load(_j, "lif", _c.lif);
		load(_j, "enc", _c.enc);
		load(_j, "mod", _c.mod);
		load(_j, "max.delay", _c.max.delay);
	}

	template<>
	void load(const json& _j, NetConf& _c)
	{
		load(_j, "type", _c.type);
		load(_j, "layout", _c.layout);
		load(_j, "rf", _c.rf);
		load(_j, "spike", _c.spike);
		load(_j, "max.age", _c.max.age);
	}

	NetConf::NetConf(Conf& _conf)
		:
		  conf(_conf),
		  id(0)
	{
		set_defaults();
	}

	uint NetConf::next_id()
	{
		return ++id;
	}

	void NetConf::reset_id()
	{
		id = 0;
	}

	void NetConf::set_defaults()
	{
		type = NT::Classical;
		layout = Layout::Random;

		rf.type = RF::None;
		rf.size = 1;
		rf.grf.cutoff = 0.95;
		rf.grf.beta = 1.5;

		spike.lif = true;
		spike.enc = Enc::Direct;
		spike.mod = 0.9;
		spike.max.delay = 70.0;

		max.age = 0;
	}

	std::string NetConf::validate()
	{
		std::stringstream problems;

		/// Make sure that the network type is consistent
		/// with the activation function
		if (type == NT::Classical)
		{
			/// Disable membrane time constant mutation.
			conf.mut.prob.erase(Mut::Tau);
		}
		else if (type == NT::Spiking)
		{
			/// Disable transfer function mutation.
			conf.mut.prob.erase(Mut::TF);

			/// The only available activation function
			/// should be Fn::Spiking
			for (auto& tf_set : conf.node.tf)
			{
				tf_set.second.clear();
				tf_set.second.emplace(TF::Spiking);
			}

			/// Restrict the absolute magnitude
			/// of link weights to 1.
			conf.link.weight.lbound = -1.0;
			conf.link.weight.ubound = 1.0;
		}
		else
		{
			problems << "\t - Invalid network type (" << type << ").\n";
		}

		if (rf.type != RF::GRF)
		{
			/// Set the number of receptors to 1 per variable.
			rf.size = 1;
		}

		if (spike.max.delay <= 0.0)
		{
			problems << "\t	- Invalid spike delay (" << spike.max.delay << ").\n";
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const NetConf& _conf)
	{
		return _strm << "\n--- NetConf ---"
					 << "\ntype: " << _conf.type
					 << "\nrf.type: " << _conf.rf.type
					 << "\nrf.size: " << _conf.rf.size
					 << "\nrf.grf.cutoff: " << _conf.rf.grf.cutoff
					 << "\nrf.grf.beta: " << _conf.rf.grf.beta
					 << "\nspike.lif: " << _conf.spike.lif
					 << "\nspike.enc: " << _conf.spike.enc
					 << "\nspike.mod: " << _conf.spike.mod
					 << "\nspike.max.delay: " << _conf.spike.max.delay
					 << "\nmax.age: " << _conf.max.age
					 << "\n";
	}
}
