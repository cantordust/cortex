#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Link.hpp"
#include "Sample.hpp"

namespace Cortex
{
	template<>
	void Net::eval<NT::Classical>()
	{
		/// Evaluate the nodes in order
		for (const auto& node : graph)
		{
			node->eval<NT::Classical>();
		}
	}

	template<>
	void Net::eval<Enc::Direct>()
	{
		while (!scheduler.empty())
		{
			scheduler.top().node->axon.time = scheduler.top().time;
			for (const auto& tgt : scheduler.top().node->links.targets)
			{
				tgt.second->tgt.eval<Enc::Direct>(tgt.second);
			}
			scheduler.pop();
		}
	}

	/// @todo Implementation
	template<>
	void Net::eval<Enc::RankOrder>()
	{
		eval<Enc::Direct>();
	}

	std::vector<real> Net::get_output()
	{
		std::vector<real> output;
		output.reserve(nodes.at(NR::O).size());
		for (const auto& out : nodes.at(NR::O))
		{
			output.push_back(out->axon.val.cur);
		}

//		dlog d;
//		d << "Net " << id << " output:";
//		for (const real o : output)
//		{
//			d << " " << o;
//		}

		return output;
	}

	///=========================================
	/// Dispatch functions
	///=========================================

	template<>
	void Net::eval<NT::Spiking>()
	{
		switch (conf.net.spike.enc)
		{
		case Enc::Direct:
			eval<Enc::Direct>();
			break;

		case Enc::RankOrder:
			eval<Enc::RankOrder>();
			break;

		default:
			dlog() << "Invalid encoding type " << conf.net.spike.enc;
			exit(EXIT_FAILURE);
		}
	}

	void Net::eval(const Sample& _sample)
	{
		/// Present the sample to the network
		rfield.convert(_sample);

		switch (conf.net.type)
		{
		case NT::Classical:
			eval<NT::Classical>();
			break;

		case NT::Spiking:
			eval<NT::Spiking>();
			break;

		default:
			dlog() << "Invalid network type " << conf.net.type;
			exit(EXIT_FAILURE);
		}
	}

	void Net::eval(const std::vector<real>& _input)
	{

	}
}
