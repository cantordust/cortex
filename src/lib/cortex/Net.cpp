#include "Task.hpp"
#include "Env.hpp"
#include "Net.hpp"
#include "Species.hpp"
#include "Node.hpp"
#include "Layer.hpp"
#include "Conf.hpp"

namespace Cortex
{
	///=====================================
	/// Constructors and destructors
	///=====================================

	Net::Net(const Genome& _genome)
		:
		  id(get_id()),
		  species(Env::get_species(_genome))
	{
		/// Add the network to its species.
		require(species, "Net::Net(const Genome& _genome): Invalid species.");
		species->add(this);

		//		dlog("`-> Setting up layers...");
		for (const auto& lconf : _genome.layers)
		{
			layers.push_back(mkup<Layer>(*this, lconf));
		}

		connect();

		//		dlog("Network ", id, " created successfully.");
		//		dlog(*this);
	}

	Net::Net(Net* const _parent1, Net* const _parent2)
		:
		  id(get_id()),
		  species(_parent1->species) /// It doesn't matter which parent we pick for the species.
	{
		require(species, "Net::Net(Net* const, Net* const): Invalid species.");

		crossover(_parent1, _parent2);
	}

	Net::~Net()
	{
		species->erase(this);
		disconnect();
	}

	///=====================================
	/// Bookkeeping
	///=====================================

	uint Net::get_id(const bool _reset)
	{
		static uint id(0);
		if (_reset)
		{
			id = 0;
			return id;
		}
		return ++id;
	}

	const NetPtr Net::self()
	{
		return shared_from_this();
	}

	///=====================================
	/// Age
	///=====================================

	bool Net::is_old() const
	{
		return (conf->net.max.age > 0 && age > conf->net.max.age);
	}

	void Net::increase_age()
	{
		++age;
		for (const auto& layer : layers)
		{
			for (const auto& node : layer->nodes)
			{
				++node->age;
				for (auto& src : node->sources)
				{
					++src.second.age;
				}
			}
		}
	}

	///=====================================
	/// Counters
	///=====================================

	uint Net::count(const ElemType _et) const
	{
		//		dlog(">>>[Net] Counting elements of type ", _et);

		switch (_et)
		{
		case ElemType::Layer:
			return layers.size();

		case ElemType::Node:
		case ElemType::Link:
			return Cortex::count(layers, [&](const uint _sum, const LayerPtr& _layer)
			{
				return _sum + _layer->count(_et);
			});

		default:
			return 0;
		}
	}

	uint Net::count(const LayerType _lt) const
	{
//		dlog(">>>[Net] Counting layers of type ", _lt);

		return Cortex::count(layers, [&](const uint _sum, const LayerPtr& _layer)
		{
			return (_lt)  == _layer->lconf.type ? _sum + 1 : _sum;
		});
	}

	uint Net::count(const LinkType _lt) const
	{
//		dlog(">>>[Net] Counting links of type ", _lt);

		return Cortex::count(layers, [&](const uint _sum, const LayerPtr& _layer)
		{
			return _sum + _layer->count(_lt);
		});
	}

	///=====================================
	/// Connections
	///=====================================

	void Net::connect()
	{
//		dlog("Connecting network ", id, "...");

		for (const auto& layer : layers)
		{
			layer->connect();
		}
	}

	void Net::disconnect()
	{
//		dlog("Disconnecting network ", id, "...");

		for (const auto& layer : layers)
		{
			layer->disconnect();
		}
	}

	///=====================================
	/// Setters, getters and counters
	///=====================================

	/// Update the fitness and optimise
	/// the associated parameters.
	void Net::set_fitness(const real _value)
	{
		fitness.set(_value);

		if (_value >= Fitness::target)
		{
			switch (stage)
			{
			case Stage::Train:
				stage = Stage::Dev;
				break;

			case Stage::Dev:
				stage = Stage::Test;
				break;

			case Stage::Test:
				Task::is_solved(self());
				break;

			default:
				break;
			}
		}

		/// Check whether we should optimise the current
		/// parameters and re-evaluate the network...
		if (rnd_chance(fitness.abs.get_offset()))
		{
			/// Learn something...
			mutate();

			/// ... and re-evaluate.
			Task::threadpool.enqueue(Task::evaluate, self());
		}
	}

