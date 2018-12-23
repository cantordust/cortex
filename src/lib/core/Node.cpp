#include "Net.hpp"
#include "Node.hpp"
#include "RField.hpp"
#include "Link.hpp"
#include "Layer.hpp"

namespace Cortex
{
	///=============================================================================
	///	Static methods
	///=============================================================================

	NodePtr Node::create(Layer* const _layer)
	{
		return create(NodeDef{}, _layer);
	}

	NodePtr Node::create(const NodeDef& _def, Layer* const _layer)
	{
		return mksp<Node>(_def, _layer);
	}

	NodePtr Node::create(const Node& _other, Layer* const _layer)
	{
		return mksp<Node>(_other.get_def(), _layer);
	}

	///=====================================
	/// Constructors and destructors
	///=====================================

	Node::Node(const NodeDef& _def, Layer* const _layer)
		:
		  vertices(_def.dim),
		  tau(_def.tau),
		  layer(_layer)
	{
		//		dlog("`-> Node created.");
	}

	//	Node::Node(const Node& _other, Layer* const _layer)
	//		:
	//		  vertices(_other.vertices.get_dimensions()),
	//		  tau(_other.tau),
	//		  layer(_layer)
	//	{

	//	}

	Node::~Node()
	{
		disconnect();
	}

	NodePtr Node::self()
	{
		return shared_from_this();
	}

	///=====================================
	/// Bookkeeping
	///=====================================

	uint Node::get_index() const
	{
		uint index(0);
		while (layer->nodes[index].get() != this)
		{
			++index;
		}
		return index;
	}

	NodeID Node::get_id() const
	{
		return {layer->get_index(), get_index()};
	}

	NodePtr Node::get_prev() const
	{
		if (!layer)
		{
			return nullptr;
		}
		uint idx(get_index());
		return (idx == 0) ? nullptr : layer->nodes[idx - 1];
	}

	NodePtr Node::get_next() const
	{
		if (!layer)
		{
			return nullptr;
		}
		uint idx(get_index());
		return (idx == layer->nodes.size() - 1) ? nullptr : layer->nodes[idx + 1];
	}

	sz Node::get_kernel_size(Node* const _src)
	{
		sz radius(0);
		VCube dim;
		for (const auto& offset : sources[_src])
		{
			if (vertices.is_valid(offset.first))
			{
				dim = vertices.get_index(offset.first);
				radius = std::max(std::max(dim.width(), dim.height()), radius);
			}
		}

		/// Ensure that the radius is odd
		if (radius % 2 == 0)
		{
			++radius;
		}

		return radius;
	}

	NodeDef Node::get_def() const
	{
		return {vertices, tau};
	}

	hset<sz> Node::get_free_vertices(Node* const _src, const sz _radius)
	{
		if (!is_key(sources, _src))
		{
			return {};
		}

		sz ksize(get_kernel_size(_src));
		sz max_ksize(std::max(vertices.width(), vertices.height()));

		hset<sz> free_vertices;

		//		for (sz k = ksize; k < max_ksize; k += 2)
		//		{
		//			opt<sz> offset;
		//			for (uint width = 0; width < ksize; ++width)
		//			{
		//				for (uint height = 0; height < ksize; ++height)
		//				{
		//					for (uint depth = 0; depth < _src->vertices.get_dimensions().depth(); ++depth)
		//					{
		//						offset = _src->vertices.get_offset(depth, height, width);
		//						if (offset &&
		//							!is_key(sources[_src], offset.value()))
		//						{
		//							free_vertices.emplace(offset.value());
		//						}
		//					}
		//				}
		//			}

		//			if (!free_vertices.empty())
		//			{
		//				uint elements(rnd_int<sz>(1, free_vertices.size()));

		//				while (free_vertices.size() > elements)
		//				{
		//					free_vertices.erase(rnd_elem(free_vertices));
		//				}

		//				return free_vertices;
		//			}
		//		}
		return free_vertices;
	}

	///=====================================
	/// Counters
	///=====================================

	template<>
	uint Node::count<ElemType>(const ElemType _et) const
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

	template<>
	uint Node::count<LinkType>(const LinkType _lt) const
	{
		//		dlog(">>>[Node] Counting links of type ", _lt);

		return Cortex::count(sources, [&](const uint _sum, const auto& _node)
		{
			return Cortex::count(_node.second, [&](const uint, const auto& _vtx)
			{
				return _sum + (_lt == LinkType::Undef || _vtx.second.type == _lt ? 1 : 0);
			});
		});
	}

	///=====================================
	/// Connections
	///=====================================

