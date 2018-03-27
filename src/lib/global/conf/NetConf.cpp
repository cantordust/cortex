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
		  ConfBase(_conf, "Network configuration"),
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

		conn = 0.5;

		rf.type = RF::Direct;
		rf.size = 1;
		rf.grf.cutoff = 0.95;
		rf.grf.beta = 1.5;

		spike.lif = true;
		spike.enc = Enc::Direct;
		spike.mod = 0.9;
		spike.max.delay = 70.0;

		max.age = 0;
	}

	void NetConf::validate()
	{
		/// Make sure that the network type is consistent
		/// with the activation function
		check(type != NT::Undef, "Invalid network type (", type, ")");

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
			/// should be TF::Spiking
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

		check(conn > 0.0, "Connectivity should be > 0 (conn: ", conn, ")");
		check(conn <= 1.0, "Connectivity should be <= 1 (conn: ", conn, ")");

		if (layout == Layout::Layered)
		{
			check(layers.size() > 1, "Invalid number of layers (", layers.size(), ")");
		}

		if (rf.type != RF::GRF)
		{
			/// Set the number of receptors to 1 per variable.
			rf.size = 1;
		}

		check(spike.max.delay > 0.0, "Invalid spike delay (", spike.max.delay, ")");
	}

	std::ostream& operator << (std::ostream& _strm, const NetConf& _conf)
	{
		_strm << _conf.header()
			  << "\ntype: " << _conf.type
			  << "\nrf.type: " << _conf.rf.type
			  << "\nrf.size: " << _conf.rf.size
			  << "\nrf.grf.cutoff: " << _conf.rf.grf.cutoff
			  << "\nrf.grf.beta: " << _conf.rf.grf.beta
			  << "\nspike.lif: " << _conf.spike.lif
			  << "\nspike.enc: " << _conf.spike.enc
			  << "\nspike.mod: " << _conf.spike.mod
			  << "\nspike.max.delay: " << _conf.spike.max.delay
			  << "\nmax.age: " << _conf.max.age;

		return _strm << "\n";
	}
}
