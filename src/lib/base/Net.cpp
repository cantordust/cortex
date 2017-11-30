#include "Net.hpp"
#include "Ecosystem.hpp"

namespace Cortex
{
	Net::Net(const uint _id,
			 Ecosystem& _ecosystem,
			 Species& _species)
		:
		  id(_id),
		  age(1),
		  ecosystem(_ecosystem),
		  species(_species),
		  cfg(_ecosystem.cfg),
		  fitness(_ecosystem.cfg)
	{
		for (const auto& role : Enum<NR>::entries)
		{
			nodes.emplace(role.first, hmap<uint, NodePtr>());
		}
		_species.add_net(*this);
	}

	Net::~Net()
	{
		for (const auto& nrole : nodes)
		{
			for (const auto& idx : nrole.second)
			{
				idx.second->disconnect();
			}
		}
	}

	void Net::init()
	{
		// Create the phenome by adding
		// nodes to the network
		for (const auto& roles : species.get().get_genome())
		{
			for (uint idx = 0; idx < roles.second; ++idx)
			{
				NodeID node_id({roles.first, idx + 1});
				if (!insert_node(node_id))
				{
					dlog() << "Error inserting node " << node_id;
					exit(EXIT_FAILURE);
				}
			}
		}
		connect();
		make_graph();
	}

	inline Node&Net::get_node(const NodeID& _id)
	{
		return *nodes.at(_id.role).at(_id.idx);
	}

	inline uint Net::get_node_count() const
	{
		return std::accumulate(nodes.begin(),
							   nodes.end(),
							   0,
							   [&](uint _sum, auto& _table) { return _sum + _table.second.size(); });
	}

	inline uint Net::get_node_count(const NR _role) const
	{
		return nodes.at(_role).size();
	}

	inline real Net::get_abs_fitness() const
	{
		return fitness.get_cur();
	}

	inline void Net::set_abs_fitness(const real _fitness)
	{
		// The default is to set the absolute fitness
		fitness.set_cur(_fitness);
		ecosystem.get().inc_evals();
		if (fitness.is_solved())
		{
			mark_solved();
		}
	}

	inline real Net::get_rel_fitness() const
	{
		return fitness.get_rel();
	}

	inline void Net::set_rel_fitness(const real _fitness)
	{
		fitness.set_rel(_fitness);
	}

	inline bool Net::is_old() const
	{
		return (cfg.get().net.max.age > 0 && age > cfg.get().net.max.age);
	}

	inline void Net::reset_marks()
	{
		for (const auto& nrole : nodes)
		{
			for (const auto& node : nrole.second)
			{
				node.second->mark = NodeMark::Undef;
			}
		}
	}

	inline real Net::get_progress() const
	{
		return fitness.progress();
	}

	inline std::vector<real> Net::get_output() const
	{
		std::vector<real> output;

		for (uint out = 1; out <= nodes.at(NR::O).size(); ++out)
		{
			output.emplace_back(nodes.at(NR::O).at(out)->output);
		}

		return output;
	}

	void Net::mutate()
	{
		// Determine the type of mutation to perform.
		auto mut_prob(cfg.get().mutation.prob);

		// The next procedure tries to make sure that
		// we are not going to end up with an ecosystem
		// full of bloated individuals.
		// A high saturation indicates that we have
		//	properly explored the potential for adding connections
		//	before resorting to the addition of a new hidden node.
		if (cfg.get().mutation.adaptive)
		{
			real sat( saturation() );
			// Low saturation => low probability
			// of adding or erasing a node or
			// erasing a link
			mut_prob.at(Mut::AddNode) *= sat;
			mut_prob.at(Mut::EraseNode) *= sat;
			mut_prob.at(Mut::EraseLink) *= sat;

			// Low saturation => high probability of adding a link
			mut_prob.at(Mut::AddLink) *= (1.0 - sat);

		}

		while (!mutate(cfg.get().w_dist(mut_prob))) {};
	}

	inline bool Net::mutate(const Mut _mut)
	{
//		dlog() << "Mutating network " << id << ": " << _mut;
		switch (_mut)
		{
		case Mut::Weight:
		case Mut::Tau:
		case Mut::Fn:
			return mutate_param(_mut);

		case Mut::AddLink:
			return add_link();

		case Mut::EraseLink:
			return erase_link();

		case Mut::AddNode:
			return add_node();

		case Mut::EraseNode:
			return erase_node();

		default:
			return false;
		}
	}

