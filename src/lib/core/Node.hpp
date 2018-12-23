#ifndef CORTEX_NODE_HPP
#define CORTEX_NODE_HPP

#include "RField.hpp"
#include "Link.hpp"
#include "Spike.hpp"

namespace Cortex
{
	///=============================================================================
	/// @class A node in the network graph.
	///=============================================================================
	class Node : public Serialiser, public std::enable_shared_from_this<Node>
	{

	protected:

		/// Layer that this node belongs to.
		Layer* layer{nullptr};

		/// Virtual cube of vertices for this node.
		VCube vertices{1,1,1};

		/// Sources and corresponding links.
		/// The key in the inner map is the
		/// offset of the corresponding vertex.
		/// This ensures constant access time.
		hmap<Node*, hmap<sz, Link>> sources;

		/// Target nodes.
		/// Links from this node to target nodes can be accessed
		/// via the respective targets' source link collections.
		hset<Node*> targets;

		/// Membrane time constant.
		real tau;

		/// Membrane potential and statistics.
		real potential{0};

		/// Timing of the last input and output spike
		struct SpikeTime
		{
			real in{0.0};
			real out{0.0};
		} last;

	public:

		///=============================================================================
		///	Static methods
		///=============================================================================

		/// @brief Create a new node
		static NodePtr create(Layer* const _layer = nullptr);

		/// @brief Create a new node
		static NodePtr create(const NodeDef& _def,
							  Layer* const _layer = nullptr);

		/// @brief Clone an existing layer.
		/// @note This *does not* clone links
		/// since they might refer to layers or
		/// nodes that have not been created yet.
		static NodePtr create(const Node& _other,
							  Layer* const _layer = nullptr);

		///=============================================================================
		///	Instance methods
		///=============================================================================

		~Node();

		/// @brief Get a shared pointer to this node
		/// from within the node.
		NodePtr self();

		///=====================================
		/// Bookkeeping
		///=====================================

		/// @brief Consecutive index of the node in the layer.
		/// This is unique for each node within a layer and	can
		/// serve as a node ID if combined with the layer index
		/// Cf. get_id();
		uint get_index() const;

		/// @brief Node ID (layer index + node index).
		/// Those uniquely identify each node.
		NodeID get_id() const;

		/// @brief Preceding node.
		/// Could return nullptr.
		NodePtr get_prev() const;

		/// @brief Following node.
		/// Could return nullptr.
		NodePtr get_next() const;

		/// @brief Get a random free vertex
		/// from a link source candidate.
		hset<sz> get_free_vertices(Node* const _src, const sz _radius);

		/// @brief Retrieve a vertex pointer
		/// from a vertex offset.
		sz get_vertex(const sz _offset);

		/// @brief Determine the radius of the
		/// receptive field for source node @p _src.
		sz get_kernel_size(Node* const _src);

		/// @brief Get the node definition
		NodeDef get_def() const;

		///=====================================
		/// Counters
		///=====================================

		/// @brief Count Nodes of type @p _et.
		template<typename Countable, OnlyEnum<Countable>...>
		uint count(const Countable _countable) const;

		///=====================================
		/// Connections
		///=====================================

		void init();

		/// @brief Connect the node to a number of sources and targets.
		void connect(const FanSize _in = FanSize::Random,
					 const FanSize _out = FanSize::Random);

		/// @brief Connect the node by using another node as a reference.
		void connect(Node& _ref_node);

		/// @brief Disconnect the node from the network.
		void disconnect();

		/// @brief Add a new link of type @p _lt
		bool add_link(Node& _src,
					  const sz _vertex,
					  const LinkType _lt,
					  const opt<real> _weight = nopt);

		/// @brief Replicate a reference link.
		bool add_link(Node& _src,
					  const sz _vertex,
					  Link& _other);

		/// @brief Erase a link to source @p _src.
		/// This automatically removes the link from
		/// the target's link tables as well.
		bool erase_link(Node& _src,
						const opt<sz> _offset = nopt);

		///=====================================
		/// Printing and (de)serialisation
		///=====================================

		/// @brief Print to a stream.
		friend os& operator << (os& _os, const Node& _node);

		/// @brief Load from JSON.
		virtual void load(const json& _j) override;

		/// @brief Save to JSON.
		virtual void save(json& _j) const override;

		friend bool operator < (const Spike& _lhs, const Spike& _rhs) noexcept;

	protected:

		Node() = delete;

		/// @brief Create a new node
		Node(const NodeDef& _def = {},
			 Layer* const _layer = nullptr);

		//		/// @brief Clone an existing node
		//		Node(const Node& _other,
		//			 Layer* const _layer = nullptr);

		/// @brief Copy constructor
		Node(const Node& _other) = delete;

		/// @brief Move constructor
		Node(Node&& _other) = delete;

		friend class Net;
		friend class Model;
		friend class RField;
		friend class Layer;
		friend class Link;
		friend class Vertex;
	};
}

#endif // CORTEX_NODE_HPP
