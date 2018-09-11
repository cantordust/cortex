#ifndef CORTEX_NET_HPP
#define CORTEX_NET_HPP

#include "Evaluator.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	class Net : public std::enable_shared_from_this<Net>
	{
	public:

		/// Network ID.
		/// This is generated automatically.
		const uint id;

		/// Age of the network in generations.
		uint age = 0;

		/// Optimisation stage
		Stage stage = Stage::Train;

	protected:

		/// Fitness statistics.
		/// This also serves as a fitness-based parameter optimiser.
		Fitness fitness;

		/// Species representing the genome for this network.
		/// The species can change as the network evolves.
		Species* species;

		/// Network layers.
		/// Nodes reside within layers, and links reside within nodes.
		std::vector<LayerPtr> layers;

		/// Evaluator built from the current network.
		/// Used for fast network evaluation.
		/// The evaluator's implementation should be
		///	decoupled from the network description.
		///
		/// @todo This paves the way to linking Cortex
		/// to established libraries such as TensorFlow.
		Evaluator evaluator;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		/// @brief Default constructor.
		Net() = delete;

		/// @brief Copy constructor.
		Net(Net const& _other) = delete;

		/// @brief Move constructor.
		Net(Net&& _other) = delete;

		/// @brief Explicit constructor taking a species pointer.
		/// This constructs a brand new network from scratch.
		explicit Net(const Genome& _genome);

		/// @brief Construct an offspring network from two parent networks.
		explicit Net(Net* const _parent1, Net* const _parent2);

		/// @brief Destructor.
		/// @note This calls Node::disconnect()
		/// for all nodes in the network.
		~Net();

		///=====================================
		/// Operators
		///=====================================

		/// @brief Copy assignment.
		Net& operator = (Net const& _other) = delete;

		/// @brief Move assignment.
		Net operator = (Net&& _other) = delete;

		///=====================================
		/// Bookkeeping
		///=====================================

		/// @brief Function providing consecutive ID values for networks.
		static uint get_id(const bool _reset = nopt);

		/// @brief Get a shared pointer to this network.
		/// Used in functions where the network must
		/// be identified by its pointer, as in Task::is_solved().
		const NetPtr self();

		///=====================================
		/// Age
		///=====================================

		/// @brief Increase the age of the network
		/// and all of its parameters.
		void increase_age();

		/// @brief Check if the network has reached its EOL.
		bool is_old() const;

		///=====================================
		/// Counters
		///=====================================

		/// @brief Count elements of a specific type.
		uint count(const ElemType _et = ElemType::Undef) const;

		/// @brief Count layers of a specific type.
		uint count(const LayerType _lt = LayerType::Undef) const;

		/// @brief Count links of a specific type.
		uint count(const LinkType _lt = LinkType::Undef) const;

		///=====================================
		/// Connections
		///=====================================

		/// @brief Connect layers and nodes.
		void connect();

		/// @brief Disconnect all nodes and layers.
		void disconnect();

		///=====================================
		/// Setters, getters and counters
		///=====================================

		/// @brief Set the absolute fitness of the network.
		void set_fitness(const real _val);

		/// @brief Total number of links allowed.
		uint get_max_links() const;

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

		/// @brief Get the genome of this network.
		/// This is constructed on demand by
		/// iterating over the layers.
		Genome get_genome() const;

		/// @brief Update the species after structural mutation.
		void set_species(Species* const _species);

		/// @brief Get a node pointer from a node ID.
		Node* const get_node(const NodeID& _id);

		///=====================================
		/// Evolution
		///=====================================

		void erase_link();

		/// @brief Parameter mutation.
		void mutate();

		/// @brief Evolve the network based on its relative fitness.
		///
		/// Parameter optimisation (learning phase):
		///
		///		- High fitness: Mutate existing parameters
		///
		///		- Low fitness:
		///			- Low link density: Add link
		///			- High link density: Erase link
		///
		/// Structural changes evolution phase:
		///
		///		- High fitness: Crossover
		///
		///		- Low fitness: Structural mutation
		///			- Low genome complexity: Add nodes
		///			- High genome complexity: Erase nodes
		void evolve();

		/// @brief Crossover procedure between two parent networks.
		/// The end result is the creation of a new phenotype
		/// (this network) by mixing nodes from the two parents.
		void crossover(Net* const _parent1, Net* const _parent2);

		///=====================================
		/// Evaluation
		///=====================================

		/// @brief Evaluate the network.
		/// @todo Import convolution code.
		void evaluate(const Sample& _sample);

		///=====================================
		/// Serialisation and printing
		///=====================================

		/// @brief Serialise the network to JSON.
		/// @todo Implementation.
		json to_json() const;

		/// @brief Output the network to a stream.
		friend os& operator << (os& _os, const Net& _net);

		///=====================================
		/// Friends
		///=====================================

		friend class Env;
		friend class Evaluator;
		friend class Species;
		friend class RField;
		template<ElemType> friend class Element;

	};
}

#endif // CORTEX_NET_HPP