	bool Net::add_node(const NR _role)
	{
		// Add a node and connect it to the network.
		Genotype genotype(species.get().get_genotype());
		if (genotype.add(_role, 1))
		{
			uint spc_id(ecosystem.get().get_species_id(genotype));
			if (spc_id == 0)
			{
				return false;
			}

			// Insert a node
			NodeID node_id({_role, get_node_count(_role) + 1});
			if (!insert_node(node_id))
			{
				return false;
			}

			// Clear the evaluation graph
			graph.clear();

			// Connect the node to the network
			nodes.at(node_id.role).at(node_id.idx)->connect();

			// Update the graph
			make_graph();

			// Unregister the network from the old species
			species.get().erase_net(id);

			// Update the species reference
			species = ecosystem.get().get_species(spc_id);

			// Register the network with the new species
			species.get().add_net(*this);

			return true;
		}

		return false;
	}

	bool Net::erase_node(const NR _role)
	{
		// Add a node and connect it to the network.
		Genotype genotype(species.get().get_genotype());
		if (genotype.erase(_role, 1))
		{
			uint spc_id(ecosystem.get().get_species_id(genotype));
			if (get_node_count(_role) == 0 ||
				spc_id == 0)
			{
				return false;
			}

//			dlog() << "Before erase_node():\n" << *this;

			NodeID node_id({_role, cfg.get().rnd_key(nodes.at(_role))});
//			dlog() << "\tErasing node " << node_id;

			// Erase the node
			if (nodes.at(_role).erase(node_id.idx) == 0)
			{
				return false;
			}

			// Clear the evaluation graph
			graph.clear();

			// Iterate over nodes in the same role
			// and decrement NodeID.idx by 1 if
			// it is greater than the one deleted.
			// After that, move the node to the new
			// index in the lookup table.
			hmap<uint, NodePtr> tmp_map;
			for (auto it = nodes.at(node_id.role).begin(); it != nodes.at(node_id.role).end(); )
			{
				if (it->first > node_id.idx)
				{
					--(it->second->id.idx);
					tmp_map.emplace(it->first - 1, std::move(it->second));
					it = nodes.at(_role).erase(it);
				}
				else
				{
					++it;
				}
			}

			// Move nodes back into the node lookup table
			for (auto&& node : tmp_map)
			{
				nodes.at(node_id.role).emplace(node.first, std::move(node.second));
			}

			// Reindex links
			for (const auto& nrole : nodes)
			{
				for (const auto& idx : nrole.second)
				{
					idx.second->reindex(node_id);
				}
			}

//			dlog() << "After erase_node():" << *this;

			// Make sure that the network is in a valid state
			connect();

			// Regenerate the evaluation graph
			make_graph();

			// Unregister the network from the old species
			species.get().erase_net(id);

			// Update the species ID
			species = ecosystem.get().get_species(spc_id);

			// Register the network with the new species.
			species.get().add_net(*this);

			return true;
		}

		return false;
	}

	bool Net::add_link()
	{
		uint attempts(cfg.get().mutation.attempts);

		while (attempts-- > 0)
		{
			LT lt(cfg.get().w_dist(cfg.get().link.type));
			if (lt == LT::R &&
				!cfg.get().link.rec)
			{
				lt = LT::F;
			}

			std::map<std::pair<NR, NR>, real> role_map;
			for (auto&& srole : cfg.get().link.prob.at(lt))
			{
				for (auto&& trole : srole.second)
				{
					role_map[{srole.first, trole.first}] = trole.second;
				}
			}

			// Pick the source and target roles
			std::pair<NR, NR> roles;
			do
			{
				roles = cfg.get().w_dist(role_map);
			} while (get_node_count(roles.first) == 0 || get_node_count(roles.second) == 0);

			NodeID src_id({roles.first, cfg.get().rnd_key(nodes.at(roles.first))});
			NodeID tgt_id({roles.second, cfg.get().rnd_key(nodes.at(roles.second))});

			Node& src(*nodes.at(src_id.role).at(src_id.idx));
			Node& tgt(*nodes.at(tgt_id.role).at(tgt_id.idx));

			// Pick the source and target nodes.
			if (src.is_link_allowed(lt, tgt))
			{
				src.add_link(lt, tgt);
				make_graph();
				return true;
			}
		}
		return false;
	}