	void Node::connect(const FanSize _in, const FanSize _out)
	{
		/// Generate forward links to this node.
		auto src_candidates(layer->get_free_sources(self(), LinkType::Forward));
		if (!src_candidates.empty())
		{
			uint src_count(_in == FanSize::One ? 1 :
												 _in == FanSize::All ? src_candidates.size() :
																	   rnd_int<uint>(1, src_candidates.size()));

			while(src_candidates.size() > src_count)
			{
				src_candidates.erase(rnd_key(src_candidates));
			}

			for (auto& src : src_candidates)
			{
				add_link(*src.first, min_element(src.second), LinkType::Forward);
			}
		}

		/// Generate forward links from this node.
		auto tgt_candidates(layer->get_free_targets(self(), LinkType::Forward));

		if (!tgt_candidates.empty())
		{
			uint tgt_count(_out == FanSize::One ? 1
												: _out == FanSize::All ? tgt_candidates.size()
																	   : rnd_int<uint>(1, tgt_candidates.size()));

			while(tgt_candidates.size() > tgt_count)
			{
				tgt_candidates.erase(rnd_elem(tgt_candidates));
			}

			for (const auto& tgt : tgt_candidates)
			{
				//				if (auto vertex	= tgt->get_vertex(self()))
				//				{
				//					tgt->add_link(self(), vertex, LinkType::Forward);
				//				}
			}
		}
	}

	void Node::connect(Node& _ref_node)
	{
		/// Iterate over the source nodes of the reference node.
		for (auto& ref_src : _ref_node.sources)
		{
			auto src(layer->net->get_node(_ref_node.get_id()));
			/// Check if the source node exists in this net.
			if (src)
			{
				/// Iterate over the reference vertices and replicate
				/// the corresponding links in this node.
				for (auto& ref_vtx : ref_src.second)
				{
					if (src->vertices.is_valid(_ref_node.vertices.get_index(ref_vtx.first)))
					{
						add_link(*src, ref_vtx.first, ref_vtx.second);
					}
				}
			}
		}

		/// Iterate over the target nodes of the reference node.
		for (auto& ref_tgt : _ref_node.targets)
		{
			auto tgt(layer->net->get_node(ref_tgt->get_id()));
			if (tgt)
			{
				/// Iterate over the reference vertices and replicate
				/// the corresponding links in the target node.
				for (auto& ref_vtx : ref_tgt->sources[std::addressof(const_cast<Node&>(_ref_node))])
				{
					if (vertices.is_valid(_ref_node.vertices.get_index(ref_vtx.first)))
					{
						tgt->add_link(*this, ref_vtx.first, ref_vtx.second);
					}
				}
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

	bool Node::add_link(Node& _src, const sz _vertex, const LinkType _lt, const opt<real> _weight)
	{
		if (/// Check if the source exists and whether we have
			/// maxed out the number of connections for that source.
			(is_key(sources, &_src) &&
			 sources[&_src].size() >= _src.vertices.get_size()))
		{
			return false;
		}

		//		dlog("`-> Adding link ", _src->id(), "->", id());

		sources[&_src].emplace(_vertex, Link{_lt, _weight});
		_src.targets.emplace(this);
		return true;
	}

	bool Node::add_link(Node& _src, const sz _vertex, Link& _other)
	{
		if (/// Check if the link already exists
			(is_key(sources, &_src) &&
			 is_key(sources[&_src], _vertex)))
		{
			return false;
		}

		//		dlog("`-> Adding link ", _src->id(), "->", id());

		sources[&_src].emplace(_vertex, _other);
		_src.targets.emplace(this);
		return true;
	}

	bool Node::erase_link(Node& _src, const opt<sz> _offset)
	{
		/// Check if we are down to a single link.
		/// If yes, delete the node
		if(sources.size() == 1 &&
		   sources.begin()->second.size() == 1)
		{
			layer->erase_node(get_index());
			return true;
		}

		/// Check if the provided node is indeed
		/// a source of the current node.
		if(!is_key(sources, &_src) ||
		   sources.size() == 1)
		{
			return false;
		}

		/// We need at least one link.
		if(sources.size() == 1 &&
		   sources.begin()->second.size() == 1)
		{
			return false;
		}

		/// Select a source node.
		/// Nodes with a larger number of links are
		/// more likely to be selected.
		Roulette<Node*> nodes;
		for (const auto& src : sources)
		{
			nodes[src.first] = src.second.size();
		}

		Node* src(nodes.spin());

		Roulette<sz> links;
		for (const auto& link : sources.at(src))
		{
			links[link.first] = link.second.weight;
		}

		sources.at(src).erase(links.inverse_spin());

		return true;

		//		dlog("`-> Erasing link ", _src->id(), "->", id());

		sources.erase(&_src);
		_src.targets.erase(this);
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
	//		tf = rnd_elem(conf.net.node.tf.at(id.type));
	//		return true;
	//	}

	///=====================================
	/// Printing and (de)serialisation
	///=====================================

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

	void Node::load(const json& _j)
	{

	}

	void Node::save(json& _j) const
	{

	}
}

