#include "Conf.hpp"
#include "Net.hpp"
#include "Layer.hpp"
#include "Node.hpp"
#include "Mutation.hpp"

namespace Cortex
{
	///=============================================================================
	///	Static methods
	///=============================================================================

	LayerPtr Layer::create(Net* const _net)
	{
		return create(LayerDef{}, _net);
	}

	LayerPtr Layer::create(const LayerDef& _def, Net* const _net)
	{
		return mksp<Layer>(_def, _net);
	}

	LayerPtr Layer::create(const Layer& _other, Net* const _net)
	{
		return mksp<Layer>(_other, _net);
	}

	///=====================================
	/// Constructors
	///=====================================

	Layer::Layer(const LayerDef& _def, Net* const _net)
		:
		  type(_def.type),
		  fixed(_def.fixed),
		  net(_net)
	{
//		dlog("`-> Creating nodes...");

		/// Generate nodes.
		for (const auto& node_def : _def.nodes)
		{
			nodes.push_back(Node::create(node_def, this));
		}
	}

	Layer::Layer(const Layer& _other, Net* const _net)
		:
		  type(_other.type),
		  fixed(_other.fixed),
		  net(_net)
	{
//		dlog("`-> Creating nodes...");

		/// Generate nodes.
		for (const auto& node : _other.nodes)
		{
			nodes.push_back(Node::create(*node, this));
		}
	}

	LayerPtr Layer::self()
	{
		return shared_from_this();
	}

	///=====================================
	/// Connections
	///=====================================

	void Layer::init(const std::vector<NodePtr>& _ref_nodes)
	{
		for (const auto& node : _ref_nodes)
		{
			add_node(*node);
		}
	}

	void Layer::connect(const std::vector<NodePtr>& _ref_nodes)
	{
//		require(_ref_nodes.empty() || _ref_nodes.size() == lconf.nodes,
//				"Layer::connect(): The number of reference nodes is inconsistent with the layer configuration.");

		for (uint n = 0; n < nodes.size(); ++n)
		{
			_ref_nodes.empty() || _ref_nodes[n] == nullptr ? nodes[n]->connect()
														   : nodes[n]->connect(*_ref_nodes[n]);
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

	opt<uint> Layer::get_index() const
	{
		if (!net)
		{
			return nopt;
		}

		uint index(0);
		while (net->layers[index].get() != this)
		{
			++index;
		}

		return index;
	}

	LayerPtr Layer::get_prev() const
	{
		if (!net)
		{
			return nullptr;
		}
		auto idx(get_index());
		return idx ? net->layers[idx.value() - 1] : nullptr ;
	}

	LayerPtr Layer::get_next() const
	{
		if (!net)
		{
			return nullptr;
		}
		auto idx(get_index());
		return (idx && idx.value() == net->layers.size() - 1) ? net->layers[idx.value() + 1] : nullptr;
	}

	hset<NodePtr> Layer::get_free_targets(const NodePtr& _src, const LinkType _lt)
	{
		hset<NodePtr> targets;

		for (const auto& l : net->get_target_layers(self(), _lt))
		{
			Cortex::apply(l->nodes, [&](const auto& _tgt)
			{
				if (!is_key(_tgt->sources, _src.get()))
				{
					targets.emplace(_tgt);
				}
			});
		}

		return targets;
	}

	hmap<NodePtr, hset<sz> > Layer::get_free_sources(const NodePtr& _tgt, const LinkType _lt)
	{
		hmap<NodePtr, hset<sz> > sources;
		for (const auto& layer : net->get_source_layers(self(), _lt))
		{
			Cortex::apply(layer->nodes, [&](const auto& _src)
			{
				if (!is_key(_tgt->sources, _src.get()))
				{
					for (uint vtx = 0; vtx < _src->vertices.get_size(); ++vtx)
					{
						sources[_src].emplace(vtx);
					}
				}
				else
				{
					for (uint vtx = 0; vtx < _src->vertices.get_size(); ++vtx)
					{
						if (!is_key(_tgt->sources[_src.get()], vtx))
						{
							sources[_src].emplace(vtx);
						}
					}
				}
			});
		}
		return sources;
	}

	LayerDef Layer::get_def() const
	{
		std::vector<NodeDef> ndefs;
		ndefs.reserve(nodes.size());
		for (const auto& node : nodes)
		{
			ndefs.emplace_back(node->get_def());
		}
		return {type, ndefs, fixed};
	}

	///=====================================
	/// Counters
	///=====================================

	template<>
	uint Layer::count<ElemType>(const ElemType _et) const
	{
//		dlog(">>>[Layer] Counting Layers of type ", _et);

		switch (_et)
		{
		case ElemType::Node:
			return nodes.size();

		case ElemType::Link:
			return Cortex::count(nodes, [&](const uint _sum, const NodePtr& _node)
			{
				return _sum + _node->count(_et);
			});

		default:
			return 0;
		}
	}

	template<>
	uint Layer::count<LinkType>(const LinkType _lt) const
	{
//		dlog(">>>[Layer] Counting links of type ", _lt);

		return Cortex::count(nodes, [&](const uint _sum, const NodePtr& _node)
		{
			return _sum + _node->count(_lt);
		});
	}

	///=====================================
	/// Structural changes
	///=====================================

	void Layer::add_node(const NodeDef& _ref, Layer* const _layer)
	{
		nodes.emplace_back(Node::create(_ref, _layer ? _layer : this));
	}

	bool Layer::erase_node(const uint _node)
	{
		if (_node < nodes.size())
		{
			nodes.erase(nodes.begin() + _node);
			return true;
		}
		return false;
	}

	void Layer::mutate(const MutationPtr& _mut)
	{
		if (_mut)
		{
			/// Apply the mutation.
			if (_mut->action == Action::Inc)
			{
				/// Add a node
				nodes.emplace_back(Node::create(this));
				nodes.back()->connect();
			}
			else
			{
//				/// Roulette wheel for nodes.
//				wmap<uint> wheel;
//				for (uint n = 0; n < nodes.size(); ++n)
//				{
//					wheel[n] = nodes[n]->age;
//				}

//				/// Spin the wheel to determine which node to erase.
//				erase_node(wheel.spin());

				/// Erase the last node
				erase_node(nodes.size() - 1);
			}
		}
	}

	///=====================================
	/// Printing and (de)serialisation
	///=====================================

	os& operator << (os& _os, const Layer& _layer)
	{
		if (!_layer.nodes.empty())
		{
			_os << "----[ Layer " << _layer.get_index() << " ]----";
			for (const auto& node : _layer.nodes)
			{
				_os << *node;
			}
			_os << "\n";
		}
		return _os;
	}

	void Layer::load(const json& _j)
	{

	}

	void Layer::save(json& _j) const
	{

	}
}
