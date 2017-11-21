#include "Net.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	Node::Node(const NodeID& _id, Net& _net)
		:
		  net(_net),
		  id(_id),
		  cfg(_net.cfg),
		  mark(Mark::None),
		  af(_id.role, _net.cfg),
		  output(0.0),
		  tau(_net.cfg, _net.cfg.node.tau)
	{}

	Node::Node(Node& _other, Net& _net)
		:
		  net(_net),
		  id(_other.id),
		  cfg(_net.cfg),
		  mark(Mark::None),
		  af(_other.id.role, _net.cfg),
		  output(0.0),
		  tau(_other.tau),
		  grf(_other.grf)
	{}

	Node::~Node()
	{
		disconnect();
	}

	bool Node::mutate(const Mut _mut)
	{
		switch (_mut)
		{
		case Mut::Weight:
			{
				/// There must be at least one link
				/// whose weight can be mutated.
				while (true)
				{
					LT lt(cfg.w_dist(cfg.link.type));
					if (!cfg.link.rec ||
						(!has_targets(LT::R) &&
						 !has_sources(LT::R)))
					{
						lt = LT::F;
					}

					NR other_role(cfg.w_dist(cfg.mutation.node));

					if (has_targets(lt, other_role))
					{
						uint idx(cfg.rnd_key(links.targets.at(lt).at(other_role)));
						return links.targets.at(lt).at(other_role).at(idx)->weight.mutate(net.fitness);
					}
					else if (has_sources(lt, other_role))
					{
						uint idx(cfg.rnd_key(links.sources.at(lt).at(other_role)));
						return links.sources.at(lt).at(other_role).at(idx).get().weight.mutate(net.fitness);
					}
				}
			}

		case Mut::Tau:
			return tau.mutate(net.fitness);

		case Mut::Fn:
			return af.set_fn(cfg.w_dist(cfg.mutation.fn.at(id.role)));

		default:
			return false;
		}
	}

	bool Node::visit(const bool _update_graph)
	{
//		dlog() << "Visiting node " << id;
		switch (mark)
		{
		case Mark::Perm: return true;
		case Mark::Temp: return false; /// Not a DAG
		case Mark::None:
			{
				mark = Mark::Temp;
				for (auto& nrole : links.targets.at(LT::F))
				{
					for (auto& lnk : nrole.second)
					{
						if (!lnk.second->tgt.visit(_update_graph))
						{
							return false;
						}
					}
				}
				mark = Mark::Perm;
				if (_update_graph)
				{
					net.graph.emplace_back(*this);
				}
				return true;
			}
		}
	}

	void Node::crossover(Node& _p1, Node& _p2, const hmap<uint, real>& _fdist)
	{
//		dlog d;
//		d << "Crossing over node " << id << "\n";
		/// Iterate over link types
		for (auto& lt : links.targets)
		{
			///	Iterate over node roles
			for (auto& nr : lt.second)
			{
//				d << "\tCrossing over "
//				  << lt.first << " targets in role " << nr.first
//				  << ": (" << _p1.links.targets.at(lt.first).at(nr.first).size()
//				  << " links)\n";
				/// Iterate over indices in parent 1
				for (auto& idx : _p1.links.targets.at(lt.first).at(nr.first))
				{
					/// Temporary node ID
					NodeID n_id{nr.first, idx.first};

//					d << "\t\tCrossing over target " << n_id << "\n";

					if (link_allowed(lt.first, net.get_node(n_id)))
					{
						bool from_p1(cfg.w_dist(_fdist) == _p1.net.id);

						if (_p2.is_tgt(lt.first, n_id) && !from_p1)
//						if (_p2.is_tgt(lt.first, n_id) && cfg.rnd_chance(0.5))
						{
							/// The same link exists in parent 2 as well.
							/// Copy the link from parent 2
//							d << "\t\t\tCopying link from network " << _p2.net.id << "\n";
							add_link(lt.first, net.get_node(n_id), _p2.get_tgt_link(lt.first, n_id));
						}
						else if (from_p1)
//						else
						{
							/// The link exists in parent 1 only,
							/// so copy it from parent 1
//							d << "\t\t\tCopying link from network " << _p1.net.id << "\n";
							add_link(lt.first, net.get_node(n_id), *idx.second);
						}
//						else
//						{
//							d << "\t\t\tSkipping target " << n_id << "\n";
//						}
					}
				}

				/// Iterate over indices in parent 2
				for (auto& idx : _p2.links.targets.at(lt.first).at(nr.first))
				{
					/// Temporary node ID
					NodeID n_id{nr.first, idx.first};

					if (!_p1.is_tgt(lt.first, n_id) &&
						cfg.w_dist(_fdist) == _p2.net.id &&
						link_allowed(lt.first, net.get_node(n_id)))
					{
						/// The link exists in parent 2 only
//						d << "\t\t\tCopying link from network " << _p2.net.id << "\n";
						add_link(lt.first, net.get_node(n_id), *idx.second);
					}
				}
			}
		}
	}

	void Node::connect()
	{
		/// Make sure that the node is connected
		/// to at least one other node.

//		dlog() << "Connecting node " << id;

		/// Target checks for bias, input and hidden nodes.
		if ((id.role == NR::B ||
			 id.role == NR::I ||
			 id.role == NR::H) &&
			!has_targets(LT::F))
		{
			/// Pick a random target
			NodeID tgt_id;
			do
			{
				/// Pick a random target role
				do
				{
					tgt_id.role = cfg.w_dist(cfg.link.prob.at(LT::F).at(id.role));
				} while (net.node_count(tgt_id.role) == 0);

				/// Pick a random node in that role
				tgt_id.idx = cfg.rnd_key(net.nodes.at(tgt_id.role));
			} while (!link_allowed(LT::F, net.get_node(tgt_id)));

			/// Create the actual link
			add_link(LT::F, net.get_node(tgt_id));
		}

		/// Source checks for hidden and output nodes.
		if ((id.role == NR::H ||
			 id.role == NR::O) &&
			!has_sources(LT::F))
		{
			emap<NR, real> src_roles;
			for (const auto& srole : cfg.link.prob.at(LT::F))
			{
				for (const auto& trole : srole.second)
				{
					if (trole.first == id.role)
					{
						src_roles.emplace(srole.first, trole.second);
					}
				}
			}

			/// Pick a random source
			NodeID src_id;
			do
			{
				/// Pick a random source role
				do
				{
					src_id.role = cfg.w_dist(src_roles);
				} while (net.node_count(src_id.role) == 0);

				/// Pick a random node in that role
				src_id.idx = cfg.rnd_key(net.nodes.at(src_id.role));
			} while (!net.get_node(src_id).link_allowed(LT::F, *this));

			/// Create the actual link
			net.get_node(src_id).add_link(LT::F, *this);
		}
	}

	void Node::disconnect()
	{
		/// Delete source links
		for (auto& ltype : links.sources)
		{
			for (auto& nrole : ltype.second)
			{
				for (auto it = nrole.second.begin(); it != nrole.second.end(); )
				{
					Node& src(it->second.get().src);

//					dlog() << "Erasing link " << src.id << "->" << id;

					it = nrole.second.erase(it);
					src.links.targets.at(ltype.first).at(id.role).erase(id.idx);
				}
			}
		}

		/// Delete target links
		for (auto& ltype : links.targets)
		{
			for (auto& nrole : ltype.second)
			{
				for (auto it = nrole.second.begin(); it != nrole.second.end(); )
				{
//					dlog() << "Erasing link " << id << "->" << it->second->tgt.id;

					it->second->tgt.links.sources.at(ltype.first).at(id.role).erase(id.idx);
					it = nrole.second.erase(it);
				}
			}
		}
	}

	bool Node::forms_cycle(Node& _tgt)
	{
		net.reset_marks();
		add_link(LT::F, _tgt);
		bool is_dag(visit(false));
		erase_link(LT::F, _tgt.id);
		return !is_dag;
	}
}
