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

	class Node
	{
	public:

		NodeID id;

		Config& cfg;

		real output;

		/// Helper enum class for topological sorting
		Mark mark;

	private:

		Net& net;

		Links links;

		Activation af;

		Param tau;

		std::vector<real> input;

		/// Gaussian receptive field
		/// \todo: Generic receptive field class.
		GRF grf;

		/// The current activation level
		real al;

		/// Hyperpolarisation level.
		/// This should be lower than the resting potential.
		real hl;

		/// The last time this node received an input.
		real last_input;

		/// The last time this node spiked
		real last_spike;

		friend class Net;
		friend std::ostream& operator<< (std::ostream&, const Net&);
		friend class Link;
		friend class Links;

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

		/// Classical network
		inline void eval()
		{
			output = af.eval(input);
		}

		/// Spiking network
		inline void eval(const real& _cur_time, const real& _input)
		{
			/// Compute the current activation level
			/// based on the last evaluation time.
			if (cfg.node.lif)
			{
				al *= std::exp( -(_cur_time - last_input) / tau.val() );
			}

			al += _input;
			last_input = _cur_time;

			if ( al >= 1.0 )
			{
				al = 0.0;
				last_spike = _cur_time;
			}
		}

		inline void add_input(const real _val)
		{
			input.push_back(_val);
		}

		inline void set_grf(const uint _N,
							const uint _i,
							const real _beta,
							const real _i_min,
							const real _i_max)
		{
			grf = GRF(_N, _i, _beta, _i_min, _i_max);
		}

		inline real get_grf_delay(const real _input)
		{
			return grf.get_delay(_input);
		}

		bool mutate(const Mut _mut);

		inline real get_last_spike_time() const
		{
			return last_spike;
		}

		inline bool set_fn(const Fn _fn)
		{
			af.set_fn(_fn);
			return true;
		}

		inline void set_last_spike_time(const real _t)
		{
			last_spike = _t;
		}

		inline void propagate()
		{
			/// First get all input from recurrent links
			if (cfg.link.rec)
			{
				get_rec();
			}

			/// Evaluate the node
			eval();

			/// Propagate the output
			if (output != 0.0)
			{
				for (auto& role : links.targets.at(LT::F))
				{
					for (auto& link : role.second)
					{
						link.second->tgt.add_input(output * link.second->weight.val());
					}
				}
			}

			/// Clear the input vector in preparation
			/// for the next evaluation
			input.clear();
		}

		inline void get_rec()
		{
			for (auto& role : links.sources.at(LT::R))
			{
				for (auto& link : role.second)
				{
					if (link.second.get().src.output != 0.0)
					{
						add_input(link.second.get().src.output * link.second.get().weight.val());
					}
				}
			}
		}

		bool visit(const bool _update_graph);

		void connect();

		void crossover(Node& _n1, Node& _n2, const hmap<uint, real>& _fdist);

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

		inline bool link_allowed(const LT _lt, Node& _tgt)
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

		inline bool link_eraseable(const LT _lt, Node& _tgt) const
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

		bool forms_cycle(Node& _tgt);

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
			//		dlog d;
			//		d << "*** Adding " << _lt << " link " << id << "->" << _tgt.id << "...";
			links.add(_lt, *this, _tgt, cfg);
			//		d << "success!";
		}

		inline void add_link(const LT _lt, Node& _tgt, Link& _other)
		{
			//		dlog d;
			//		d << "*** Copying " << _lt << " link " << id << "->" << _tgt.id << "...";
			links.add(_lt, *this, _tgt, _other);
			//		d << "success!";
		}

		inline void erase_link(const LT _lt, const NodeID& _id)
		{
			//		dlog d;
			//		d << "*** Erasing " << _lt << " link " << id << "->" << _id << "...";
			links.erase(_lt, _id);
			//		d << "success!";
		}

		inline void reindex(const NodeID& _id)
		{
			links.reindex(_id);
		}

		void disconnect();

		friend std::ostream& operator<< (std::ostream& _strm, const Node& _node)
		{
			_strm << _node.id;
			return _strm;
		}

	private:


	};
}

#endif // NODE_HPP