	/// @todo Implementation
	uint Net::get_max_links() const
	{
		return 0.0;
	}

	/// @todo Implementation
	real Net::get_link_density() const
	{
		return 0.0;
	}

	Genome Net::get_genome() const
	{
		Genome genome;
		genome.layers.reserve(layers.size());
		for (const auto& layer : layers)
		{
			genome.layers.push_back(layer->lconf);
		}
		return genome;
	}

	void Net::set_species(Species* _species)
	{
		/// Erase the network from the current species
		/// if the latter exists.
		if (species)
		{
			species->erase(this);
		}

		/// Update the species
		species = _species;

		/// Register the network with the new species.
		species->add(this);
	}

	Node* const Net::get_node(const NodeID& _id)
	{
		return ( (_id.layer.is_set() &&
				  _id.node.is_set() &&
				  _id.layer() < layers.size() &&
				  _id.node() < layers[_id.layer()]->nodes.size())
				? layers[_id.layer()]->nodes[_id.node()].get()
				: nullptr);
	}

	///=====================================
	/// Evolution
	///=====================================

//	void Net::add_link()
//	{
//		/// Pick a random layer for the source node
//		uint max_src_layer(layers.size() - 2);
//		if (conf->link.type.lateral ||
//			conf->link.type.recurrent)
//		{
//			/// Include the output layer
//			max_src_layer = layers.size() - 1;
//		}

//		uint src_layer(rnd_int<uint>(0, max_src_layer));

//		/// Pick a random node from that layer
//		Node* src(rnd_elem(layers[src_layer]->nodes).get());

//		/// Create a range of potential target layers
//		std::pair<uint, uint> tgt_layer_range(src->id.layer + 1, src->id.layer + 1);

//		/// If lateral links are enabled, extend the range to
//		/// include the layer of the source node
//		if (conf->link.type.lateral)
//		{
//			tgt_layer_range.first = src->id.layer;
//		}

//		/// If recurrent links are enabled, extend the range to include
//		/// all layers before the one of the source node
//		if (conf->link.type.recurrent)
//		{
//			tgt_layer_range.first = 0;
//		}

//		/// If skip links are enabled, extend the range to include
//		/// all layers after the one of the source node
//		if (conf->link.type.skip)
//		{
//			tgt_layer_range.second = layers.size() - 1;
//		}

//		/// Pick a target node
//		hmap<Node*, real> candidates;
//		SMAStat stat;

//		///Iterate over layers
//		for (const auto& layer : layers)
//		{
//			/// Iterate over nodes in the layer
//			for (const auto& node : layer->nodes)
//			{
//				/// Check if the node is already a target
//				if (src->links.find(node.get()) == src->links.end())
//				{
//					candidates[node.get()] = node->potential.mean;
//					stat.update(node->potential.mean);
//				}
//			}
//		}

//		/// Return if there is no suitable target
//		if (candidates.empty())
//		{
//			return false;
//		}

//		/// Normalise.
//		for (auto& tgt : candidates)
//		{
//			tgt.second = logistic(stat.get_offset(tgt.second));
//		}

//		/// Nodes with higher mean membrane potential
//		/// are more likely to be picked
//		Node* tgt(roulette(candidates));

//		if (tgt->id.layer != src->id.layer + 1)
//		{
//			/// This is not a standard forward link to a node
//			/// in the following layer.
//			/// Add the source node to the list of visitors
//			/// in the relevant layer.

//			src->add_link(tgt, layers[tgt->id.layer]->weight_elem(src, tgt));
//		}
//		else if (tgt->id.layer == src->id.layer + 1)
//		{

//		}

//		Node* node(get_rnd_node());
//		if (node &&
//			node->mutate(MutType::AddLink))
//		{
//			make_layers();
//			return true;
//		}
//		return false;
//	}

	void Net::erase_link()
	{
		/// Weighted distribution of nodes to choose from.
		/// All source links of the selected node are added
		/// to the fitness optimiser.
		wmap<Node*> nodes;

		/// Populate the roulette wheel.
		for (const auto& layer : layers)
		{
			for (const auto& node : layer->nodes)
			{
				nodes[node.get()] = node->age;
			}
		}

		nodes.flip_spin()->erase_link();
	}

