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
		/// Create the phenome by adding
		/// nodes to the network
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

	void Net::set_abs_fitness(const real _fitness)
	{
		/// The default is to set the absolute fitness
		fitness.set_abs(_fitness);
		ecosystem.get().inc_evals();
		if (fitness.is_solved())
		{
			mark_solved();
		}
	}

	bool Net::add_node(const NR _role)
	{
		/// Add a node and connect it to the network.
		Genotype genotype(species.get().get_genotype());
		if (genotype.add(_role, 1))
		{
			uint spc_id(ecosystem.get().get_species_id(genotype));
			if (spc_id == 0)
			{
				return false;
			}

			/// Insert a node
			NodeID node_id({_role, node_count(_role) + 1});
			if (!insert_node(node_id))
			{
				return false;
			}

			/// Clear the evaluation graph
			graph.clear();

			/// Connect the node to the network
			nodes.at(node_id.role).at(node_id.idx)->connect();

			/// Update the graph
			make_graph();

			/// Unregister the network from the old species
			species.get().erase_net(id);

			/// Update the species reference
			species = ecosystem.get().get_species(spc_id);

			/// Register the network with the new species
			species.get().add_net(*this);

			return true;
		}

		return false;
	}

	bool Net::erase_node(const NR _role)
	{
		/// Add a node and connect it to the network.
		Genotype genotype(species.get().get_genotype());
		if (genotype.erase(_role, 1))
		{
			uint spc_id(ecosystem.get().get_species_id(genotype));
			if (node_count(_role) == 0 ||
				spc_id == 0)
			{
				return false;
			}

//			dlog() << "Before erase_node():\n" << *this;

			NodeID node_id({_role, cfg.rnd_key(nodes.at(_role))});
//			dlog() << "\tErasing node " << node_id;

			/// Erase the node
			if (nodes.at(_role).erase(node_id.idx) == 0)
			{
				return false;
			}

			/// Clear the evaluation graph
			graph.clear();

			/// Iterate over nodes in the same role
			/// and decrement NodeID.idx by 1 if
			/// it is greater than the one deleted.
			/// After that, move the node to the new
			/// index in the lookup table.
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

			/// Move nodes back into the node lookup table
			for (auto& node : tmp_map)
			{
				nodes.at(node_id.role).emplace(node.first, std::move(node.second));
			}

			/// Reindex links
			for (const auto& nrole : nodes)
			{
				for (const auto& idx : nrole.second)
				{
					idx.second->reindex(node_id);
				}
			}

//			dlog() << "After erase_node():" << *this;

			/// Make sure that the network is in a valid state
			connect();

			/// Regenerate the evaluation graph
			make_graph();

			/// Unregister the network from the old species
			species.get().erase_net(id);

			/// Update the species ID
			species = ecosystem.get().get_species(spc_id);

			/// Register the network with the new species
			species.get().add_net(*this);

			return true;
		}

		return false;
	}

	bool Net::add_link()
	{
		uint attempts(cfg.mutation.attempts);

		while (attempts-- > 0)
		{
			LT lt(cfg.w_dist(cfg.link.type));
			if (lt == LT::R &&
				!cfg.link.rec)
			{
				lt = LT::F;
			}

			std::map<std::pair<NR, NR>, real> role_map;
			for (const auto& srole : cfg.link.prob.at(lt))
			{
				for (const auto& trole : srole.second)
				{
					role_map[{srole.first, trole.first}] = trole.second;
				}
			}

			/// Pick the source and target roles
			std::pair<NR, NR> roles;
			do
			{
				roles = cfg.w_dist(role_map);
			} while (node_count(roles.first) == 0 || node_count(roles.second) == 0);

			NodeID src_id({roles.first, cfg.rnd_key(nodes.at(roles.first))});
			NodeID tgt_id({roles.second, cfg.rnd_key(nodes.at(roles.second))});

			Node& src(*nodes.at(src_id.role).at(src_id.idx));
			Node& tgt(*nodes.at(tgt_id.role).at(tgt_id.idx));

			/// Pick the source and target nodes.
			if (src.link_allowed(lt, tgt))
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
		uint attempts(cfg.mutation.attempts);

		while (attempts-- > 0)
		{
			LT lt(cfg.w_dist(cfg.link.type));
			if (lt == LT::R &&
				!cfg.link.rec)
			{
				lt = LT::F;
			}

			std::map<std::pair<NR, NR>, real> role_map;
			for (const auto& srole : cfg.link.prob.at(lt))
			{
				for (const auto& trole : srole.second)
				{
					role_map[{srole.first, trole.first}] = trole.second;
				}
			}

			/// Pick the source and target roles
			std::pair<NR, NR> roles;
			do
			{
				roles = cfg.w_dist(role_map);
			} while (node_count(roles.first) == 0 || node_count(roles.second) == 0);

			NodeID src_id({roles.first, cfg.rnd_key(nodes.at(roles.first))});
			NodeID tgt_id({roles.second, cfg.rnd_key(nodes.at(roles.second))});

			Node& src(*nodes.at(src_id.role).at(src_id.idx));
			Node& tgt(*nodes.at(tgt_id.role).at(tgt_id.idx));

			/// Pick the source and target nodes.
			if (src.link_eraseable(lt, tgt))
			{
				src.erase_link(lt, tgt.id);
				make_graph();
				return true;
			}
		}
		return false;
	}

	void Net::connect()
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
				if (node.second->mark == Mark::None &&
					!node.second->visit(_update))
				{
					dlog() << "Net::make_graph(): Failed to make graph for network "
						   << id << ", exiting...\n" << *this;
					exit(EXIT_FAILURE);
				}
			}
		}

		/// The graph is constructed backwards,
		/// so we need to reverse it.
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
		switch (cfg.net.type)
		{
		case NT::Classical:
			eval_classical(_input);
			break;

		case NT::Spiking:
			eval_spiking(_input);
			break;

		default:
			dlog() << "Invalid network type " << as_str<NT>(cfg.net.type);
			exit(EXIT_FAILURE);
		}
	}

	void Net::mutate()
	{
		/// Determine the type of mutation to perform.
		auto mut_prob(cfg.mutation.prob);

		/// The next procedure tries to make sure that
		/// we are not going to end up with an ecosystem
		/// full of bloated individuals.
		/// A high saturation indicates that we have
		///	properly explored the potential for adding connections
		///	before resorting to the addition of a new hidden node.
		if (cfg.mutation.adaptive)
		{
			real sat( saturation() );
			/// Low saturation => low probability
			/// of adding or erasing a node or
			/// erasing a link
			mut_prob.at(Mut::AddNode) *= sat;
			mut_prob.at(Mut::EraseNode) *= sat;
			mut_prob.at(Mut::EraseLink) *= sat;

			/// Low saturation => high probability of adding a link
			mut_prob.at(Mut::AddLink) *= (1.0 - sat);

		}

		while (!mutate(cfg.w_dist(mut_prob))) {};
	}

	bool Net::mutate(const Mut _mut)
	{
//		dlog() << "Mutating network " << id << ": " << _mut;
		switch (_mut)
		{
		case Mut::Weight:
		case Mut::Tau:
		case Mut::Fn:
			{
				NR role(NR::Undef);
				do
				{
					role = cfg.w_dist(cfg.mutation.node);
				} while (node_count(role) == 0);

//				dlog() << "\tRole: " << role;

				return nodes.at(role).at(cfg.rnd_key(nodes.at(role)))->mutate(_mut);
			}

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

	void Net::crossover(Net& _p1, Net& _p2)
	{
		/// Relative fitness scores of the two parents.
		hmap<uint, real> fdist;
		fdist.emplace(_p1.id, _p1.get_abs_fitness());
		fdist.emplace(_p2.id, _p2.get_abs_fitness());

//		fdist.emplace(_p1.id, 0.5);
//		fdist.emplace(_p2.id, 0.5);

//		dlog() << "\nMating networks " << _p1.id << " and " << _p2.id << ":\n"
//			   << _p1 << "\n\n" << _p2 << "\n"
//			   << "Replicating nodes...";

		/// Create the nodes
		for (const auto& nrole : _p1.nodes)
		{
			for (const auto& idx : nrole.second)
			{
				if (!insert_node(cfg.w_dist(fdist) == _p1.id ? *idx.second : _p2.get_node(idx.second->id)))
				{
					dlog() << "Net::crossover(): node replication failed!";
					exit(EXIT_FAILURE);
				}
			}
		}

//		dlog() << "Performing crossover...";

		/// Populate the network with links
		for (const auto& nrole : nodes)
		{
			for (const auto& idx : nrole.second)
			{
				idx.second->crossover(_p1.get_node(idx.second->id), _p2.get_node(idx.second->id), fdist);
			}
		}

		connect();
		make_graph();

//		dlog() << "Offspring:\n" << *this << "\n";
	}

	////// Classical nets

	/// Experimental!
	void Net::stdp()
	{
//		for (uint tgt = 1; tgt <= nodes_old.size(); ++tgt)
//		{
//			if (!phenome.is_empty(LT::S, tgt))
//			{
//				/// The node has sources (it is not an input or a bias node)
//				for (const uint src : phenome.get(LT::S, tgt))
//				{
//					/// Synaptic plasticity.
//					/// This should be self-regulating
//					/// and should not lead to runaway excitation.
//					real new_w( link(src, tgt).get(Mut::Weight) );
//					new_w += cfg.stdp.lr * nodes_old.at(tgt).get_output() * ( nodes_old.at(src).get_output() - new_w * nodes_old.at(tgt).get_output() );

//					if (new_w > cfg.link.params[Mut::Weight].max)
//					{
//						new_w = cfg.link.params[Mut::Weight].max;
//					}

//					link(src, tgt).set(Mut::Weight, new_w);
//				}
//			}
//		}

//		if (cfg.link.rec.enabled)
//		{
//			for (uint src = 1; src <= nodes_old.size(); ++src)
//			{
//				if (!phenome.is_empty(LT::R, src))
//				{
//					for (const uint tgt : phenome.get(LT::R, src))
//					{
//						real new_w( link(src, tgt).get(Mut::Weight) );
//						new_w += cfg.stdp.lr * nodes_old.at(tgt).get_output() * ( nodes_old.at(src).get_output() - new_w * nodes_old.at(tgt).get_output() );

//						if (new_w > cfg.link.params[Mut::Weight].max)
//						{
//							new_w = cfg.link.params[Mut::Weight].max;
//						}

//						link(src, tgt).set(Mut::Weight, new_w);
//					}
//				}
//			}
//		}
	}

	////// Spiking nets

	void Net::stdp_post_spike()
	{
//		real cur_time(scheduler.top().first);
//		uint src(scheduler.top().second.first);
//		uint tgt(scheduler.top().second.second);

//		real tgt_last_spike_time( nodes_old.at(tgt).get_last_spike_time() );

//		if (tgt_last_spike_time > 0)
//		{
//			real w(link(src, tgt).get(Mut::Weight));
//			real decay( std::exp( (tgt_last_spike_time - cur_time) / cfg.stdp.span ) );

//			/// \todo Make the decay cutoff a parameter
//			if (decay > 0.05)
//			{
//				switch (cfg.stdp.type)
//				{
//				case STDP::Heb:
//					{
//						real dw(cfg.stdp.lr * decay * cfg.stdp.alpha * std::abs(w));

//						if (w >= 0)
//						{
//							link(src, tgt).set(Mut::Weight, w - dw);
//						}
//						else
//						{
//							link(src, tgt).set(Mut::Weight, w + dw);
//						}

//						break;
//					}

//				case STDP::AntiHeb:
//					{
//						real dw(cfg.stdp.lr * decay * (cfg.link.params[Mut::Weight].max - std::abs(w)));

//						if (w >= 0)
//						{
//							link(src, tgt).set(Mut::Weight, std::abs(w) + dw);
//						}
//						else
//						{
//							link(src, tgt).set(Mut::Weight, std::abs(w) - dw);
//						}

//						break;
//					}

//				default:
//					break;
//				}
//			}
//		}
	}

	void Net::stdp_pre_spike()
	{
//		real cur_time(scheduler.top().first);
//		uint tgt(scheduler.top().second.second);
//		std::priority_queue<event, std::vector<event>> spike_times;

//		for (const uint src : phenome.get(LT::S, tgt))
//		{
//			if (nodes_old.at(src).get_role() != NR::B)
//			{
//				real src_last_spike_time( nodes_old.at(src).get_last_spike_time() );

//				if (src_last_spike_time > 0.0 &&
//					src_last_spike_time + link(src, tgt).get(Mut::Delay) <= cur_time)
//				{
//					spike_times.emplace(src_last_spike_time, src);
//				}
//			}
//		}

//		while (!spike_times.empty())
//		{
//			real spike_time(spike_times.top().first);
//			spike_times.pop();

//			uint src(spike_times.top().second);
//			real w( std::fabs( link(src, tgt).get(Mut::Weight) ) );
//			real decay( std::exp( (spike_time - cur_time) / cfg.stdp.span ) );

//			//							std::cout << "Afferent spike time: " << aff_spike_time << ", hebbian learning for link " << afferent_spikes.top().second << "->" << target_id << std::endl;
//			//						std::cout << "(s) diff: " << aff_spike_time - current_time << ", decay: " << decay << std::endl;
//			//						std::cin.get();
//			if (decay > 0.05)
//			{
//				switch (cfg.stdp.type)
//				{
//				case STDP::Heb:
//					link(src, tgt).set(Mut::Weight, w + cfg.stdp.lr * (cfg.link.params[Mut::Weight].max - w) * decay);
//					break;

//				case STDP::AntiHeb:
//					link(src, tgt).set(Mut::Weight, w - cfg.stdp.lr * cfg.stdp.alpha * w * decay);
//					break;

//				default:
//					break;
//				}
//			}

//		}
	}

	void Net::setup_grf(const std::vector<std::pair<real, real> >& _var_ranges)
	{
//		uint N( node_count(NR::I) / _var_ranges.size() );
//		uint id( species.get().get_ids(NR::I).front() );

//		for (uint var = 0; var < _var_ranges.size(); ++var)
//		{
//			for (uint i = 0; i < N; ++i)
//			{
//				nodes_old.at(id).set_grf(N,
//										 i,
//										 cfg.net.spiking.beta,
//										 _var_ranges[var].first,
//										 _var_ranges[var].second);
//				grf[var].insert(id);
//				++id;
//			}
//		}
	}

	bool Net::activate(const event_pair& _e)
	{
//		nodes_old.at(_e.second.second).eval(_e.first, links.at(_e.second.first).at(_e.second.second).get(Mut::Weight));
//		if (nodes_old.at(_e.second.second).get_output() > 0.0)
//		{
//			/// Schedule the targets *of the target* for evaluation.
//			for (const uint tgt : phenome.get(LT::F, _e.second.second))
//			{
//				scheduler.emplace( event_pair(_e.first + links.at(_e.second.second).at(tgt).get(Mut::Delay), {_e.second.second, tgt}) );
//			}

//			if (cfg.link.rec.enabled)
//			{
//				for (const uint tgt : phenome.get(LT::R, _e.second.second))
//				{
//					scheduler.emplace( event_pair(_e.first + links.at(_e.second.second).at(tgt).get(Mut::Delay), {_e.second.second, tgt}) );
//				}
//			}


//			/// Multiplicative Hebbian plasticity.
//			/// Adapted from Rubin, Lee & Sompolinsky (2000),
//			/// Equilibrium Properties Of Temporally Asymmetric Hebbian Plasticity
//			if (cfg.stdp.enabled)
//			{
//				stdp_pre_spike();
//			}
//		}
//		else if (cfg.stdp.enabled)
//		{
//			stdp_post_spike();
//		}

//		return nodes_old.at(_e.second.second).get_output() > 0.0;
		return true;
	}

	void Net::add_input(const std::vector<real>& _input)
	{
		switch (cfg.net.rf)
		{
		case RF::GRF:
//			for (uint var = 0; var < _input.size(); ++var)
//			{
//				for (const uint n_id : grf[var])
//				{
//					nodes_old.at(n_id).set_last_spike_time(nodes_old.at(n_id).get_grf_delay(_input[var]) * cfg.net.spiking.max.latency);
//				}
//			}
			break;

		case RF::ARF:
			//				add_input_arf(std::forward<const std::vector<real>&>(_input));
			break;

		case RF::ST:
			//				add_input_st(std::forward<const std::vector<real>&>(_input));
			break;

		default:
			dlog() << "Invalid receptive field type " << cfg.net.rf;
			exit(EXIT_FAILURE);
		}
	}

	std::queue<event> Net::eval_spiking(const std::vector<real>& _input)
	{
//		reset();

		add_input(_input);

		switch (cfg.net.spiking.enc)
		{
		case Enc::Lat:
			return eval_latency();

		case Enc::Rank:
			return eval_rank_order();

		default:
			dlog() << "Invalid encoding type " << cfg.net.spiking.enc;
			exit(EXIT_FAILURE);
		}
	}

	std::queue<event> Net::eval_latency()
	{
		std::queue<event> spikes;

//		while (!scheduler.empty())
//		{
//			/// Event pair: spike time | source | target
//			if ( activate(scheduler.top()) &&
//				 is_output(scheduler.top().second.second) )
//			{
//				spikes.emplace( event(scheduler.top().first, scheduler.top().second.second) );
//			}

//			scheduler.pop();
//		}

		return spikes;
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
								  << " (" << tgt_role.second->weight.val() << ")\n";
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
								  << " (" << tgt_role.second->weight.val() << ")\n";
						}

					}
					_strm << "\t  ^\n";
				}

				_strm << "\t" << *idx.second
					  << " (" << idx.second->af.get_fn() << ")\n\n";
			}
		}

		_strm << "\nTotal nodes: " << _net.node_count()
			  << "\n--------------------------------------\n\n";

		return _strm;
	}
}
