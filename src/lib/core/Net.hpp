#ifndef CORTEX_NET_HPP
#define CORTEX_NET_HPP

#include "Fitness.hpp"

namespace Cortex
{
	class Net : public std::enable_shared_from_this<Net>, Serialiser
	{
	public:

		///=============================================================================
		///	Static attributes
		///=============================================================================

		/// Environment as a collection of nets.
		inline static hset<NetPtr> nets;

		/// The fittest network.
		inline static NetPtr champ{nullptr};

		/// ID factory.
		inline static IDGenerator id_generator;

		///=============================================================================
		///	Instance attributes
		///=============================================================================

		const uint id{0};

		/// Age of the network in generations.
		uint age{0};

		/// Optimisation stage.
		Stage stage{Stage::Train};

		/// Network fitness.
		Fitness fitness;

	protected:

		/// The network layers are expressions of
		/// the chromosomes in the genome,
		/// Nodes serve the role of individual genes.
		std::vector<LayerPtr> layers;

		/// Evaluation model
		ModelPtr model;

	public:

		///=============================================================================
		///	Static methods
		///=============================================================================

		/// @brief Create a new network from layer configurations.
		static NetPtr create(const std::vector<LayerDef>& _defs = {});

		/// @brief Create a new network from layer configurations.
		static NetPtr create(const Genome& _genome);

		/// @brief Create a new network using a strand of DNA.
		static NetPtr create(const DNA& _dna);

		/// @brief Clone an existing network.
		static NetPtr create(const Net& _other);

		/// @brief Create a new network as an offspring of two parent networks.
		static NetPtr create(const Net& _parent1,
							 const Net& _parent2);

		/// @brief Evolve all networks.
		static void evolve();

		/// @brief Update the relative fitness values
		/// of networks and species.
		static void calibrate();

		/// @brief Cull empty species and unfit networks.
		static void cull();

		///=============================================================================
		///	Instance methods
		///=============================================================================

		/// @brief Destructor.
		/// @note This disconnects all nodes in the network.
		~Net();

		/// @brief Get a shared pointer to this network
		/// from within the network.
		NetPtr self();

//		///=====================================
//		/// Operators
//		///=====================================

//		/// @brief Copy assignment.
//		Net& operator = (Net const& _other) = delete;

//		/// @brief Move assignment.
//		Net operator = (Net&& _other) = delete;

		/// @brief Check if the network has reached its EOL.
		bool is_old() const;

		/// @brief Count elements of a specific type.
		template<typename Type, OnlyEnum<Type>...>
		uint count(const Type _type) const;

		///=====================================
		/// Setters, getters and counters
		///=====================================

		/// @brief Set the absolute fitness of the network.
		void set_fitness(const real _val);

		/// @brief Total number of links allowed.
		uint get_max_links() const;

//		/// @brief Get the first layer of type @p _lt.
//		opt<uint> get_first(const LayerType _lt);

//		/// @brief Get the last layer of type @p _lt.
//		opt<uint> get_last(const LayerType _lt);

//		opt<uint> get_nth(const LayerType _lt, const uint _n);

		/// @brief Get the n-th layer of type @p _lt.
		LayerPtr get_layer(const LayerType _type, const uint _index);

		/// @brief Get a list of layers with nodes which can
		/// serve as potential sources for links of type @p _lt.
		DNA get_source_layers(const LayerPtr& _layer, const LinkType _lt);

		/// @brief Get a list of layers with nodes which can
		/// serve as potential targets for links of type @p _lt.
		DNA get_target_layers(const LayerPtr& _layer, const LinkType _lt);

