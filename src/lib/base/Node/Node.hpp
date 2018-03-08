#ifndef CORTEX_NODE_HPP
#define CORTEX_NODE_HPP

#include "Axon.hpp"
#include "Soma.hpp"
#include "Link.hpp"

namespace Cortex
{
	class Node
	{
	public:

		NodeID id;

		Conf& conf;

	private:

		/// Containing network
		Net& net;

		/// This is where incoming signals are integrated
		Soma soma;

		/// Signal produced after integrating the input
		Axon axon;

		/// Source and target links
		Links links;

		/// Helper enum class for topological sorting
		/// @todo Remove this enum entirely and implement
		/// topological sorting with explicit stack.
		NodeMark mark;

		/// Transfer function
		TF tf;

		/// Decay time constant
		Param tau;

		/// @todo Firing threshold
		/// Param ft;

	public:

		///=========================================
		/// Node.cpp
		///=========================================

		Node() = delete;

		Node(const Node& _other) = delete;

		Node(Node&& _other) = delete;

		explicit Node(const NR _nr, const uint _idx, Net& _net);

		explicit Node(const Node& _other, Net& _net);

		void connect();

		void disconnect();

		bool set_tf(const TF _tf);

		const LinkPtr get_src_link(const NodeID& _id) const;

		const LinkPtr get_src_link(const NodePtr& _nptr) const;

		const LinkPtr get_tgt_link(const NodeID& _id) const;

		const LinkPtr get_tgt_link(const NodePtr& _nptr) const;

		NodePtr get_rnd_src(const NR _tr = NR::Undef, const LT _lt = LT::Undef);

		NodePtr get_rnd_tgt(const NR _sr = NR::Undef, const LT _lt = LT::Undef);

		///=========================================
		/// Checks.cpp
		///=========================================

		uint src_count(const LT _lt = LT::Undef) const;

		uint tgt_count(const LT _lt = LT::Undef) const;

		bool has_sources(const NR _nr = NR::Undef, const LT _lt = LT::Undef) const;

		bool has_targets(const NR _nr = NR::Undef, const LT _lt = LT::Undef) const;

		bool is_src(const NodeID& _id, const LT _lt = LT::Undef) const;

		bool is_src(const NodePtr& _nptr, const LT _lt = LT::Undef) const;

		bool is_tgt(const NodeID& _id, const LT _lt = LT::Undef) const;

		bool is_tgt(const NodePtr& _nptr, const LT _lt = LT::Undef) const;

		bool link_exists(const NodeID& _id, const LT _lt = LT::Undef) const;

		bool link_exists(const NodePtr& _nptr, const LT _lt = LT::Undef) const;

		bool is_link_allowed(const NodeID& _id, const LT _lt = LT::Undef);

		bool is_link_allowed(const NodePtr& _nptr, const LT _lt = LT::Undef);

		bool is_link_eraseable(const NodeID& _id, const LT _lt = LT::Undef);

		bool is_link_eraseable(const NodePtr& _nptr, const LT _lt = LT::Undef);

		///=========================================
		/// Evolution.cpp
		///=========================================

		bool add_link(const NodeID& _id, const LT _lt = LT::Undef);

		bool add_link(const NodePtr& _nptr, const LT _lt = LT::Undef);

		bool add_link(const NodeID& _id, const LinkPtr& _other);

		bool add_link(const NodePtr& _nptr, const LinkPtr& _other);

		bool erase_link(const NodeID& _id);

		bool erase_link(const NodePtr& _nptr);

		void crossover(const NodePtr& _p1, const NodePtr& _p2, const hmap<uint, real>& _fdist);

		bool mutate(const Mut _mut);

		template<Mut mut>
		bool mutate();

		///=========================================
		/// Evaluation.cpp
		///=========================================

		/// @brief Node evaluation.
		/// Procedure:
		/// - Receive input
		/// - Integrate and transfer the input and store the output
		/// - (Optional) Hebbian learning
		/// - (Optional) Clear the input
		template<NT nt>
		void eval();

		template<Enc enc>
		void eval(const LinkPtr& _link);

	private:

		///=========================================
		/// Node.cpp
		///=========================================

		/// @brief Returns a node pointer to this node
		const NodePtr self();

		///=========================================
		/// Checks.cpp
		///=========================================

		bool visit(const bool _update_graph);

		bool forms_cycle(const NodeID& _id);

		bool forms_cycle(const NodePtr& _nptr);

		///=========================================
		/// Evaluation.cpp
		///=========================================

		template<NT nt>
		void receive_input();

		template<Enc enc>
		void receive_input(const LinkPtr& _link);

		///=========================================
		/// Transfer.cpp
		///=========================================

		/// @brief Schedule all target neurons for evaluation.
		/// This is used for spiking networks.
		void schedule(const real _time);

		/// @brief Compute the output via the transfer function.
		template<NT nt>
		void transfer();

		/// @brief Transfer functions for spiking networks.
		template<Enc enc>
		void transfer();

		/// @brief Transfer functions for classical networks.
		template<TF tf>
		void transfer();

		///=========================================
		/// Friends
		///=========================================

	public:

		friend std::ostream& operator<< (std::ostream& _strm, const Node& _node);

	private:

		friend class Net;
		friend class Link;
		friend class Links;
		friend class Soma;
		friend class RField;
	};
}

#endif // CORTEX_NODE_HPP
