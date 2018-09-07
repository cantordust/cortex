#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Layer.hpp"

namespace Cortex
{
	///=====================================
	/// Constructors
	///=====================================

	Layer::Element(Net& _net, const LayerConf _lconf)
		:
		  net(_net),
		  lconf(_lconf)
	{
//		dlog("`-> Setting up nodes...");

		/// Generate nodes.
		for (uint n = 0; n < lconf.nodes; ++n)
		{
			nodes.push_back(mkup<Node>(*this));
		}
	}

	///=====================================
	/// Connections
	///=====================================

	void Layer::init(const std::vector<Node*>& _ref_nodes)
	{
		require(_ref_nodes.empty() || _ref_nodes.size() == lconf.nodes,
				"Layer::init(): The number of reference nodes is inconsistent with the layer configuration.");

		for (uint n = 0; n < lconf.nodes; ++n)
		{
			nodes.emplace_back(_ref_nodes.empty() ? mkup<Node>(*this) : mkup<Node>(*this, _ref_nodes[n]));
		}
	}

	void Layer::connect(const std::vector<Node*>& _ref_nodes)
	{
		require(_ref_nodes.empty() || _ref_nodes.size() == lconf.nodes,
				"Layer::connect(): The number of reference nodes is inconsistent with the layer configuration.");

		for (uint n = 0; n < nodes.size(); ++n)
		{
			_ref_nodes.empty() ? nodes[n]->connect() : nodes[n]->connect(_ref_nodes[n]);
		}
	}

	void Layer::disconnect()
	{
		for (const auto& node : nodes)
		{
			node->disconnect();
		}
	}

	///=====================================
	/// Bookkeeping
	///=====================================

	uint Layer::index() const
	{
		uint index(0);
		while (net.layers[index].get() != this)
		{
			++index;
		}

		return index;
	}

	Layer* const Layer::prev() const
	{
		uint idx(index());
		return (idx == 0) ? nullptr : net.layers[idx - 1].get();
	}

	Layer* const Layer::next() const
	{
		uint idx(index());
		return (idx == net.layers.size() - 1) ? nullptr : net.layers[idx + 1].get();
	}

	Layer* const Layer::front() const
	{
		return net.layers.front().get();
	}

	Layer* const Layer::back() const
	{
		return net.layers.back().get();
	}

	///=====================================
	/// Counters
	///=====================================

	uint Layer::count(const ElemType _et) const
	{
//		dlog(">>>[Layer] Counting elements of type ", _et);

		switch (_et)
		{
		case ElemType::Node:
			return nodes.size();

		case ElemType::Link:
			return std::accumulate(nodes.cbegin(),
								   nodes.cend(),
								   0,
								   [&](const uint _sum, const NodePtr& _node)
			{
				return _sum + _node->count(_et);
			});

		default:
			return 0;
		}
	}

	uint Layer::count(const LinkType _lt) const
	{
//		dlog(">>>[Layer] Counting links of type ", _lt);

		return std::accumulate(nodes.cbegin(),
							   nodes.cend(),
							   0,
							   [&](const uint _sum, const NodePtr& _node)
		{
			return _sum + _node->count(_lt);
		});
	}

	///=====================================
	/// Evolution
	///=====================================

	void Layer::mutate(const LayerConf& _lc)
	{
		/// Update the layer configuration.
		lconf = _lc;

		require(_lc.nodes != nodes.size(), "Error: mutating layer failed (new configuration is the same as the old one).");

		/// Apply the mutation.
		if (_lc.nodes > nodes.size())
		{
			/// Add a node
			nodes.emplace_back(mkup<Node>(*this));
			nodes.back()->connect();
		}
		else
		{
			/// Roulette wheel for nodes.
			wmap<uint> wheel;
			for (uint n = 0; n < nodes.size(); ++n)
			{
				wheel[n] = nodes[n]->age;
			}

			/// Spin the wheel to determine which node to erase.
			nodes.erase(nodes.begin() + wheel.spin());
		}
	}

	///=====================================
	/// Serialisation and printing
	///=====================================

	/// @todo Return something useful
	json Layer::to_json() const
	{
		return json();
	}

	os& operator << (os& _os, const Layer& _layer)
	{
		if (!_layer.nodes.empty())
		{
			_os << "----[ Layer " << _layer.index() << " ]----";
			for (const auto& node : _layer.nodes)
			{
				_os << *node;
			}
			_os << "\n";
		}
		return _os;
	}

}
