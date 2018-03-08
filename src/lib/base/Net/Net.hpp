#ifndef CORTEX_NET_HPP
#define CORTEX_NET_HPP

#include "Fitness.hpp"
#include "RField.hpp"

namespace Cortex
{
	struct EventComp
	{
		bool operator () (const Event& _lhs, const Event& _rhs) const noexcept;
	};

	using Scheduler = std::priority_queue<Event, std::vector<Event>, EventComp>;

	class Net
	{

	public:

		uint id;

		uint age;

		Conf& conf;

	private:

		/// The	containing ecosystem
		Ecosystem& eco;

		/// Note: This is not const because it can
		/// change as the network evolves.
		SpcPtr spc;

		/// Node pointers
		/// @todo Custom topologies.
		emap<NR, std::vector<NodePtr>> nodes;

		/// Evaluation graph
		std::vector<NodePtr> graph;

		/// Receptive field
		RField rfield;

		/// Fitness statistics
		Fitness fitness;

		/// Evaluation scheduler for spiking networks.
		Scheduler scheduler;

		/// @todo Substrates.

	public:

		Net() = delete;

		Net(const Net& _other) = delete;

		Net(Net&& _other) = delete;

		explicit Net(const uint _id, const SpcPtr& _species, Ecosystem& _eco);

		~Net();

		/// @brief Initialise the network by
		/// adding and connecting nodes and ensuring
		/// that forward connections form a DAG.
		void init();

		/// @brief Get the node with ID @p _id.
		/// @param _id Node ID.
		const NodePtr get_node(const NodeID& _id);

		/// @brief Get all nodes in role @p _nr.
		const std::vector<NodePtr>& get_nodes(const NR _nr);

		/// @brief Get a random node in the specified role
		const NodePtr get_rnd_node(const NR _role = NR::Undef);

		/// @return Total number of nodes.
		uint get_node_count() const;

		/// @param _role Node role.
		/// @return Total number of nodes in role @p _role.
		uint get_node_count(const NR _role);

		/// @return Absolute fitness of the network
		real get_fitness() const;

		/// @brief Set the absolute fitness of the network.
		/// This is usually computed by the evaluation function
		/// and depends on the experiment.
		/// @param _fitness Floating-point value.
		void set_fitness(const real _val);

		/// @brief Check if the network has reached its EOL.
		/// @return Boolean value.
		bool is_old() const;

		/// @brief Extract the output of the network.
		/// @return A real-valued vector.
		std::vector<real> get_output();

		/// @brief Ensure that the network is connected.
		void connect();

		/// @brief Disconnect all nodes in the network.
		void disconnect();

		/// @brief Topological sorting using depth-first search.
		/// @param _update Signal whether the evaluation graph
		/// should be updated in the process.
		void make_graph(const bool _update = true);

		/// @brief Apply a random mutation to the network.
		void mutate();

		/// @brief Apply a mutation of type @p _mut to the network.
		/// Mutation can affect a single parameter (such as
		/// a link weight or node time constant), the transfer
		/// function of a node (for classical networks only)
		/// or the whole network (in the case of adding or erasing
		/// a link or a node).
		/// This is a dispatch function.
		/// @param _mut Mutation type.
		/// @return Boolean value indicating success or failure.
		bool mutate(const Mut _mut);

		/// @brief Add a node.
		/// Adding a node might result in the creation of a new
		/// species if no other species has the same genotype.
		/// @param _role Role of the node to add.
		/// @return Boolean value indicating success or failure.
		bool add_node(const NR _role = NR::H, const uint _count = 1);

		/// @brief Erase a random node.
		/// Erasing a node might result in the creation of a new
		/// species if no other species has the same genotype.
		/// @param _role Role of the node to erase.
		/// @return Boolean value indicating success or failure.
		bool erase_node(const NR _role = NR::H);

		/// @brief Add a random link.
		/// @return Boolean value indicating success or failure.
		/// @todo Implement axon guidance.
		bool add_link();

		/// @brief Erase a random link.
		/// @return Boolean value indicating success or failure.
		/// @todo Enable pruning of links with weight close to 0.
		bool erase_link();

		/// @brief Mutate a random parameter of the network.
		/// @param _mut Mutation type.
		/// @return Boolean value indicating success or failure.
		bool mutate_param(const Mut _mut);

		/// @brief Crossover procedure with a member of the same species.
		/// cf. Ecosystem::mate()
		/// @param _p1 Parent 1.
		/// @param _p2 Parent 2.
		void crossover(const NetPtr& _p1, const NetPtr& _p2);

		/// @brief Signal the ecosystem that this net has solved the task.
		void mark_solved();

		/// @brief Serialise the network to JSON.
		/// @return JSON object.
		/// @todo Implementation.
		json to_json();

		/// @brief Dispatch function for evaluation of all network types.
		/// @param _input Real-valued input vector.
		/// For spiking networks, the input is relative spike times
		/// in the case of direct encoding or relative order in case
		/// of rank order encoding.
		void eval(const Sample& _sample);

		/// @brief Dispatch function for evaluation of all network types.
		/// @param _input Real-valued input vector.
		/// For spiking networks, the input is relative spike times
		/// in the case of direct encoding or relative order in case
		/// of rank order encoding.
		void eval(const std::vector<real>& _input);

	private:

		/// @brief Update the species after adding or erasing a node.
		void set_spc(const SpcPtr& _spc);

		/// @brief Helper function for topological sorting.
		void reset_marks();

		/// @brief Saturation of the network in terms of number of links.
		/// A fully connected network has \frac{N(N-1)}{2} links,
		/// where N is the number of nodes.
		/// This function computes the proportion of exisitng links
		/// relative to the maximal number of possible links.
		/// The result can be used to determine whether we should
		/// add another node or we should still try to further
		/// optimise the existing connections.
		/// Caveat emptor: the result is an overestimate because
		/// certain types of links (such as bias-bias) are not allowed.
		/// @todo A more accurate method which accounts for disallowed link types.
		real get_saturation();

		/// @return Total number of links.
		uint get_link_count() const;

		/// @brief Insert a new node with ID @p _id.
		/// @param _id ID of the new node.
		/// @return Boolean value indicating success or failure.
		std::vector<NodePtr> insert_nodes(const NR _nr, const uint _count = 1);

		/// @brief Insert a new node as a copy of another node.
		/// @param _other Reference to the other node.
		/// @return Boolean value indicating success or failure.
		const NodePtr insert_nodes(const NodePtr& _other);

		///=========================================
		/// Network evaluation
		///=========================================

		template<NT nt>
		void eval();

		template<Enc enc>
		void eval();

		///=========================================
		/// Friends
		///=========================================

	public:

		/// @brief Output the network to a stream.
		/// @param _strm Stream to output to.
		/// @param _net Network to output.
		/// @return Stream reference.
		friend std::ostream& operator<< (std::ostream& _strm, const Net& _net);

	private:

		friend class Ecosystem;
		friend class Species;
		friend class Node;
		friend class RField;
	};
}

#endif // CORTEX_NET_HPP
