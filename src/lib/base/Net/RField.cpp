#include "Conf.hpp"
#include "Data.hpp"
#include "RField.hpp"
#include "Net.hpp"
#include "Node.hpp"

namespace Cortex
{
	RField::RField(Net& _net)
		:
		  net(_net),
		  nodes(_net.conf.node.roles.at(NR::I))
	{
		for (uint var = 0; var < nodes.size(); ++var)
		{
			nodes[var].reserve(net.conf.net.rf.size);
		}
	}

	template<>
	void RField::init<RF::GRF>()
	{
		real mu(0.0);
		real denom(0.0);
		for (uint rc = 0; rc < net.conf.net.rf.size; ++rc)
		{
			mu = 0.5 * (2.0 * rc - 3.0) / net.conf.net.rf.size;
			denom = 1.0 / (net.conf.net.rf.grf.beta * (net.conf.net.rf.size - 2.0));
			grf.push_back(GRF(mu, denom));
		}
	}

	template<>
	void RField::convert<NT::Spiking, RF::GRF>(const std::vector<std::vector<real>>& _input) const
	{
		real delay(0.0);

		for (uint var = 0; var < _input.size(); ++var)
		{
			for (uint r = 0; r < nodes[var].size(); ++r)
			{
				delay = grf[r].convert(_input[var].front());
				if (delay < net.conf.net.rf.grf.cutoff)
				{
					nodes[var][r]->schedule(delay);
				}
			}
		}
	}

	template<>
	void RField::convert<NT::Spiking, RF::Direct>(const std::vector<std::vector<real>>& _input) const
	{
		for (uint var = 0; var < _input.size(); ++var)
		{
			for (uint time = 0; time < _input[var].size(); ++time)
			{
				nodes[var].front()->schedule(time);
			}
		}
	}

	template<>
	void RField::convert<NT::Classical, RF::Direct>(const std::vector<std::vector<real>>& _input) const
	{
		for (uint var = 0; var < _input.size(); ++var)
		{
			for (uint r = 0; r < _input[var].size(); ++r)
			{
				nodes[var][r]->soma.add<NT::Classical>(_input[var][r]);
			}
		}
	}

	///=========================================
	/// Dispatch functions
	///=========================================

	void RField::init()
	{
		/// Add node references
		uint idx(0);
		for (uint var = 0; var < net.get_node_count(NR::I); ++var)
		{
			for (uint r = 0; r < net.conf.net.rf.size; ++r)
			{
				nodes[var].emplace_back(net.nodes[NR::I].at(idx++));
			}
		}

		switch (net.conf.net.rf.type)
		{
		case RF::GRF:
			init<RF::GRF>();
			break;

		default:
			break;
		}
	}

	template<>
	void RField::convert<NT::Spiking>(const std::vector<std::vector<real>>& _input) const
	{
		switch (rf)
		{
		case RF::GRF:
			convert<NT::Spiking, RF::GRF>(_input);
			break;

		case RF::Direct:
			convert<NT::Spiking, RF::Direct>(_input);
			break;

		default:
			break;
		}
	}

	template<>
	void RField::convert<NT::Classical>(const std::vector<std::vector<real>>& _input) const
	{
		switch (rf)
		{
		/// @todo Implement other receptive fields

		default:
			convert<NT::Classical, RF::Direct>(_input);
			break;
		}
	}

	void RField::convert(const Sample& _sample) const
	{
		switch (net.conf.net.type)
		{
		case NT::Classical:
			convert<NT::Classical>(_sample.input);
			break;

		case NT::Spiking:
			convert<NT::Spiking>(_sample.input);
			break;

		default:
			break;
		}
	}

	void RField::convert(const std::vector<std::vector<real> >& _input) const
	{

	}

	void RField::convert(const std::vector<real>& _input) const
	{

	}

	void RField::convert(std::vector<std::vector<real> >&& _input)
	{

	}

	void RField::convert(std::vector<real>&& _input)
	{

	}

	void RField::clear()
	{
		nodes.clear();
	}
}
