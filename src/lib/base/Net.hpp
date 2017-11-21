#ifndef NET_HPP
#define NET_HPP

#include "Species.hpp"
#include "Fitness.hpp"
//#include "Phenome.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	class Net
	{

	public:

		uint id;

		uint age;

		SpeciesRef species;

		Config& cfg;

	private:

		/// The	containing ecosystem
		EcosystemRef ecosystem;

		/// Nodes organised by node role
		emap<NR, hmap<uint, NodePtr>> nodes;

		/// Evaluation graph
		std::vector<NodeRef> graph;

		/// Fitness statistics
		Fitness fitness;

		/// A scheduler which holds information about
		/// which nodes should be evaluated at what time.
		/// The entries are events representing a timestamp
		/// and the pair of node IDs which have to be evaluated.
		Scheduler scheduler;

		/// Gaussian receptive fields.
		/// \todo Generic RF class
		hmap<uint, hset<uint>> grf;

		inline void eval_classical(const std::vector<real>& _input)
		{
			/// Present the input to the network
			uint idx(0);
			for (const auto& node : nodes.at(NR::I))
			{
				node.second->add_input(_input.at(idx));
				++idx;
			}

			/// Propagate
			for (auto& node : graph)
			{
				node.get().propagate();
			}
		}

		std::queue<event> eval_spiking(const std::vector<real>& _input);

		/// Evaluation using exact latencies.
		std::queue<event> eval_latency();

		/// Input: a vector of real values.
		/// Evaluated using receptive fields.
		std::queue<event> eval_rank_order()
		{
			return eval_latency();
		}

		void stdp();

		/// A fully connected network has N(N-1)/2 links,
		/// where N is the number of nodes.
		/// This function computes the proportion of exisitng links
		/// relative to the maximal number of possible links.
		/// The result can be used to determine whether we should
		/// add another node or we should still try to further
		/// optimise the existing connections.
		/// Caveat emptor: the result is an overestimate because
		/// certain types of links (such as bias-bias) are not allowed.
		/// \todo A more accurate method which accounts for disallowed link types.
		inline real saturation()
		{
			return (2.0 * link_count()) / (std::pow(node_count(), 2) - node_count());
		}

		inline uint link_count() const
		{
			/// Sum links
			return std::accumulate(nodes.begin(),
								   nodes.end(),
								   0,
								   [&](uint _sum, const auto& _role)
			{
				return _sum + std::accumulate(_role.second.begin(),
											  _role.second.end(),
											  0,
											  [&](uint _link_sum, const auto& _node)
				{
					return _link_sum + _node.second->link_count();
				});
			});
		}

		inline bool insert_node (const NodeID& _id)
		{
			auto success(nodes.at(_id.role).emplace(_id.idx,
													std::make_unique<Node>(_id, *this)));
			return success.second;
		}

		inline bool insert_node(Node& _other)
		{
			auto success(nodes.at(_other.id.role).emplace(_other.id.idx,
														  std::make_unique<Node>(_other, *this)));

			return success.second;
		}

		friend class Node;

	public:

		Net() = delete;

		Net(const Net& _other) = delete;

		Net(Net&& _other) = delete;

		explicit Net(const uint _id, Ecosystem& _ecosystem, Species& _species);

		~Net();

		void init();

		inline Node& get_node(const NodeID& _id)
		{
			return *nodes.at(_id.role).at(_id.idx);
		}

		inline uint node_count() const
		{
			return std::accumulate(nodes.begin(),
								   nodes.end(),
								   0,
								   [&](uint _sum, auto& _table) { return _sum + _table.second.size(); });
		}

		inline uint node_count(const NR _role) const
		{
			return nodes.at(_role).size();
		}

		inline const real get_abs_fitness() const
		{
			return fitness.get_abs();
		}

		void set_abs_fitness(const real _fitness);

		inline const real get_rel_fitness() const
		{
			return fitness.get_rel();
		}

		inline void set_rel_fitness(const real _fitness)
		{
			fitness.set_rel(_fitness);
		}

		inline bool is_old() const
		{
			return (cfg.net.max.age > 0 && age > cfg.net.max.age);
		}

		inline void reset_marks()
		{
			for (const auto& nrole : nodes)
			{
				for (const auto& node : nrole.second)
				{
					node.second->mark = Mark::None;
				}
			}
		}

		inline real progress() const
		{
			return fitness.progress();
		}

		void connect();

		/// Topological sorting using depth-first search
		void make_graph(const bool _update = true);

		/// Adding a node might result in the creation of a
		/// new species if no other species has the same genotype.
		bool add_node(const NR _role = NR::H);

		/// Deleting a node might result in the creation of a
		/// new species if no other species has the same genotype.
		bool erase_node(const NR _role = NR::H);

		bool add_link();

		bool erase_link();

		void mutate();

		bool mutate(const Mut _mut);

		/// Mating procedure.
		/// The species need not match, but this network
		/// is *at most* as complex as the partner.
		/// cf. Ecosystem::mate()
		void crossover(Net& _p1, Net& _p2);

		void connect(const uint _id);

		void mark_solved();

		/////////// Classical nets

		void eval(const std::vector<real>& _input);

		inline std::vector<real> get_output() const
		{
			std::vector<real> output;

			for (uint out = 1; out <= nodes.at(NR::O).size(); ++out)
			{
				output.emplace_back(nodes.at(NR::O).at(out)->output);
			}

			return output;
		}

		////// Spiking nets

		void setup_grf(const std::vector<std::pair<real, real>>& _var_ranges);

		bool activate(const event_pair& _e);

		void add_input(const std::vector<real>& _input);

		/// \brief Hebbian learning for links corresponding to
		/// spikes preceding the postsynaptic spike
		void stdp_pre_spike();

		/// \brief Hebbian learning for links corresponding to
		/// spikes following the postsynaptic spike
		void stdp_post_spike();

		inline json as_json()
		{
			json j;
			return j;
		}

		friend std::ostream& operator<< (std::ostream& _strm, const Net& _net);

	};
}

#endif // NET_HPP
