#ifndef NODE_HPP
#define NODE_HPP

#include "Link.hpp"
#include "Activation.hpp"
#include "GRF.hpp"

namespace Cortex
{
	struct NodeID
	{
		NR role;
		uint idx;

		inline bool operator == (const NodeID& _other) const
		{
			return (role == _other.role && idx == _other.idx);
		}

		friend std::ostream& operator << (std::ostream& _strm, const NodeID& _id)
		{
			return _strm << "(" << _id.role << " " << _id.idx << ")";
		}
	};

	struct Spike
	{
		real time = 0.0;
		NodeID src = {NR::Undef, 0};
	};

	struct Event
	{
		real time = 0.0;
		LinkRef link;
	};

	using Scheduler = std::deque<Spike>;

	class Node
	{
	public:

		NodeID id;

		ConfigRef cfg;

		real output;

		// Helper enum class for topological sorting
		NodeMark mark;

	private:

		Net& net;

		Links links;

		Activation af;

		std::vector<real> input;

		// Time constant
		Param tau;

		// Gaussian receptive field
		// @todo Generic receptive field class.
		GRF grf;

		// Used in SNN
		struct
		{
			// Incoming spike events
			std::vector<Event> in;

			// Relative timing of the last outbound spike
			struct
			{
				real time = 0.0;
			} out;

			// Difference of spike event time and last outward spike time
			real t_stdp = 0.0;
		} spikes;

		friend class Net;
		friend class Link;
		friend class Links;

		friend std::ostream& operator<< (std::ostream&, const Net&);

	public:

		Node() = delete;

		Node(const Node& _other) = delete;

		Node(Node&& _other) = delete;

		explicit Node(const NodeID& _id, Net& _net);

		explicit Node(Node& _other, Net& _net);

		~Node();

		inline uint link_count(const LT _ltype) const
		{
			return std::accumulate(links.targets.at(_ltype).begin(),
								   links.targets.at(_ltype).end(),
								   0,
								   [&](uint _sum, auto& _table) { return _sum + _table.second.size(); });
		}

		inline uint link_count() const
		{
			return link_count(LT::F) + link_count(LT::R);
		}

		inline void add_input(const real _val)
		{
			input.push_back(_val);
		}

		inline void set_grf(const uint _idx)
		{
			grf = GRF(cfg, _idx);
		}

		inline real get_grf_delay(const real _input)
		{
			return grf.convert(_input);
		}

		inline bool set_fn(const Fn _fn)
		{
			af.set_fn(_fn);
			return true;
		}

		inline void propagate()
		{
			// First get all input from recurrent links
			if (cfg.get().link.rec)
			{
				propagate_rec();
			}

			// Evaluate the node
			eval();

			// Propagate the output
			if (output != 0.0)
			{
				for (auto& role : links.targets.at(LT::F))
				{
					for (auto& link : role.second)
					{
						link.second->tgt.add_input(output * link.second->weight.cur());
					}
				}
			}

			// Clear the input vector in preparation
			// for the next evaluation
			input.clear();
		}

		inline bool has_targets(const LT _lt) const
		{
			bool empty(true);
			for (const auto& nroles : links.targets.at(_lt))
			{
				empty &= nroles.second.empty();
			}
			return !empty;
		}

		inline bool has_sources(const LT _lt) const
		{
			bool empty(true);
			for (const auto& nroles : links.sources.at(_lt))
			{
				empty &= nroles.second.empty();
			}
			return !empty;
		}

		inline uint tgt_count(const LT _lt) const
		{
			return std::accumulate(links.targets.at(_lt).begin(),
								   links.targets.at(_lt).end(),
								   0,
								   [&](uint _sum, const auto& _nrole)
			{
				return _sum + _nrole.second.size();
			});
		}

		inline uint src_count(const LT _lt) const
		{
			return std::accumulate(links.sources.at(_lt).begin(),
								   links.sources.at(_lt).end(),
								   0,
								   [&](uint _sum, const auto& _nrole)
			{
				return _sum + _nrole.second.size();
			});
		}

		inline bool has_targets(const LT _lt, const NR _nr) const
		{
			return !links.targets.at(_lt).at(_nr).empty();
		}

		inline bool has_sources(const LT _lt, const NR _nr) const
		{
			return !links.sources.at(_lt).at(_nr).empty();
		}

		inline bool is_tgt(const LT _lt, const NodeID& _id) const
		{
			return links.targets.at(_lt).at(_id.role).find(_id.idx) != links.targets.at(_lt).at(_id.role).end();
		}

		inline bool is_src(const LT _lt, const NodeID& _id) const
		{
			return links.sources.at(_lt).at(_id.role).find(_id.idx) != links.sources.at(_lt).at(_id.role).end();
		}

		inline bool is_tgt(const NodeID& _id) const
		{
			return is_tgt(LT::F, _id) || is_tgt(LT::R, _id);
		}

		inline bool is_src(const NodeID& _id) const
		{
			return is_src(LT::F, _id) || is_src(LT::R, _id);
		}

		inline bool is_link_allowed(const LT _lt, Node& _tgt)
		{
			switch (_lt)
			{
			case LT::F:
				return !is_tgt(_tgt.id) && !forms_cycle(_tgt);

			case LT::R:
				return !is_tgt(_tgt.id) && forms_cycle(_tgt);

			default:
				return false;
			}
		}

		inline bool is_link_eraseable(const LT _lt, Node& _tgt) const
		{
			switch (_lt)
			{
			case LT::F:
				return is_tgt(_lt, _tgt.id) &&
						tgt_count(_lt) > 1 &&
						_tgt.src_count(_lt) > 1;

			case LT::R:
				return is_tgt(_lt, _tgt.id);

			default:
				return false;
			}
		}

		inline Link& get_tgt_link(const LT _lt, const NodeID& _id)
		{
			return *links.targets.at(_lt).at(_id.role).at(_id.idx);
		}

		inline Link& get_src_link(const LT _lt, const NodeID& _id)
		{
			return links.sources.at(_lt).at(_id.role).at(_id.idx).get();
		}

		inline void add_link(const LT _lt, Node& _tgt)
		{
			links.add(_lt, *this, _tgt, cfg);
		}

		inline void add_link(const LT _lt, Node& _tgt, Link& _other)
		{
			links.add(_lt, *this, _tgt, _other);
		}

		inline void erase_link(const LT _lt, const NodeID& _id)
		{
			links.erase(_lt, _id);
		}

		inline void reindex(const NodeID& _id)
		{
			links.reindex(_id);
		}

		void disconnect();

		void connect();

		void crossover(Node& _n1, Node& _n2, const hmap<uint, real>& _fdist);

		// Classical version
		void eval_classical();

		// Spiking version
		void eval_time(const Event& _ev);

		bool mutate(const Mut _mut);

		friend std::ostream& operator<< (std::ostream& _strm, const Node& _node);

	private:

		inline void propagate_rec()
		{
			for (auto& role : links.sources.at(LT::R))
			{
				for (auto& link : role.second)
				{
					if (link.second.get().src.output != 0.0)
					{
						add_input(link.second.get().src.output * link.second.get().weight.cur());
					}
				}
			}
		}

		bool visit(const bool _update_graph);

		bool forms_cycle(Node& _tgt);

		bool mutate_weight();

		bool mutate_tau();

		bool mutate_af();

		void stdp_classical();

		void stdp_pre_spike(const real _t_diff);

		void stdp_post_spike(const real _t_diff);
	};
}

#endif // NODE_HPP