	bool Net::erase_link()
	{
		uint attempts(cfg.get().mutation.attempts);

		while (attempts-- > 0)
		{
			LT lt(cfg.get().w_dist(cfg.get().link.type));
			if (lt == LT::R &&
				!cfg.get().link.rec)
			{
				lt = LT::F;
			}

			std::map<std::pair<NR, NR>, real> role_map;
			for (const auto& srole : cfg.get().link.prob.at(lt))
			{
				for (const auto& trole : srole.second)
				{
					role_map[{srole.first, trole.first}] = trole.second;
				}
			}

			// Pick the source and target roles
			std::pair<NR, NR> roles;
			do
			{
				roles = cfg.get().w_dist(role_map);
			} while (get_node_count(roles.first) == 0 || get_node_count(roles.second) == 0);

			NodeID src_id({roles.first, cfg.get().rnd_key(nodes.at(roles.first))});
			NodeID tgt_id({roles.second, cfg.get().rnd_key(nodes.at(roles.second))});

			Node& src(*nodes.at(src_id.role).at(src_id.idx));
			Node& tgt(*nodes.at(tgt_id.role).at(tgt_id.idx));

			// Pick the source and target nodes.
			if (src.is_link_eraseable(lt, tgt))
			{
				src.erase_link(lt, tgt.id);
				make_graph();
				return true;
			}
		}
		return false;
	}

	bool Net::mutate_param(const Mut _mut)
	{
		NR role(NR::Undef);
		do
		{
			role = cfg.get().w_dist(cfg.get().mutation.node);
		} while (get_node_count(role) == 0);

		//				dlog() << "\tRole: " << role;

		return nodes.at(role).at(cfg.get().rnd_key(nodes.at(role)))->mutate(_mut);
	}

	inline void Net::connect()
	{
//		dlog() << "Network " << id << ": connecting...";
		for (const auto& nrole : nodes)
		{
			for (const auto& node : nrole.second)
			{
				node.second->connect();
			}
		}
	}

	void Net::make_graph(const bool _update)
	{
//		dlog() << "Network " << id << ": creating graph...";

		if (_update)
		{
			graph.clear();
		}

		reset_marks();

		for (const auto& nrole : nodes)
		{
			for (const auto& node : nrole.second)
			{
				if (node.second->mark == NodeMark::Undef &&
					!node.second->visit(_update))
				{
					dlog() << "Net::make_graph(): Failed to make graph for network "
						   << id << ", exiting...\n" << *this;
					exit(EXIT_FAILURE);
				}
			}
		}

		// The graph is constructed backwards,
		// so we need to reverse it.
		if (_update &&
			!graph.empty())
		{
			std::reverse(graph.begin(), graph.end());
		}
	}

	void Net::mark_solved()
	{
		ecosystem.get().mark_solved(id);
	}

	void Net::eval(const std::vector<real>& _input)
	{
		add_input(_input);

		switch (cfg.get().net.type)
		{
		case NT::Classical:
			eval_classical();
			break;

		case NT::Spiking:
			eval_spiking();
			break;

		default:
			dlog() << "Invalid network type " << to_str<NT>(cfg.get().net.type);
			exit(EXIT_FAILURE);
		}
	}

	void Net::crossover(Net& _p1, Net& _p2)
	{
		// Relative fitness scores of the two parents.
		hmap<uint, real> fdist;
		fdist.emplace(_p1.id, _p1.get_abs_fitness());
		fdist.emplace(_p2.id, _p2.get_abs_fitness());

		// Create the nodes
		for (const auto& nrole : _p1.nodes)
		{
			for (const auto& idx : nrole.second)
			{
				if (!insert_node(cfg.get().w_dist(fdist) == _p1.id ? *idx.second : _p2.get_node(idx.second->id)))
				{
					dlog() << "Net::crossover(): node replication failed!";
					exit(EXIT_FAILURE);
				}
			}
		}

		// Populate the network with links
		for (const auto& nrole : nodes)
		{
			for (const auto& idx : nrole.second)
			{
				idx.second->crossover(_p1.get_node(idx.second->id), _p2.get_node(idx.second->id), fdist);
			}
		}

		connect();
		make_graph();
	}

	void Net::setup_grf()
	{
		for (uint var = 0; var < cfg.get().node.roles.at(NR::I); ++var)
		{
			for (uint idx = 0; idx < cfg.get().net.spiking.receptors; ++idx)
			{
				nodes.at(NR::I).at(idx)->set_grf(idx);
			}
		}
	}

	bool Net::activate()
	{
//		nodes_old.at(_e.second.second).eval(_e.first, links.at(_e.second.first).at(_e.second.second).get(Mut::Weight));
//		if (nodes_old.at(_e.second.second).get_output() > 0.0)
//		{
//			// Schedule the targets *of the target* for evaluation.
//			for (const uint tgt : phenome.get(LT::F, _e.second.second))
//			{
//				scheduler.emplace( event_pair(_e.first + links.at(_e.second.second).at(tgt).get(Mut::Delay), {_e.second.second, tgt}) );
//			}

//			if (cfg.get().link.rec.enabled)
//			{
//				for (const uint tgt : phenome.get(LT::R, _e.second.second))
//				{
//					scheduler.emplace( event_pair(_e.first + links.at(_e.second.second).at(tgt).get(Mut::Delay), {_e.second.second, tgt}) );
//				}
//			}


//			// Multiplicative Hebbian plasticity.
//			// Adapted from Rubin, Lee & Sompolinsky (2000),
//			// Equilibrium Properties Of Temporally Asymmetric Hebbian Plasticity
//			if (cfg.get().stdp.enabled)
//			{
//				stdp_pre_spike();
//			}
//		}
//		else if (cfg.get().stdp.enabled)
//		{
//			stdp_post_spike();
//		}

//		return nodes_old.at(_e.second.second).get_output() > 0.0;
		return true;
	}

