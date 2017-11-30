#include "Net.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	Node::Node(const NodeID& _id, Net& _net)
		:
		  id(_id),
		  cfg(_net.cfg),
		  net(_net),
		  af(_id.role, _net.cfg),
		  mark(NodeMark::Undef),
		  output(0.0),
		  tau(_net.cfg, _net.cfg.get().node.tau)
	{}

	Node::Node(Node& _other, Net& _net)
		:
		  id(_other.id),
		  cfg(_net.cfg),
		  net(_net),
		  af(_other.id.role, _net.cfg),
		  mark(NodeMark::Undef),
		  output(0.0),
		  tau(_other.tau),
		  grf(_other.grf)
	{}

	Node::~Node()
	{
		disconnect();
	}

	inline void Node::eval_classical()
	{
		output = af.eval(input);

		// STDP if enabled
		if (cfg.get().stdp.enabled)
		{
			stdp_classical();
		}
	}

	void Node::eval_time(const Event& _ev)
	{
		// Compute the current activation level
		// based on the last evaluation time.
		if (cfg.get().node.lif)
		{
			output *= std::exp( -(_ev.time - (spikes.in.empty() ? spikes.out.time : spikes.in.back().time)) / tau.cur() );
		}

		spikes.in.emplace_back(_ev);
		output += _ev.link.get().weight.cur();
		spikes.out.time += _ev.time;

		if (output >= 1.0)
		{
			// The firing threshold has been reached.

			// STDP
			if (cfg.get().stdp.enabled)
			{
				// Set the timing of the last incoming spike to 0
				spikes.in.back().time = 0.0;

				// Reset the time difference between the current
				// outward spike time and the event time
				spikes.t_stdp = 0.0;

				// Go back through the spike events and apply STDP
				while (!spikes.in.empty())
				{
					if (spikes.t_stdp > cfg.get().stdp.span.pre)
					{
						break;
					}
					stdp_pre_spike(spikes.t_stdp);
					spikes.t_stdp += spikes.in.back().time;
					spikes.in.pop_back();
				}
			}
			//				propagate();

			// Reset the neuron to the resting potential
			output = 0.0;

			// The activation has reached the firing threshold
			spikes.out.time = 0.0;

			// Clear the spike queue
			spikes.in.clear();
		}
		else if (cfg.get().stdp.enabled &&
				 spikes.out.time < cfg.get().stdp.span.post)
		{
			stdp_post_spike(spikes.out.time);
		}
	}

	bool Node::mutate(const Mut _mut)
	{
		switch (_mut)
		{
		case Mut::Weight:
			return mutate_weight();

		case Mut::Tau:
			return mutate_tau();

		case Mut::Fn:
			return mutate_af();

		default:
			return false;
		}
	}

	void Node::crossover(Node& _p1, Node& _p2, const hmap<uint, real>& _fdist)
	{
		// Iterate over link types
		for (auto&& lt : links.targets)
		{
			//	Iterate over node roles
			for (auto&& nr : lt.second)
			{
				// Iterate over indices in parent 1
				for (auto&& idx : _p1.links.targets.at(lt.first).at(nr.first))
				{
					// Temporary target node ID
					NodeID tgt_id{nr.first, idx.first};

					if (is_link_allowed(lt.first, net.get_node(tgt_id)))
					{
						bool from_p1(cfg.get().w_dist(_fdist) == _p1.net.id);

						// Check if we should copy the link from parent 1
						if (from_p1)
						{
							// Copy link from parent 1
							add_link(lt.first, net.get_node(tgt_id), *idx.second);
						}
						// Otherwise check if the link exists in parent 2
						else if (_p2.is_tgt(lt.first, tgt_id))
						{
							// Copy link from parent 2
							add_link(lt.first, net.get_node(tgt_id), _p2.get_tgt_link(lt.first, tgt_id));
						}
					}
				}

				// Iterate over indices in parent 2
				for (auto&& idx : _p2.links.targets.at(lt.first).at(nr.first))
				{
					// Temporary target node ID
					NodeID tgt_id{nr.first, idx.first};

					if (!_p1.is_tgt(lt.first, tgt_id) &&
						cfg.get().w_dist(_fdist) == _p2.net.id &&
						is_link_allowed(lt.first, net.get_node(tgt_id)))
					{
						// Copy link from parent 2
						add_link(lt.first, net.get_node(tgt_id), *idx.second);
					}
				}
			}
		}
	}

	void Node::connect()
	{
		// Make sure that the node is connected
		// to at least one other node.

//		dlog() << "Connecting node " << id;

		// Target checks for all nodes other than output ones.
		if (id.role != NR::O &&
			!has_targets(LT::F))
		{
			// ID of the target node
			NodeID tgt_id;
			do
			{
				// Pick a random target role
				do
				{
					tgt_id.role = cfg.get().w_dist(cfg.get().link.prob.at(LT::F).at(id.role));
				} while (net.get_node_count(tgt_id.role) == 0);

				// Pick a random node in that role
				tgt_id.idx = cfg.get().rnd_key(net.nodes.at(tgt_id.role));

			} while (!is_link_allowed(LT::F, net.get_node(tgt_id)));

			// Create the link
			add_link(LT::F, net.get_node(tgt_id));
		}

		// Source checks for hidden and output nodes.
		if ((id.role == NR::H ||
			 id.role == NR::O) &&
			!has_sources(LT::F))
		{
			emap<NR, real> src_roles;
			for (const auto& srole : cfg.get().link.prob.at(LT::F))
			{
				for (const auto& trole : srole.second)
				{
					if (trole.first == id.role)
					{
						src_roles.emplace(srole.first, trole.second);
					}
				}
			}

			// ID of the source node
			NodeID src_id;
			do
			{
				// Pick a random source role
				do
				{
					src_id.role = cfg.get().w_dist(src_roles);
				} while (net.get_node_count(src_id.role) == 0);

				// Pick a random node in that role
				src_id.idx = cfg.get().rnd_key(net.nodes.at(src_id.role));

			} while (!net.get_node(src_id).is_link_allowed(LT::F, *this));

			// Create the link
			net.get_node(src_id).add_link(LT::F, *this);
		}
	}

	void Node::disconnect()
	{
		// Delete source links
		for (auto&& ltype : links.sources)
		{
			for (auto&& nrole : ltype.second)
			{
				for (auto it = nrole.second.begin(); it != nrole.second.end(); )
				{
					// Temporary alias of the source node
					Node& src(it->second.get().src);

					// Erase the link from this node
					it = nrole.second.erase(it);

					// Erase the link from the source node
					src.links.targets.at(ltype.first).at(id.role).erase(id.idx);
				}
			}
		}

		// Delete target links
		for (auto&& ltype : links.targets)
		{
			for (auto&& nrole : ltype.second)
			{
				for (auto it = nrole.second.begin(); it != nrole.second.end(); )
				{

					it->second->tgt.links.sources.at(ltype.first).at(id.role).erase(id.idx);
					it = nrole.second.erase(it);
				}
			}
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
				mark = NodeMark::Perm;
				if (_update_graph)
				{
					net.graph.emplace_back(*this);
				}
				return true;
			}
		}
	}

	inline bool Node::forms_cycle(Node& _tgt)
	{
		net.reset_marks();
		add_link(LT::F, _tgt);
		bool is_dag(visit(false));
		erase_link(LT::F, _tgt.id);
		return !is_dag;
	}

	inline bool Node::mutate_weight()
	{
		uint idx(0);
		NR other_role(NR::Undef);

		// There must be at least one link
		// whose weight can be mutated.
		while (true)
		{
			LT lt(cfg.get().w_dist(cfg.get().link.type));
			if (!cfg.get().link.rec ||
				(!has_targets(LT::R) &&
				 !has_sources(LT::R)))
			{
				lt = LT::F;
			}

			other_role = cfg.get().w_dist(cfg.get().mutation.node);

			if (has_targets(lt, other_role))
			{
				idx = cfg.get().rnd_key(links.targets.at(lt).at(other_role));
				return links.targets.at(lt).at(other_role).at(idx)->weight.mutate(net.fitness);
			}
			else if (has_sources(lt, other_role))
			{
				// Special case for output neurons
				idx = cfg.get().rnd_key(links.sources.at(lt).at(other_role));
				return links.sources.at(lt).at(other_role).at(idx).get().weight.mutate(net.fitness);
			}
		}

	}

	inline bool Node::mutate_tau()
	{
		return tau.mutate(net.fitness);
	}

	inline bool Node::mutate_af()
	{
		return af.set_fn(cfg.get().w_dist(cfg.get().mutation.fn.at(id.role)));
	}

	void Node::stdp_classical()
	{

//		for (uint tgt = 1; tgt <= nodes_old.size(); ++tgt)
//		{
//			if (!phenome.is_empty(LT::S, tgt))
//			{
//				// The node has sources (it is not an input or a bias node)
//				for (const uint src : phenome.get(LT::S, tgt))
//				{
//					// Synaptic plasticity.
//					// This should be self-regulating
//					// and should not lead to runaway excitation.
//					real new_w( link(src, tgt).get(Mut::Weight) );
//					new_w += cfg.get().stdp.lr * nodes_old.at(tgt).get_output() * ( nodes_old.at(src).get_output() - new_w * nodes_old.at(tgt).get_output() );

//					if (new_w > cfg.get().link.params[Mut::Weight].max)
//					{
//						new_w = cfg.get().link.params[Mut::Weight].max;
//					}

//					link(src, tgt).set(Mut::Weight, new_w);
//				}
//			}
//		}

//		if (cfg.get().link.rec.enabled)
//		{
//			for (uint src = 1; src <= nodes_old.size(); ++src)
//			{
//				if (!phenome.is_empty(LT::R, src))
//				{
//					for (const uint tgt : phenome.get(LT::R, src))
//					{
//						real new_w( link(src, tgt).get(Mut::Weight) );
//						new_w += cfg.get().stdp.lr * nodes_old.at(tgt).get_output() * ( nodes_old.at(src).get_output() - new_w * nodes_old.at(tgt).get_output() );

//						if (new_w > cfg.get().link.params[Mut::Weight].max)
//						{
//							new_w = cfg.get().link.params[Mut::Weight].max;
//						}

//						link(src, tgt).set(Mut::Weight, new_w);
//					}
//				}
//			}
//		}

	}

	void Node::stdp_pre_spike(const real _t_diff)
	{
		real decay( std::exp( - _t_diff / cfg.get().stdp.span.pre ) );
		real w(spikes.in.back().link.get().weight.cur());
		real dw(cfg.get().stdp.lr * decay * cfg.get().stdp.alpha * w);

		switch (cfg.get().stdp.type)
		{
		case STDP::Heb:
			spikes.in.back().link.get().weight.update(dw);
			break;

		case STDP::AntiHeb:
			spikes.in.back().link.get().weight.update(-dw);
			break;

		default:
			break;
		}

		if (tgt_last_spike_time > 0)
		{
			real w(link(src, tgt).get(Mut::Weight));


			// @todo Make the decay cutoff a parameter
			if (decay > 0.05)
			{
				switch (cfg.get().stdp.type)
				{
				case STDP::Heb:
					{


						if (w >= 0)
						{
							link(src, tgt).set(Mut::Weight, w - dw);
						}
						else
						{
							link(src, tgt).set(Mut::Weight, w + dw);
						}

						break;
					}

				case STDP::AntiHeb:
					{
						real dw(cfg.get().stdp.lr * decay * (cfg.get().link.params[Mut::Weight].max - std::abs(w)));

						if (w >= 0)
						{
							link(src, tgt).set(Mut::Weight, std::abs(w) + dw);
						}
						else
						{
							link(src, tgt).set(Mut::Weight, std::abs(w) - dw);
						}

						break;
					}

				default:
					break;
				}
			}
		}
	}

	void Node::stdp_post_spike(const real _t_diff)
	{

		// Post-spike

		real decay( std::exp( - _t_diff / cfg.get().stdp.span.post ) );
		real dw(cfg.get().stdp.lr * decay * cfg.get().stdp.alpha * spikes.in.back().link.get().weight.cur());


		//		real cur_time(scheduler.top().first);
		//		uint tgt(scheduler.top().second.second);
//		std::priority_queue<event, std::vector<event>> spike_times;

//		for (const uint src : phenome.get(LT::S, tgt))
//		{
//			if (nodes_old.at(src).get_role() != NR::B)
//			{
//				real src_last_spike_time( nodes_old.at(src).get_last_spike_time() );

//				if (src_last_spike_time > 0.0 &&
//					src_last_spike_time + link(src, tgt).get(Mut::Delay) <= cur_time)
//				{
//					spike_times.emplace(src_last_spike_time, src);
//				}
//			}
//		}

//		while (!spike_times.empty())
//		{
//			real spike_time(spike_times.top().first);
//			spike_times.pop();

//			uint src(spike_times.top().second);
//			real w( std::fabs( link(src, tgt).get(Mut::Weight) ) );
//			real decay( std::exp( (spike_time - cur_time) / cfg.get().stdp.span ) );

//			//							std::cout << "Afferent spike time: " << aff_spike_time << ", hebbian learning for link " << afferent_spikes.top().second << "->" << target_id << std::endl;
//			//						std::cout << "(s) diff: " << aff_spike_time - current_time << ", decay: " << decay << std::endl;
//			//						std::cin.get();
//			if (decay > 0.05)
//			{
//				switch (cfg.get().stdp.type)
//				{
//				case STDP::Heb:
//					link(src, tgt).set(Mut::Weight, w + cfg.get().stdp.lr * (cfg.get().link.params[Mut::Weight].max - w) * decay);
//					break;

//				case STDP::AntiHeb:
//					link(src, tgt).set(Mut::Weight, w - cfg.get().stdp.lr * cfg.get().stdp.alpha * w * decay);
//					break;

//				default:
//					break;
//				}
//			}

//		}
	}

	std::ostream& operator<< (std::ostream& _strm, const Node& _node)
	{
		_strm << _node.id;
		return _strm;
	}
}
