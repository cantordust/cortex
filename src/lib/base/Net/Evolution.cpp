#include "Ecosystem.hpp"
#include "Species.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Conf.hpp"

namespace Cortex
{
	void Net::mutate()
	{
		/// Determine the type of mutation to perform.
		auto mut_prob(conf.mut.prob);

		/// This procedure tries to make sure that
		/// we are not going to end up with an ecosystem
		/// full of bloated individuals.
		/// High saturation indicates that we have
		/// the potential for adding connections is low,
		/// so we should probably add a new node.
		if (conf.mut.adaptive)
		{
			real sat( get_saturation() );

			/// Low saturation => low probability of
			/// adding a node or erasing a link
			if (mut_prob.find(Mut::AddNode) != mut_prob.end())
			{
				mut_prob.at(Mut::AddNode) *= sat;
			}
			if (mut_prob.find(Mut::EraseLink) != mut_prob.end())
			{
				mut_prob.at(Mut::EraseLink) *= sat;
			}

			/// Low saturation => high probability of
			/// adding a link or erasing a node
			if (mut_prob.find(Mut::EraseNode) != mut_prob.end())
			{
				mut_prob.at(Mut::EraseNode) *= (1.0 - sat);
			}
			if (mut_prob.find(Mut::AddLink) != mut_prob.end())
			{
				mut_prob.at(Mut::AddLink) *= (1.0 - sat);
			}

		}

		while (!mutate(conf.w_dist(mut_prob)));
	}

	bool Net::add_node(const NR _role, const uint _count)
	{
		/// Add a node and connect it to the network.
		Genotype new_gen(spc->get_genotype());
		if (!new_gen.add(_role, _count))
		{
			return false;
		}

		SpcPtr new_spc(eco.get_spc(new_gen));
		if (!new_spc)
		{
			return false;
		}

		if (_role == NR::I &&
			_count % conf.net.rf.size != 0)
		{
			dlog() << "Invalid number of input nodes (" << _count << "): \n"
				   << "Should be a multiple of " << conf.net.rf.size;
			return false;
		}

		for (uint c = 0; c < _count; ++c)
		{
			/// Insert a new node
			insert_nodes(_role);

			/// Connect the node to the network
			nodes[_role].back()->connect();
		}

		/// Update the species
		set_spc(new_spc);

		/// Update the graph
		make_graph();

		if (_role == NR::I)
		{
			rfield.init();
		}

		return true;
	}

	bool Net::erase_node(const NR _role)
	{
		/// Check if the new genotype is allowed
		Genotype new_gen(spc->get_genotype());
		if (!new_gen.erase(_role, 1))
		{
			return false;
		}

		SpcPtr new_spc(eco.get_spc(new_gen));
		if (!new_spc)
		{
			return false;
		}

		//			dlog() << "Before erase_node():\n" << *this;

		const NodePtr del_node(get_rnd_node(_role));
		if (!del_node)
		{
			return false;
		}

//		dlog() << "\tErasing node " << del_node->id;

		auto it (nodes[_role].begin() + del_node->id.idx - 1);
		if ((*it)->id != del_node->id)
		{
			dlog() << "Node index mismatch: requested " << del_node->id
				   << ", got " << (*it)->id;
			exit(EXIT_FAILURE);
		}

		del_node->disconnect();

		/// Iterate over nodes in the same role
		/// and decrement idx by 1 if
		/// it is greater than the one deleted.
		/// Erase any links to and from
		/// the node being erased.
		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				if (node->id > del_node->id)
				{
					--node->id;
				}
			}
		}

		nodes[_role].erase(it);

		/// @todo Checks for input nodes.

		/// Make sure that the network is in a valid state
		connect();

		/// Regenerate the evaluation graph
		make_graph();

		/// Update the species
		set_spc(new_spc);

		dlog() << "After erase_node():" << *this;

		return true;
	}

	bool Net::add_link()
	{
		/// Pick a random node
		if (get_rnd_node()->mutate(Mut::AddLink))
		{
			make_graph();
			return true;
		}
		return false;

	}

	bool Net::erase_link()
	{
		if (get_rnd_node()->mutate(Mut::EraseLink))
		{
			make_graph();
			return true;
		}
		return false;
	}

	void Net::set_spc(const SpcPtr& _spc)
	{
		/// Erase the network from the current species
		/// if the latter exists.
		if (spc)
		{
			spc->erase_net(id);
		}

		/// Update the species
		spc = _spc;

		/// Register the network with the new species.
		spc->add_net(*this);
	}

	std::vector<NodePtr> Net::insert_nodes(const NR _nr, const uint _count)
	{
		std::vector<NodePtr> new_nodes;
		for (uint c = 0; c < _count; ++c)
		{
			nodes[_nr].emplace_back(std::make_shared<Node>(_nr, nodes[_nr].size() + 1, *this));
			new_nodes.emplace_back(nodes[_nr].back());
		}
		return new_nodes;
	}

	const NodePtr Net::insert_nodes(const NodePtr& _other)
	{
		if (nodes[_other->id.role].size() >= _other->id.idx)
		{
			return nullptr;
		}
		nodes[_other->id.role].emplace_back(std::make_shared<Node>(*_other, *this));
		return nodes[_other->id.role].back();
	}

	void Net::crossover(const NetPtr& _p1, const NetPtr& _p2)
	{
		/// Relative fitness scores of the two parents.
		hmap<uint, real> fdist;
		fdist.emplace(_p1->id, _p1->get_fitness());
		fdist.emplace(_p2->id, _p2->get_fitness());

		// Create the nodes
		for (const auto& role : _p1->nodes)
		{
			for (const auto& p1_node : role.second)
			{
				if (!insert_nodes(conf.w_dist(fdist) == _p1->id ? p1_node : _p2->get_node(p1_node->id)))
				{
					dlog() << "Net::crossover(): node replication failed!";
					exit(EXIT_FAILURE);
				}
			}
		}

		/// Populate the network with links
		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				node->crossover(_p1->get_node(node->id), _p2->get_node(node->id), fdist);
			}
		}

		connect();
		make_graph();
		rfield.init();
	}

	///=========================================
	/// Dispatch functions
	///=========================================

	bool Net::mutate_param(const Mut _mut)
	{
		return get_rnd_node()->mutate(_mut);
	}

	bool Net::mutate(const Mut _mut)
	{
//		dlog() << "Mutating network " << id << ": " << _mut;
		switch (_mut)
		{
		case Mut::Weight:
		case Mut::Tau:
		case Mut::TF:
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
}