		/// @brief Saturation of the network in terms of the number
		/// of existing links compared to the maximal allowed of links.
		///
		/// @note The number of links in a fully connected network is
		///
		/// \frac{N(N-1)}{2}, <-- @todo LaTeXified documentation?
		///
		/// where N is the number of nodes.
		/// This function computes the proportion of exisitng links
		/// relative to the maximal possible number of links.
		/// The result can be used to determine whether a structural
		/// mutation should add a node or another link to an existing node.
		///
		/// @warning Caveat emptor: the result is likely an underestimate because
		/// certain types of links (such as input-input) are not allowed,
		/// but get_max_links() (cf. above) does not take that into account.
		///
		/// @todo A more accurate method which accounts for disallowed link types.
		real get_link_density() const;

		/// @brief Get the DNA of this network.
		/// This is used to compare the phenotype
		/// to potentially matching genomes.
		const DNA& get_dna() const;

		/// @brief Get the genome of this network.
		/// Only applicable if speciation is enabled.
		GenomePtr get_genome() const;

		/// @brief Get a node pointer from a node ID.
		NodePtr get_node(const NodeID& _id);

		/// @brief Chance of this network getting retired.
		real get_cull_chance(const SMAStat& _age_stat);

		///=====================================
		/// Structural changes
		///=====================================

		/// @brief Add a new layer with supplied configuration
		LayerPtr add_layer(const LayerDef& _def = {}, const LayerPtr& _next = nullptr);

		/// @brief Add a new layer containing @p _nodes nodes.
		LayerPtr add_layer(const std::vector<NodePtr>& _nodes, const LayerPtr& _next = nullptr);

		/// @brief Clone a layer.
		LayerPtr add_layer(const LayerPtr& _ref, const LayerPtr& _next = nullptr);

		/// @brief Erase a layer.
		void erase_layer(const LayerPtr& _layer = nullptr);

		/// @brief Add a node to layer @p _layer
		void add_node(const LayerPtr& _layer = nullptr);

		/// @brief Erase a node from layer @p _layer.
		void erase_node(const LayerPtr& _layer, const opt<uint> _index);

		/// @brief Parameter mutation.
		void mutate();

		/// @brief Evolve the network based on its relative fitness.
		///
		/// Learning phase:
		///
		///		- High fitness: Reduce learning rate
		///
		///		- Low fitness:
		///			- Low link density: Add link
		///			- High link density: Erase link
		///
		/// Evolution phase:
		///
		///		- High fitness: Crossover
		///
		///		- Low fitness: Structural mutation
		///			- Low genome complexity: Add nodes
		///			- High genome complexity: Erase nodes
//		void evolve();

		/// @brief Evaluate network @p _net.
		void evaluate(const NetPtr& _net);

		///=====================================
		/// Evaluation
		///=====================================

		void train();

		/// @brief Evaluate the network.
		void evaluate(const Sample& _sample);

		///=====================================
		/// Printing and (de)serialisation
		///=====================================

		/// @brief Print to a stream.
		friend os& operator << (os& _os, const Net& _net);

		/// @brief Load from JSON.
		virtual void load(const json& _j) override;

		/// @brief Save to JSON.
		virtual void save(json& _j) const override;

	protected:

		/// @brief Default constructor.
		Net();

		/// @brief Constructor taking a list of layer definitions.
		Net(const std::vector<LayerDef>& _conf);

		/// @brief Constructor taking a reference strand of DNA.
		Net(const DNA& _dna);

		/// @brief Copy constructor.
		Net(const Net& _other) = delete;

		/// @brief Move constructor.
		Net(Net&& _other) = delete;

	private:

		/// @brief Connect layers and nodes,
		/// optionally using reference DNA.
		void connect(const DNA& _ref_dna = {});

		/// @brief Disconnect all nodes and layers.
		void disconnect();

		/// @brief Crossover procedure between two parent networks.
		/// The end result is the creation of a new phenotype
		/// (this network) by mixing nodes from the two parents.
		DNA crossover(const Net& _n1, const Net& _n2);

		friend class Model;
		friend class Genome;
		friend class RField;
		friend class Layer;
		friend class Node;
		friend class Link;
	};
}

#endif // CORTEX_NET_HPP
