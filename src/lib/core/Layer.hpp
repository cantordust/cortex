#ifndef CORTEX_LAYER_HPP
#define CORTEX_LAYER_HPP

#include "Conf.hpp"

namespace Cortex
{
	/// @class Layer class.
	/// This serves as a container for nodes,
	/// which in turn contain all links.
	class Layer : Serialiser, public std::enable_shared_from_this<Layer>
	{
	public:

		/// Layer type
		const LayerType type{LayerType::Undef};

		/// Indicates whether the layer can be evolved
		const bool fixed{false};

	protected:

		/// Containing network
		Net* net{nullptr};

		/// Bias for the next layer.
		NodePtr bias{nullptr};

		/// Nodes belonging to this layer.
		std::vector<NodePtr> nodes;

	public:

		///=============================================================================
		///	Static methods
		///=============================================================================

		/// @brief Create a new layer
		static LayerPtr create(Net* const _net = nullptr);

		/// @brief Create a new layer
		static LayerPtr create(const LayerDef& _def = {},
							   Net* const _net = nullptr);

		/// @brief Clone an existing layer.
		/// @note This *does not* clone links
		/// since they might refer to layers or
		/// nodes that have not been created yet.
		static LayerPtr create(const Layer& _other,
							   Net* const _net = nullptr);

		/// @brief Get a shared pointer to this layer
		/// from within the layer.
		LayerPtr self();

		///=====================================
		/// Bookkeeping
		///=====================================

		/// @brief Consecutive index of the layer.
		/// This is unique for each layer within a network
		/// and can serve as a layer ID.
		opt<uint> get_index() const;

		/// @brief Preceding layer.
		/// @note Could return nullptr.
		LayerPtr get_prev() const;

		/// @brief Following layer.
		/// @note Could return nullptr.
		LayerPtr get_next() const;

		/// @brief Get the target nodes that this node can connect to.
		hset<NodePtr> get_free_targets(const NodePtr& _src,
									   const LinkType _lt = LinkType::Forward);

		/// @brief Get the nodes that can connect to this node.
		hmap<NodePtr, hset<sz>> get_free_sources(const NodePtr& _tgt,
												 const LinkType _lt = LinkType::Forward);

		/// @brief Layer configuration generated on demand.
		/// Used for matching layers to genomes.
		LayerDef get_def() const;


		///=====================================
		/// Counters
		///=====================================

		/// @brief Count layers of a specific type
		template<typename Countable, OnlyEnum<Countable>...>
		uint count(const Countable _countable) const;

		///=====================================
		/// Connections
		///=====================================

		/// @brief Populate the layer with nodes, optionally
		/// using a list of reference nodes.
		void init(const std::vector<NodePtr>& _ref_nodes);

		/// @brief Connect the layer to a number of sources and targets.
		void connect(const FanSize _in, const FanSize _out);

		/// @brief Connect nodes in the layer, optinally
		/// using a list of reference nodes.
		void connect(const std::vector<NodePtr>& _ref_nodes = {});

		/// @brief Disconnect all nodes in this layer.
		void disconnect();

		/// @brief Add a node from a definition
		void add_node(const NodeDef& _ref = {}, Layer* const _layer = nullptr);

		/// @brief Clone an existing node
		void add_node(const Node& _ref, Layer* const = nullptr);

		bool erase_link(const opt<uint> _node = nopt);

		bool erase_node(const uint _node);

		/// @brief Layer mutation (mutated configuration)
		void mutate(const MutationPtr& _mut);

		///=====================================
		/// Printing and (de)serialisation
		///=====================================

		/// @brief Print to a stream.
		friend os& operator<< (os& _os, const Layer& _layer);

		/// @brief Load from JSON.
		virtual void load(const json& _j) override;

		/// @brief Save to JSON.
		virtual void save(json& _j) const override;

	protected:

		///=====================================
		/// Constructors and destructors
		///=====================================

		/// @brief Create a new layer
		Layer(const LayerDef& _def = {},
			  Net* const _net = nullptr);

		/// @brief Clone an existng layer.
		Layer(const Layer& _other,
			  Net* const _net);

		Layer(const Layer& _other) = delete;

		Layer(Layer&& _other) = delete;

		friend class Net;
		friend class Model;
		friend class Node;
	};
}

#endif // CORTEX_LAYER_HPP
