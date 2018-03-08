#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	Node::Node(const NR _nr, const uint _idx, Net& _net)
		:
		  id({_nr, _idx}),
		  conf(_net.conf),
		  axon(_net.conf.node.axon),
		  soma(_net.conf.node.soma),
		  net(_net),
		  tf(_net.conf.node.rnd_tf(_nr)),
		  mark(NodeMark::Undef),
		  tau(_net.conf.node.tau)
	{}

	Node::Node(const Node& _other, Net& _net)
		:
		  id(_other.id),
		  conf(_net.conf),
		  axon(_net.conf.node.axon),
		  soma(_net.conf.node.soma),
		  net(_net),
		  tf(_other.tf),
		  mark(NodeMark::Undef),
		  tau(_other.tau)
	{}

	bool Node::set_tf(const TF _tf)
	{
		tf = _tf;
		return true;
	}

	void Node::connect()
	{
		/// Make sure that the node is connected.

		//		dlog() << "Connecting node " << id;

		/// Target checks
//		dlog d;
		if (id.role != NR::O)
		{
//			for (const auto& l : ldefs)
//			{
//				d << l << "\n";
//			}
//			d << "---\n";
			while (!has_targets(NR::Undef, LT::Fwd))
			{
				const NodePtr& tgt(get_rnd_tgt(id.role, LT::Fwd));
				if (is_link_allowed(tgt, LT::Fwd))
				{
					add_link(tgt, LT::Fwd);
				}
			}
		}

		/// Source checks
		if ((id.role == NR::H ||
			 id.role == NR::O))
		{
//			for (const auto& l : ldef)
//			{
//				d << l << "\n";
//			}
			while (!has_sources(NR::Undef, LT::Fwd))
			{
				const NodePtr& src(get_rnd_src(id.role, LT::Fwd));
				if (src->is_link_allowed(self(), LT::Fwd))
				{
					src->add_link(self(), LT::Fwd);
				}
			}
		}
	}

	void Node::disconnect()
	{
		NodePtr this_node(self());
		while(!links.sources.empty())
		{
			links.sources.begin()->first->links.erase(links.sources.begin()->first, this_node);
		}
		while(!links.targets.empty())
		{
			links.erase(this_node, links.targets.begin()->first);
		}
	}

	const LinkPtr Node::get_src_link(const NodeID& _id) const
	{
		return get_src_link(net.get_node(_id));
	}

	const LinkPtr Node::get_src_link(const NodePtr& _nptr) const
	{
		if (!is_src(_nptr))
		{
			return nullptr;
		}
		return links.sources.at(_nptr);
	}

	const LinkPtr Node::get_tgt_link(const NodeID& _id) const
	{
		return get_tgt_link(net.get_node(_id));
	}

	const LinkPtr Node::get_tgt_link(const NodePtr& _nptr) const
	{
		if (!is_tgt(_nptr))
		{
			return nullptr;
		}
		return links.targets.at(_nptr);
	}

	NodePtr Node::get_rnd_src(const NR _tr, const LT _lt)
	{
		hset<LinkDef> ldefs(conf.link.by_tgt(_tr, _lt));
		hmap<NodePtr, real> srcs;
		Stat val(MA::SMA);

		/// Iterate over link defs
		for (const auto& ldef : ldefs)
		{
			/// Iterate over potential sources
			for (const auto& node : net.nodes[ldef.sr])
			{
				if (!is_src(node))
				{
					srcs[node] = node->axon.val.mean;
					val.update(node->axon.val.mean);
				}
			}
		}

		if (srcs.empty())
		{
			/// There are no suitable potential sources
			return nullptr;
		}

		/// Normalise.
		/// Higher values indicate more likely candidates.
		for (auto& src : srcs)
		{
			src.second = Logistic(val.get_offset(src.second));
		}

		/// Nodes with higher mean axon output
		/// are more likely to be picked
		return conf.w_dist(srcs);
	}

	NodePtr Node::get_rnd_tgt(const NR _sr, const LT _lt)
	{
		hset<LinkDef> ldefs(conf.link.by_src(_sr, _lt));
		hmap<NodePtr, real> tgts;
		Stat val(MA::SMA);

		/// Iterate over link defs
		for (const auto& ldef : ldefs)
		{
			/// Iterate over potential targets
			for (const auto& node : net.nodes[ldef.tr])
			{
				if (!is_tgt(node))
				{
					tgts[node] = node->soma.stat.mean;
					val.update(node->soma.stat.mean);
				}
			}
		}

		if (tgts.empty())
		{
			/// There is no suitable target
			return nullptr;
		}

		/// Normalise *and invert*.
		/// Lower values indicate more likely candidates.
		for (auto& tgt : tgts)
		{
			tgt.second = 1.0 - Logistic(val.get_offset(tgt.second));
		}

		/// Nodes with lower mean input
		/// are more likely to be picked
		return conf.w_dist(tgts);
	}

	std::ostream& operator << (std::ostream& _strm, const Node& _node)
	{
		for (const auto& link : _node.links.targets)
		{
			_strm << *link.second;
		}
		_strm << "\t  ^\n"
			  << "\t" << _node.id << " (" << _node.tf << ")\n\n";

		return _strm;
	}

}
