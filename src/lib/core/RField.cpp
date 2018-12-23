#include "Conf.hpp"
#include "Data.hpp"
#include "RField.hpp"
#include "Net.hpp"
#include "Node.hpp"

namespace Cortex
{
	///=====================================
	/// Constructors and destructors
	///=====================================

	RField::RField(Node& _node)
		:
		  node(_node)
	{
//		set_dimensions(_dimensions);
	}

//	opt<Index> RField::index(Offset _offset) const
//	{
//		if (_offset >= size)
//		{
//			return nopt;
//		}

//		Array index;
//		if(_offset < size)
//		{
//			for (sz d = dimensions.size(); d-- > 0;)
//			{
//				index[d] = _offset / offsets[d];
//				_offset %= offsets[d];
//			}
//		}
//		return index;
//	}

//	opt<Offset> RField::offset(const Index& _index) const
//	{
//		if (!is_in_range(_index))
//		{
//			return nopt;
//		}

//		sz dim(0);
//		return std::accumulate(_index.cbegin(),
//							   _index.cend(),
//							   0,
//							   [&](const sz _sum, const sz _idx)
//		{
//			return _sum + _idx * offsets[dim++];
//		});
//	}

//	bool RField::is_in_range(const Array& _index) const
//	{
//		for (sz i = 0; i < _index.size(); ++i)
//		{
//			if (_index[i] >= dimensions[i])
//			{
//				return false;
//			}
//		}
//		return true;
//	}

//	void RField::set_dimensions(const Array& _dimensions)
//	{
//		dimensions = _dimensions;

//		size = 1;
//		for (sz d = 0; d < dimensions.size(); ++d)
//		{
//			if (dimensions[d] == 0)
//			{
//				dimensions[d] = 1;
//			}
//			offsets[d] = size;
//			size *= dimensions[d];
//		}
//	}

//	template<>
//	void RField::init<RSensorType::Gaussian>()
//	{
//		Vec mu(conf.net.rf.gaussian.nodes, arma::fill::zeros);
//		real denom(1.0 / (conf.net.rf.gaussian.width * (conf.net.rf.gaussian.nodes - 2.0)));

//		/// Generate an array of Gaussians for each input variable
//		for (uint var = 0; var < conf.net.init.layers.front().nodes; ++var)
//		{
//			grf.emplace_back(mu, denom);
//		}
//	}

//	template<>
//	void RField::init<RSensorType::Direct>()
//	{

//	}

//	template<>
//	void RField::init<RSensorType::Adaptive>()
//	{

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
	//			for (uint r = 0; r < conf.net.rf.size; ++r)
	//			{
	//				nodes[var].emplace_back(net.nodes[NodeType::Input].at(idx++));
	//			}
	//		}

	//		switch (conf.net.rf.type)
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

//	void RField::convert(const Sample& _sample) const
//	{

//	}

	//	RField::RField(Net& _net)
	//	{
	//		switch (conf.net.rf.type)
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
