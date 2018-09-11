#ifndef CORTEX_NODE_HPP
#define CORTEX_NODE_HPP

#include "Link.hpp"

namespace Cortex
{
	template<>
	class Element<ElemType::Node>
	{
	public:

		/// Node age in terms of generations
		/// since it was created.
		uint age = 0;

	protected:

		/// Layer that this node belongs to.
		Layer& layer;

		/// Sources and corresponding links.
		hmap<Node*, Link> sources;

		/// Target nodes.
		/// Links from this node to target nodes can be accessed
		/// via the targets' source link collection.
		hset<Node*> targets;

		/// Membrane potential and statistics.
		EMAStatPack potential;

		/// Timing of the last input and output spike
		struct SpikeTime
		{
			real in = 0.0;
			real out = 0.0;
		} last;

		/// Membrane time constant.
		Parameter tau;

		/// Transfer function.
		Func tf = Func::Tanh;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		Element() = delete;

		Element(const Node& _other) = delete;

		Element(Node&& _other) = delete;

		explicit Element(Layer& _layer, Node* const _other = nullptr);

		///=====================================
		/// Bookkeeping
		///=====================================

		/// @brief Consecutive index of the node in the layer.
		/// This is unique for each node within a layer and	can
		/// serve as a node ID if combined with the layer index
		/// Cf. id();
		uint index() const;

		/// @brief Node ID (layer index + node index)
		NodeID id() const;

		/// @brief Preceding node.
		/// Could return nullptr.
		Node* const prev() const;

		/// @brief Following node.
		/// Could return nullptr.
		Node* const next() const;

		/// @brief First node.
		Node* const front() const;

		/// @brief Last node.
		Node* const back() const;

		///=====================================
		/// Counters
		///=====================================

		/// @brief Count elements of a specific type
		uint count(const ElemType _et = ElemType::Undef) const;

		/// @brief Count links of a specific type.
		uint count(const LinkType _lt = LinkType::Undef) const;

		///=====================================
		/// Connections
		///=====================================

		/// @brief Connect the node to random sources and targets.
		void connect(Node* const _other = nullptr);

		/// @brief Disconnect the node from the network.
		void disconnect();

		///=====================================
		/// Evolution
		///=====================================

		/// @brief Add a new link of type @p _lt
		bool add_link(Node* const _src, const LinkType _lt = LinkType::Undef);

		/// @brief Replicate a reference link.
		bool add_link(Node* const _src, const Link& _other);

		/// @brief Erase a link to source @p _src.
		/// This automatically removes the link from
		/// the target's link tables as well.
		bool erase_link(Node* const _src = nullptr);

		///=====================================
		/// Serialisation and printing
		///=====================================

		json to_json();

		friend os& operator << (os& _os, const Node& _node);

		///=====================================
		/// Friends
		///=====================================

		friend class Net;
		friend class Evaluator;
		friend class RField;
		template<ElemType> friend class Element;
	};
}

#endif // CORTEX_NODE_HPP
