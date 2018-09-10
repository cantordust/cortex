#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Link.hpp"
#include "Layer.hpp"

namespace Cortex
{
	///=====================================
	/// Constructors and destructors
	///=====================================

	Node::Element(Layer& _layer, Node* const _other)
		:
		  layer(_layer),
		  tau(conf->node.tau, (_other ? _other->tau.value : conf->node.tau.initialise()))
	{
//		dlog("`-> Node created.");
	}

	///=====================================
	/// Bookkeeping
	///=====================================

	uint Node::index() const
	{
		uint index(0);
		while (layer.nodes[index].get() != this)
		{
			++index;
		}

		return index;
	}

	NodeID Node::id() const
	{
		return {layer.index(), index()};
	}

	Node* const Node::prev() const
	{
		uint idx(index());
		return (idx == 0) ? nullptr : layer.nodes[idx - 1].get();
	}

	Node* const Node::next() const
	{
		uint idx(index());
		return (idx == layer.nodes.size() - 1) ? nullptr : layer.nodes[idx + 1].get();
	}

	Node* const Node::front() const
	{
		return layer.nodes.front().get();
	}

	Node* const Node::back() const
	{
		return layer.nodes.back().get();
	}

	///=====================================
	/// Counters
	///=====================================

	uint Node::count(const ElemType _et) const
	{
//		dlog(">>>[Node] Counting elements of type ", _et);

		switch (_et)
		{
		case ElemType::Link:
			return sources.size();

		default:
			return 0;
		}
	}

	uint Node::count(const LinkType _lt) const
	{
//		dlog(">>>[Node] Counting links of type ", _lt);

		return Cortex::count(sources, [&](const uint _sum, const auto& _src)
		{
			return (_lt == LinkType::Undef || _src.second.type == _lt) ? _sum + 1
																	   : _sum;
		});
	}

	///=====================================
	/// Connections
	///=====================================

	void Node::connect(Node* const _ref_node)
	{
		/// Check if a reference node has been provided
		/// (this happens during crossover).
		if (_ref_node)
		{
			/// Replicate source links to the reference node.
			for (const auto& ref_src : _ref_node->sources)
			{
				Node* const src(layer.net.get_node(ref_src.first->id()));
				if (src)
				{
					add_link(src, ref_src.second);
				}
			}

			/// Replicate target links from the reference node.
			for (const auto& ref_tgt : _ref_node->targets)
			{
				Node* const tgt(layer.net.get_node(ref_tgt->id()));
				if (tgt)
				{
					tgt->add_link(this, ref_tgt->sources.at(_ref_node));
				}
			}
		}
		/// No reference node - connect at random
		else
		{
			/// Connect the node to a random node in the preceding layer.
			auto prev_layer(layer.prev());
			if (prev_layer)
			{
				add_link(rnd_elem(prev_layer->nodes).get(), LinkType::Forward);
			}

			/// Connect the node to a random node in the following layer.
			auto next_layer(layer.next());
			if (next_layer)
			{
				rnd_elem(next_layer->nodes)->add_link(this, LinkType::Forward);
			}
		}
	}

	void Node::disconnect()
	{
		/// Erase links from this node from all sources.
		for (const auto& src : sources)
		{
			src.first->targets.erase(this);
		}

		/// Erase links from this node to all targets.
		for (const auto& tgt : targets)
		{
			tgt->sources.erase(this);
		}

		sources.clear();
		targets.clear();
	}

	///=====================================
	/// Mutation
	///=====================================

	bool Node::add_link(Node* const _src, const LinkType _lt)
	{
		if (!_src || is_key(sources, _src))
		{
			return false;
		}

//		dlog("`-> Adding link ", _src->id(), "->", id());

		sources.emplace(_src, _lt);
		_src->targets.emplace(this);
		return true;
	}

	bool Node::add_link(Node* const _src, const Link& _other)
	{
		if (!_src || is_key(sources, _src))
		{
			return false;
		}

//		dlog("`-> Adding link ", _src->id(), "->", id());

		sources.emplace(_src, _other);
		_src->targets.emplace(this);
		return true;
	}

	bool Node::erase_link(Node* const _src)
	{
		if (!_src || !is_key(sources, _src) ||
			sources.size() == 1)
		{
			return false;
		}

//		dlog("`-> Erasing link ", _src->id(), "->", id());

		sources.erase(_src);
		_src->targets.erase(this);
		return true;
	}

	//	template<>
	//	bool Node::mutate<MutType::Weight>()
	//	{
	//		if (links.targets.size() > 0)
	//		{
	//			return rnd_val(links.targets)->weight.mutate(net.fitness);
	//		}
	//		else if (links.sources.size() > 0)
	//		{
	//			return rnd_val(links.sources)->weight.mutate(net.fitness);
	//		}
	//		else
	//		{
	//			die("Disconnected node ", id, ", exiting...");
	//		}
	//		return false;
	//	}

	//	template<>
	//	bool Node::mutate<MutType::TimeConst>()
	//	{
	//		return tau.mutate(net.fitness);
	//	}

	//	template<>
	//	bool Node::mutate<MutType::TransferFunction>()
	//	{
	//		tf = rnd_elem(conf->net.node.tf.at(id.type));
	//		return true;
	//	}

	//	template<>
	//	bool Node::mutate<MutType::AddLink>()
	//	{
	//		NodePtr other(get_rnd_tgt(id.type));
	//		if (other &&
	//			add_link(other))
	//		{
	//			return true;
	//		}

	//		other = get_rnd_src(id.type);
	//		if (other &&
	//			other->add_link(self()))
	//		{
	//			return true;
	//		}

	//		return false;
	//	}

	//	template<>
	//	bool Node::mutate<MutType::EraseLink>()
	//	{
	//		if ((!links.targets.empty() &&
	//			erase_link(rnd_key(links.targets))) ||
	//			(!links.sources.empty() &&
	//			 rnd_key(links.sources)->erase_link(self())))
	//		{
	//			return true;
	//		}
	//		return false;
	//	}

	///=====================================
	/// Serialisation and printing
	///=====================================

	/// @todo Return something useful
	json Node::to_json()
	{
		return json();
	}

	os& operator << (os& _os, const Node& _node)
	{
		if (!_node.targets.empty())
		{
			_os << "\t  ^\n";
			for (const auto& link : _node.sources)
			{
				_os << link.second;
			}
			_os << "\n\n";
		}
		return _os;
	}
}