	void Net::mutate()
	{
		/// Clear the currently stored parameters.
		fitness.clear_parameters();

		/// Weighted distribution of nodes to choose from.
		/// All source links of the selected node are added
		/// to the fitness optimiser.
		wmap<Node*> nodes;

		/// Populate the roulette wheel.
		for (const auto& layer : layers)
		{
			for (const auto& node : layer->nodes)
			{
				nodes[node.get()] = node->age;
			}
		}

		/// Pick the candidate node for optimisation.
		Node* candidate(nodes.flip_spin());

		/// Iterate over all source links of the candidate
		/// and add them to the fitness optimiser.
		for (auto& link : candidate->sources)
		{
			fitness.add_parameter(link.second.weight);
		}

		/// Compile the vectorised network representation
		/// from the network description (this).
		evaluator.compile(*this);
	}

	void Net::evolve()
	{
		/// Get a copy of the network's genome
		/// so we can manipulate it.
		Genome genome(get_genome());

		/// Mutate the genome.
		Mutation mut;
		while (	!(mut = genome.mutate()) );

		/// Shorthand for the layer index.
		uint layer(mut.layer.index());

		/// Apply the genome mutation to the genotype.
		switch (mut.element)
		{
		case ElemType::Layer:
			mut.action == Action::Inc ? layers.emplace(layers.begin() + layer, mkup<Layer>(*this, genome.layers[layer]))
									  : layers.erase(layers.begin() + layer);
			break;

		case ElemType::Node:
			layers[layer]->mutate(genome.layers[layer]);
			break;

		default:
			break;
		}

		/// Compile the vectorised network representation
		/// from the network description (this).
		evaluator.compile(*this);
	}

