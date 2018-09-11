#include "Conf.hpp"
#include "Data.hpp"
#include "RField.hpp"
#include "Net.hpp"
#include "Node.hpp"

namespace Cortex
{
	RField::RField(Evaluator& _evaluator)
		:
		  evaluator(_evaluator)
	{}

	template<>
	void RField::init<RFType::Gaussian>()
	{
		Vec mu(conf->net.rf.gaussian.nodes, arma::fill::zeros);
		real denom(1.0 / (conf->net.rf.gaussian.width * (conf->net.rf.gaussian.nodes - 2.0)));

		/// Generate an array of Gaussians for each input variable
		for (uint var = 0; var < conf->net.init.layers.front().nodes; ++var)
		{
			grf.emplace_back(mu, denom);
		}
	}

	template<>
	void RField::init<RFType::Direct>()
	{

	}

	template<>
	void RField::init<RFType::Adaptive>()
	{

	}

	template<>
	void RField::convert<RFType::Gaussian>(const Sample& _sample) const
	{
//		real delay(0.0);

//		for (uint var = 0; var < conf->net.init.layers.front().nodes; ++var)
//		{
//			for (uint r = 0; r < nodes[var].size(); ++r)
//			{
//				delay = grf[r].convert(_sample[var].front());
//				if (delay < conf->net.rf.grf.cutoff)
//				{
//					nodes[var][r]->schedule(delay);
//				}
//			}
//		}
	}

//	template<>
//	void RField::convert<NetType::Spiking, RFType::Direct>(const std::vector<std::vector<real>>& _input) const
//	{
//		for (uint var = 0; var < _input.size(); ++var)
//		{
//			for (uint time = 0; time < _input[var].size(); ++time)
//			{
//				nodes[var].front()->schedule(time);
//			}
//		}
//	}

//	template<>
//	void RField::convert<NetType::Classical, RFType::Direct>(const std::vector<std::vector<real>>& _input) const
//	{
//		for (uint var = 0; var < _input.size(); ++var)
//		{
//			for (uint r = 0; r < _input[var].size(); ++r)
//			{
//				nodes[var][r]->soma.add<NetType::Classical>(_input[var][r]);
//			}
//		}
//	}

	///=====================================
	/// Dispatch functions
	///=====================================

//	template<>
//	void RField::init()
//	{
//		/// Add nodes
//		uint idx(0);
//		for (uint var = 0; var < net.get_node_count(NodeType::Input); ++var)
//		{
//			for (uint r = 0; r < conf->net.rf.size; ++r)
//			{
//				nodes[var].emplace_back(net.nodes[NodeType::Input].at(idx++));
//			}
//		}

//		switch (conf->net.rf.type)
//		{
//		case RFType::Gaussian:
//			init<RFType::Gaussian>();
//			break;

//		default:
//			break;
//		}
//	}

//	template<>
//	void RField::convert<NetType::Spiking>(const Sample& _sample) const
//	{
//		switch (rf)
//		{
//		case RFType::Gaussian:
//			convert<NetType::Spiking, RFType::Gaussian>(_sample);
//			break;

//		case RFType::Direct:
//			convert<NetType::Spiking, RFType::Direct>(_sample);
//			break;

//		case RFType::Adaptive:
//			convert<NetType::Spiking, RFType::Adaptive>(_sample);
//			break;

//		default:
//			break;
//		}
//	}

//	template<>
//	void RField::convert<NetType::Convolutional>(const Sample& _sample) const
//	{
//		switch (rf)
//		{
//		case RFType::Direct:
//			convert<NetType::Convolutional, RFType::Convolutional>(_sample);
//			break;

//		default:
//			break;
//		}
//	}

//	template<>
//	void RField::convert<NetType::Classical>(const Sample& _sample) const
//	{
//		switch (rf)
//		{
//		case RFType::Direct:
//			convert<NetType::Classical, RFType::Direct>(_sample);
//			break;

//		default:
//			convert<NetType::Classical, RFType::Direct>(_sample);
//			break;
//		}
//	}

	template<>
	void RField::convert<RFType::Undef>(const Sample& _sample) const
	{

	}

//	RField::RField(Net& _net)
//	{
//		switch (conf->net.rf.type)
//		{
//		case RFType::Direct:
//			init<RFType::Direct>();
//			break;

//		case RFType::Convolutional:
//			init<RFType::Convolutional>();
//			break;

//		case RFType::Adaptive:
//			init<RFType::Adaptive>();
//			break;

//		default:
//			break;
//		}
//	}
}