	inline void Net::eval_classical(const std::vector<real>& _input)
	{
		// Present the input to the network
		uint idx(0);
		for (const auto& node : nodes.at(NR::I))
		{
			node.second->add_input(_input.at(idx));
			++idx;
		}

		// Propagate
		for (auto&& node : graph)
		{
			node.get().propagate();
		}
	}

	inline void Net::eval_spiking(const std::vector<real>& _input)
	{
//		reset();

//		add_input(_input);

		switch (cfg.get().net.spiking.enc)
		{
		case Enc::Time:
			return eval_time();

		case Enc::RankOrder:
			return eval_rank_order();

		default:
			dlog() << "Invalid encoding type " << cfg.get().net.spiking.enc;
			exit(EXIT_FAILURE);
		}
	}

	inline void Net::eval_time()
	{
		std::queue<Event> spikes;

		while (!scheduler.empty())
		{
			// Event pair: spike time | source | target
			if ( activate(scheduler.top()) &&
				 is_output(scheduler.top().second.second) )
			{
				spikes.emplace( event(scheduler.top().first, scheduler.top().second.second) );
			}

			scheduler.pop();
		}
	}

	inline void Net::eval_rank_order()
	{
		eval_time();
	}

	inline real Net::saturation()
	{
		return (2.0 * link_count()) / (std::pow(get_node_count(), 2) - get_node_count());
	}

	inline uint Net::link_count() const
	{
		// Sum links
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

	inline bool Net::insert_node(const NodeID& _id)
	{
		auto success(nodes.at(_id.role).emplace(_id.idx,
												std::make_unique<Node>(_id, *this)));
		return success.second;
	}

	inline bool Net::insert_node(Node& _other)
	{
		auto success(nodes.at(_other.id.role).emplace(_other.id.idx,
													  std::make_unique<Node>(_other, *this)));

		return success.second;
	}

	json Net::to_json()
	{
		json j;
		// @todo populate json

		return j;
	}

	inline void Net::add_input(const std::vector<real>& _input)
	{

	}

	std::ostream& operator<< (std::ostream& _strm, const Net& _net)
	{
		_strm << "\n---------------- Network ----------------\n";

		_strm << "ID: " << _net.id << "\n"
			  << "Age: " << _net.age << "\n"
			  << "Species: " << _net.species.get().id << "\n"
			  << "Absolute fitness: " << _net.get_abs_fitness() << "\n"
			  << "Relative fitness: " << _net.get_rel_fitness() << "\n";

		_strm << "\n\nEvaluation order: ";

		for (const auto& node : _net.graph)
		{
			_strm << node.get().id << " ";
		}

		_strm << "\nForward:\n";

		for (const auto& nrole : _net.nodes)
		{
			for (const auto& idx : nrole.second)
			{
				if (idx.second->has_targets(LT::F))
				{
					for (const auto& flinks : idx.second->links.targets.at(LT::F))
					{
						for (const auto& tgt_role : flinks.second)
						{
							_strm << "\t  ,-> " << tgt_role.second->tgt.id
								  << " (" << tgt_role.second->weight.cur() << ")\n";
						}

					}
					_strm << "\t  ^\n";
				}

				_strm << "\t" << *idx.second
					  << " (" << idx.second->af.get_fn() << ")\n\n";
			}
		}

		_strm << "\nRecurrent:\n";

		for (const auto& nrole : _net.nodes)
		{
			for (const auto& idx : nrole.second)
			{
				if (idx.second->has_targets(LT::R))
				{
					for (const auto& flinks : idx.second->links.targets.at(LT::R))
					{
						for (const auto& tgt_role : flinks.second)
						{
							_strm << "\t  ,-> " << tgt_role.second->tgt.id
								  << " (" << tgt_role.second->weight.cur() << ")\n";
						}

					}
					_strm << "\t  ^\n";
				}

				_strm << "\t" << *idx.second
					  << " (" << idx.second->af.get_fn() << ")\n\n";
			}
		}

		_strm << "\nTotal nodes: " << _net.get_node_count()
			  << "\n--------------------------------------\n\n";

		return _strm;
	}
}