	void Net::crossover(Net* const _parent1, Net* const _parent2)
	{
		///=================
		/// Preliminary steps
		///=================

		/// Roulette wheel for selecting chromosomes
		/// (layers) and genes (nodes) from the two
		/// parents based on their fitness.
		wmap<Net*> parent_wheel
		{
			{_parent1, _parent1->fitness.rel.value},
			{_parent2, _parent2->fitness.rel.value}
		};

		/// Basic sanity check.
		require(!_parent1->layers.empty(), "Error: Network ", _parent1->id, " is empty.");
		require(!_parent2->layers.empty(), "Error: Network ", _parent2->id, " is empty.");

		/// Input layers.
		Layer* layer1(_parent1->layers.front().get());
		Layer* layer2(_parent2->layers.front().get());

		/// Output layers.
		Layer* end1(_parent1->layers.back().get());
		Layer* end2(_parent2->layers.back().get());

		/// Make sure that the input and output layers have the same type.
		/// If they are not, we have a bigger problem.
		require(layer1->lconf.type == layer2->lconf.type, "Error: Input layer types do not match.");
		require(end1->lconf.type == end2->lconf.type, "Error: Output layer types do not match.");

		///=================
		/// Select layers
		///=================

		/// Reference genotypes containing layers collected
		/// from the two parents.
		///
		/// @note The reference genotypes will end up
		/// having the same size even if the two parents
		/// have genotypes with different sizes.
		/// This is necessary in case speciation is disabled
		/// and crossover can happen between any two networks.
		std::vector<Layer*> genotype1;
		std::vector<Layer*> genotype2;

		/// Iterate over the layers of the two parents.
		while (layer1 && layer2)
		{
			/// Store the parents' layers into the reference genotypes.
			genotype1.push_back(layer1);
			genotype2.push_back(layer2);

			layer1 = layer1->next();
			layer2 = layer2->next();

			/// Ensure that reference layers match in type.
			if (layer1 && layer2 && layer1->lconf.type != layer2->lconf.type)
			{
				/// Store the last matching type from one of the genotypes
				/// (it doesn't matter which one, they should be the same).
				LayerType last_type(genotype1.back()->lconf.type);

				/// Select a layer at random.
				/// If we select the layer that matches the
				/// last stored layer type, store all subsequent
				/// layers of the same type from that parent.
				Layer*& layer(parent_wheel.spin() == _parent1 ? layer1 : layer2);

				while (layer && layer->lconf.type == last_type)
				{
					genotype1.push_back(layer);
					genotype2.push_back(layer);
					layer = layer->next();
				}
			}

			/// Check if we have reached the end of
			/// one network but not the other.
			if ((layer1 == end1 && layer2 != end2) ||
				(layer2 == end2 && layer1 != end1))
			{
				/// Optionally store the extra layers.
				Layer*& layer(parent_wheel.spin() == _parent1 ? layer1 : layer2);
				Layer* end(layer == layer1 ? end1 : end2);

				while (layer && layer != end)
				{
					genotype1.push_back(layer);
					genotype2.push_back(layer);
					layer = layer->next();
				}
			}
		}

		///=================
		/// Select nodes
		///=================

		/// Perform crossover by selecting nodes
		/// from corresponding chromosomes.
		std::vector<std::vector<Node*>> genotype;

		for (uint l = 0; l < genotype1.size(); ++l)
		{
			/// Insert the description of one of the parents'
			///	layers into this network's genotype.
			layers.push_back(mkup<Layer>(*this, (parent_wheel.spin() == _parent1 ? genotype1[l]->lconf
																				 : genotype2[l]->lconf)));

			/// Create a reference collection of nodes for this layer.
			/// These will be used to populate the layers with nodes.
			genotype.emplace_back();

			/// Select nodes.
			Node* n1(genotype1[l]->nodes.front().get());
			Node* n2(genotype2[l]->nodes.front().get());

			/// Roulette wheel for selecting nodes based on
			/// the parent's fitness and the node's age.
			wmap<Node*> node_wheel;

			/// Iterate over nodes in this layer.
			while (n1 && n2)
			{
				/// Update the roulette wheel
				node_wheel =
				{
					{n1, _parent1->fitness.rel.value * n1->age},
					{n2, _parent2->fitness.rel.value * n2->age}
				};

				genotype.back().push_back(node_wheel.spin());
				n1 = n1->next();
				n2 = n2->next();

				/// If we have run out of nodes in one of the layers,
				/// check the layer description to see if we should
				/// store the extra nodes from the other layer.
				if ((!n1 || !n2) && layers.back()->lconf.nodes > genotype.back().size())
				{
					/// Get the node which is *not* at the end of its layer.
					Node*& node(n1 ? n1 : n2);

					/// Store all subsequent nodes
					while (node)
					{
						genotype.back().push_back(node);
						node = node->next();
					}
				}
			}
		}

		///=================
		/// Initialise layers
		///=================

		for (uint layer = 0; layer < genotype.size(); ++layer)
		{
			layers[layer]->init(genotype[layer]);
		}

		///=================
		/// Connect nodes
		///=================

		for (uint layer = 0; layer < genotype.size(); ++layer)
		{
			layers[layer]->connect(genotype[layer]);
		}
	}

	///=====================================
	/// Evaluation
	///=====================================

	void Net::evaluate(const Sample& _sample)
	{

	}

	///=====================================
	/// Serialisation and printing
	///=====================================

	json Net::to_json() const
	{
		return json();
	}

	os& operator << (os& _os, const Net& _net)
	{
		_os << "\n----------------[ Network" << _net.id << "]----------------\n";

		_os << "\nAge:\t" << _net.age
			<< "\nSpecies:\t" << _net.species->id
			<< "\nLayers:\t" << _net.count(ElemType::Layer);

		for (const auto& type : values<LayerType>)
		{
			uint count(_net.count(type.first));
			if (count > 0)
			{
				_os << "\n\t" << type.second.pretty_name << ": " << count;
			}
		}

		_os << "\nNodes:" << _net.count(ElemType::Node)
			<< "\nLinks:" << _net.count(ElemType::Link);

		for (const auto& type : conf->link.types)
		{
			_os << "\n\t" << type << ": " << _net.count(type);
		}

		_os << "\nFitness:" << _net.fitness
			<< "\n-----------------------------------------------\n\n";

		return _os;
	}

}
