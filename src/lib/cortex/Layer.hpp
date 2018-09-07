#ifndef CORTEX_LAYER_HPP
#define CORTEX_LAYER_HPP

#include "Conf.hpp"

namespace Cortex
{
	/// @class Layer class.
	/// This serves as a container for nodes,
	/// which in turn contain all links.
	template<>
	class Element<ElemType::Layer>
	{

	protected:

		/// Containing network.
		Net& net;

		/// Layer configuration.
		LayerConf lconf;

		/// Nodes belonging to this layer
		std::vector<NodePtr> nodes;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		Element(Net& _net, const LayerConf _conf);

		///=====================================
		/// Connections
		///=====================================

		/// @brief Populate the layer with nodes.
		void init(const std::vector<Node*>& _ref_nodes = {});

		/// Connect all nodes in the layer.
		void connect(const std::vector<Node*>& _ref_nodes = {});

		/// @brief Disconnect all nodes in this layer.
		void disconnect();

		///=====================================
		/// Bookkeeping
		///=====================================

		/// @brief Consecutive index of the layer.
		/// This is unique for each layer within a network
		/// and can serve as a layer ID.
		uint index() const;

		/// @brief Preceding layer.
		/// @note Could return nullptr.
		Layer* const prev() const;

		/// @brief Following layer.
		/// @note Could return nullptr.
		Layer* const next() const;

		/// @brief First layer.
		Layer* const front() const;

		/// @brief Last layer.
		Layer* const back() const;

		///=====================================
		/// Counters
		///=====================================

		/// @brief Count elements of a specific type
		uint count(const ElemType _et =	ElemType::Undef) const;

		/// @brief Count links of a specific type
		uint count(const LinkType _lt = LinkType::Undef) const;

		///=====================================
		/// Evolution
		///=====================================

		/// @brief Layer mutation (mutated configuration)
		void mutate(const LayerConf& _lc);

		///=====================================
		/// Serialisation and printing
		///=====================================

		json to_json() const;

		friend os& operator<< (os& _os, const Layer& _layer);

		///=====================================
		/// Friends
		///=====================================

		friend class Net;
		friend class Evaluator;
		template<ElemType> friend class Element;
	};
}

#endif // CORTEX_LAYER_HPP
