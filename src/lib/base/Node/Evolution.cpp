#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"

namespace Cortex
{

	bool Node::add_link(const NodeID& _id, const LT _lt)
	{
		return add_link(net.get_node(_id), _lt);
	}

	bool Node::add_link(const NodePtr& _nptr, const LT _lt)
	{
		if (!_nptr)
		{
			return false;
		}

		if (_lt == LT::Undef)
		{
			return add_link(_nptr, LT::Fwd) || add_link(_nptr, LT::Rec);
		}
		else if(!is_link_allowed(_nptr, _lt))
		{
			return false;
		}

		/// Add the link
		links.add(self(), _nptr, _lt);

		return true;
	}

	bool Node::add_link(const NodeID& _id, const LinkPtr& _other)
	{
		return add_link(net.get_node(_id), _other);
	}

	bool Node::add_link(const NodePtr& _nptr, const LinkPtr& _other)
	{
		if (!_nptr ||
			!_other ||
			!is_link_allowed(_nptr, _other->type))
		{
			return false;
		}

		/// Add the link
		links.add(self(), _nptr, _other);

		return true;
	}

	bool Node::erase_link(const NodeID& _id)
	{
		return erase_link(net.get_node(_id));
	}

	bool Node::erase_link(const NodePtr& _nptr)
	{
		if (!_nptr ||
			!is_link_eraseable(_nptr))
		{
			return false;
		}

		/// Erase the link
		links.erase(self(), _nptr);

		return true;
	}

	void Node::crossover(const NodePtr& _p1, const NodePtr& _p2, const hmap<uint, real>& _fdist)
	{
		/// Iterate over links in parent 1
		for (const auto& tgt : _p1->links.targets)
		{
			const NodeID tgt_id(tgt.first->id);

			/// Check if the link is allowed
			if (is_link_allowed(tgt_id, tgt.second->type))
			{
				/// Check if the link exists in parent 2
				if (_p2->is_tgt(tgt_id, tgt.second->type))
				{
					/// The link exists in both parents.
					/// Roll a die to check which parent
					/// we should take the link from.
					if (conf.w_dist(_fdist) == _p1->net.id)
					{
						/// Copy the link from parent 1
						add_link(tgt_id, tgt.second);
					}
					else
					{
						/// Copy the link from parent 2
						add_link(tgt_id, _p2->get_tgt_link(tgt_id));
					}
				}
				else
				{
					/// Copy the link from parent 1
					add_link(tgt_id, tgt.second);
				}
			}
		}

		/// Iterate over links in parent 2
		for (const auto& tgt : _p2->links.targets)
		{
			const NodeID tgt_id(tgt.first->id);

			/// Check if the link exists in parent 1
			/// and whether it is allowed
			if (!_p1->is_tgt(tgt_id, tgt.second->type) &&
				is_link_allowed(tgt_id, tgt.second->type))
			{
				/// Copy link from parent 2
				add_link(tgt_id, tgt.second);
			}
		}
	}

	template<>
	bool Node::mutate<Mut::Weight>()
	{
		if (links.targets.size() > 0)
		{
			return conf.rnd_val(links.targets)->weight.mutate(net.fitness);
		}
		else if (links.sources.size() > 0)
		{
			return conf.rnd_val(links.sources)->weight.mutate(net.fitness);
		}
		else
		{
			dlog() << "Disconnected node " << id << "!";
			exit(EXIT_FAILURE);
		}
		return false;
	}

	template<>
	bool Node::mutate<Mut::Tau>()
	{
		return tau.mutate(net.fitness);
	}

	template<>
	bool Node::mutate<Mut::TF>()
	{
		tf = conf.rnd_elem(conf.node.tf.at(id.role));
		return true;
	}

	template<>
	bool Node::mutate<Mut::AddLink>()
	{
		NodePtr other(get_rnd_tgt(id.role));
		if (other &&
			add_link(other))
		{
			return true;
		}

		other = get_rnd_src(id.role);
		if (other &&
			other->add_link(self()))
		{
			return true;
		}

		return false;
	}

	template<>
	bool Node::mutate<Mut::EraseLink>()
	{
		if ((!links.targets.empty() &&
			erase_link(conf.rnd_key(links.targets))) ||
			(!links.sources.empty() &&
			 conf.rnd_key(links.sources)->erase_link(self())))
		{
			return true;
		}
		return false;
	}

	///=========================================
	/// Dispatch functions
	///=========================================

	bool Node::mutate(const Mut _mut)
	{
		switch (_mut)
		{
		case Mut::Weight:
			return mutate<Mut::Weight>();

		case Mut::Tau:
			return mutate<Mut::Tau>();

		case Mut::TF:
			return mutate<Mut::TF>();

		case Mut::EraseLink:
			return mutate<Mut::EraseLink>();

		case Mut::AddLink:
			return mutate<Mut::AddLink>();

		default:
			return false;
		}
	}

	const NodePtr Node::self()
	{
		return net.get_node(id);
	}
}


