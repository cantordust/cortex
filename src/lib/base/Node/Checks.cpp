#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Link.hpp"
#include "Species.hpp"

namespace Cortex
{
	uint Node::src_count(const LT _lt) const
	{
		if (_lt == LT::Undef)
		{
			return links.sources.size();
		}
		uint sum(0);
		for (const auto& link : links.sources)
		{
			if (link.second->type == _lt)
			{
				++sum;
			}
		}
		return sum;
	}

	uint Node::tgt_count(const LT _lt) const
	{
		if (_lt == LT::Undef)
		{
			return links.targets.size();
		}
		uint sum(0);
		for (const auto& link : links.targets)
		{
			if (link.second->type == _lt)
			{
				++sum;
			}
		}
		return sum;
	}

	bool Node::has_sources(const NR _nr, const LT _lt) const
	{
		for (const auto& link : links.sources)
		{
			if ((_nr == NR::Undef || link.first->id.role == _nr) &&
				(_lt == LT::Undef || link.second->type == _lt))
			{
				return true;
			}
		}
		return false;
	}

	bool Node::has_targets(const NR _nr, const LT _lt) const
	{
		for (const auto& link : links.targets)
		{
			if ((_nr == NR::Undef || link.first->id.role == _nr) &&
				(_lt == LT::Undef || link.second->type == _lt))
			{
				return true;
			}
		}
		return false;
	}

	bool Node::is_src(const NodeID& _id, const LT _lt) const
	{
		return is_src(net.get_node(_id), _lt);
	}

	bool Node::is_src(const NodePtr& _nptr, const LT _lt) const
	{
		return links.sources.find(_nptr) != links.sources.end() &&
			   (_lt == LT::Undef || links.sources.at(_nptr)->type == _lt);
	}

	bool Node::is_tgt(const NodeID& _id, const LT _lt) const
	{
		return is_tgt(net.get_node(_id), _lt);
	}

	bool Node::is_tgt(const NodePtr& _nptr, const LT _lt) const
	{
		return links.targets.find(_nptr) != links.targets.end() &&
			   (_lt == LT::Undef || links.targets.at(_nptr)->type == _lt);
	}

	bool Node::link_exists(const NodeID& _id, const LT _lt) const
	{
		return link_exists(net.get_node(_id), _lt);
	}

	bool Node::link_exists(const NodePtr& _nptr, const LT _lt) const
	{
		return is_src(_nptr, _lt) || is_tgt(_nptr, _lt);
	}

	bool Node::is_link_allowed(const NodeID& _id, const LT _lt)
	{
		return is_link_allowed(net.get_node(_id), _lt);
	}

	bool Node::is_link_allowed(const NodePtr& _nptr, const LT _lt)
	{
		if (link_exists(_nptr))
		{
			return false;
		}

		switch (_lt)
		{
		case LT::Fwd:
			return !forms_cycle(_nptr);

		case LT::Rec:
			return conf.link.rec && forms_cycle(_nptr);

		default:
			return false;
		}
	}

	bool Node::is_link_eraseable(const NodeID& _id, const LT _lt)
	{
		return is_link_eraseable(net.get_node(_id), _lt);
	}

	bool Node::is_link_eraseable(const NodePtr& _nptr, const LT _lt)
	{
		if (!link_exists(_nptr))
		{
			return false;
		}

		switch (_lt)
		{
		case LT::Fwd:
			return tgt_count(_lt) > 1 && _nptr->src_count(_lt) > 1;

		case LT::Rec:
			return true;

		default:
			return false;
		}
	}

	bool Node::visit(const bool _update_graph)
	{
		switch (mark)
		{
		case NodeMark::Perm: return true;
		case NodeMark::Temp: return false; // Not a DAG
		case NodeMark::Undef:
			{
				mark = NodeMark::Temp;
				for (const auto& link : links.targets)
				{
					if (link.second->type == LT::Fwd &&
						!link.second->tgt.visit(_update_graph))
					{
						return false;
					}
				}
				mark = NodeMark::Perm;
				if (_update_graph)
				{
					net.graph.push_back(self());
				}
				return true;
			}
		}
	}

	bool Node::forms_cycle(const NodeID& _id)
	{
		return forms_cycle(net.get_node(_id));
	}

	bool Node::forms_cycle(const NodePtr& _nptr)
	{
		net.reset_marks();
		links.add(self(), _nptr, LT::Fwd);
		bool is_dag(visit(false));
		links.erase(self(), _nptr);
		return !is_dag;
	}
}
