#ifndef NET_HPP
#define NET_HPP

#include "Species.hpp"
#include "Fitness.hpp"
#include "Node.hpp"
#include "Link.hpp"
#include "Tensor.hpp"

namespace Cortex
{
	class Net
	{

	public:

		uint id;

		uint age;

		SpeciesRef species;

		ConfigRef cfg;

	private:

		// The	containing ecosystem
		EcosystemRef ecosystem;

		// Nodes organised by node role
		emap<NR, hmap<uint, NodePtr>> nodes;

		// Evaluation graph
		std::vector<NodeRef> graph;

		// Fitness statistics
		Fitness fitness;

		// A scheduler which holds information about
		// which nodes should be evaluated at what time.
		Scheduler scheduler;

		// Gaussian receptive fields.
		// @todo Generic RF class
		hmap<uint, hset<uint>> grf;

		friend class Node;

	public:

		Net() = delete;

		Net(const Net& _other) = delete;

		Net(Net&& _other) = delete;

		explicit Net(const uint _id, Ecosystem& _ecosystem, Species& _species);

		~Net();

		void init();

		Node& get_node(const NodeID& _id);

		uint get_node_count() const;

		uint get_node_count(const NR _role) const;

		real get_abs_fitness() const;

		void set_abs_fitness(const real _fitness);

		real get_rel_fitness() const;

		void set_rel_fitness(const real _fitness);

		bool is_old() const;

		void reset_marks();

		real get_progress() const;

		std::vector<real> get_output() const;

		void connect();

		// Topological sorting using depth-first search
		void make_graph(const bool _update = true);

		void mutate();

		// Dispatch function.
		bool mutate(const Mut _mut);

		// Adding a node might result in the creation of a
		// new species if no other species has the same genotype.
		bool add_node(const NR _role = NR::H);

		// Deleting a node might result in the creation of a
		// new species if no other species has the same genotype.
		bool erase_node(const NR _role = NR::H);

		bool add_link();

		bool erase_link();

		bool mutate_param(const Mut _mut);

		// Mating procedure.
		// The species need not match, but this network
		// is *at most* as complex as the partner.
		// cf. Ecosystem::mate()
		void crossover(Net& _p1, Net& _p2);

		void connect(const uint _id);

		void mark_solved();

		// Dispatch function for evaluation of all network types.
		void eval(const std::vector<real>& _input);

		// Spiking nets
		void setup_grf();

		json to_json();

		friend std::ostream& operator<< (std::ostream& _strm, const Net& _net);

	private:

		// Add the input to the network
		void add_input(const std::vector<real>& _input);

		// Generic evaluation function for classical nets.
		void eval_classical();

		// Dispatch function for spiking networks.
		void eval_spiking();

		// Evaluation of spiking net using relative times.
		void eval_time();

		// Evaluation of spiking net using rank order encoding.
		void eval_rank_order();

		// A fully connected network has N(N-1)/2 links,
		// where N is the number of nodes.
		// This function computes the proportion of exisitng links
		// relative to the maximal number of possible links.
		// The result can be used to determine whether we should
		// add another node or we should still try to further
		// optimise the existing connections.
		// Caveat emptor: the result is an overestimate because
		// certain types of links (such as bias-bias) are not allowed.
		// @todo A more accurate method which accounts for disallowed link types.
		inline real saturation();

		uint link_count() const;

		bool insert_node (const NodeID& _id);

		bool insert_node(Node& _other);

		bool activate();
	};
}

#endif // NET_HPP
