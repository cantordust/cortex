#include "Net.hpp"
#include "Genome.hpp"
#include "Mutation.hpp"
#include "Node.hpp"
#include "Layer.hpp"
#include "Model.hpp"
#include "Conf.hpp"

namespace Cortex
{
	///=============================================================================
	///	Static methods
	///=============================================================================

	NetPtr Net::create(const std::vector<LayerDef>& _defs)
	{
		auto net(mksp<Net>(_defs));
		net->connect();
		nets.emplace(net);
		return net;
	}

	NetPtr Net::create(const Genome& _genome)
	{
		return create(_genome.layers);
	}

	NetPtr Net::create(const DNA& _dna)
	{
		auto net(mksp<Net>(_dna));
		net->connect(_dna);
		nets.emplace(net);
		return net;
	}

	NetPtr Net::create(const Net& _other)
	{
		return create(_other.layers);
	}

	NetPtr Net::create(const Net& _parent1, const Net& _parent2)
	{
		auto net(mksp<Net>());
		DNA dna(net->crossover(_parent1.layers, _parent2.layers));

		///=================
		/// Connect nodes
		///=================

		for (uint layer = 0; layer < dna.size(); ++layer)
		{
			net->layers[layer]->connect(dna[layer]->nodes);
		}

		nets.emplace(net);
		return net;
	}

	///=============================================================================
	///	Instance methods
	///=============================================================================

	///=====================================
	/// Constructors and destructors
	///=====================================

	Net::Net(const std::vector<LayerDef>& _defs)
		:
		  id(id_generator.next())
	{
//		dlog("`-> Adding layers...");

//		for (const auto& c : _conf)
//		{
//			add_layer(c);
//		}

//		dlog("Network ", id, " created successfully.");
//		dlog(*this);
	}

	Net::Net(const DNA& _dna)
		:
		  id(id_generator.next())
	{
//		for (const auto& ref_layer : _dna)
//		{
//			require(ref_layer != nullptr, "[Net::Net(const DNA& _dna)] Invalid reference layer.");
//			add_layer(*ref_layer);
//		}
	}

//	Net::Net(const NetPtr& _parent1, const NetPtr& _parent2)
//		:
//		  id(id_generator.next())
//	{
//		crossover(_parent1, _parent2);
//	}

	Net::~Net()
	{
		disconnect();
	}

	NetPtr Net::self()
	{
		return shared_from_this();
	}

	bool Net::is_old() const
	{
		return (conf.net.max.age > 0
				&& age > conf.net.max.age);
	}

	///=====================================
	/// Counters
	///=====================================

	template<>
	uint Net::count<ElemType>(const ElemType _et) const
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

	template<>
	uint Net::count<LayerType>(const LayerType _lt) const
	{
//		dlog(">>>[Net] Counting layers of type ", _lt);

		return Cortex::count(layers, [&](const uint _sum, const LayerPtr& _layer)
		{
			return (_lt)  == _layer->type ? _sum + 1 : _sum;
		});
	}

	template<>
	uint Net::count<LinkType>(const LinkType _lt) const
	{
//		dlog(">>>[Net] Counting links of type ", _lt);

		return Cortex::count(layers, [&](const uint _sum, const LayerPtr& _layer)
		{
			return _sum + _layer->count(_lt);
		});
	}
	///=====================================
	/// Setters, getters and counters
	///=====================================

	/// Update the fitness and optimise
	/// the associated parameters.
	void Net::set_fitness(const real _value)
	{
		fitness.abs.update(_value);

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
				Cortex::mark_solved(self());
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
			Cortex::enqueue(self());
		}
	}

	/// @todo Implementation
	uint Net::get_max_links() const
	{
		return 0.0;
	}

//	opt<uint> Net::get_first(const LayerType _lt)
//	{
//		for (uint index = 0; index < layers.size(); ++index)
//		{
//			if (layers[index]->type == _lt)
//			{
//				return index;
//			}
//		}
//		return nopt;
//	}

//	opt<uint> Net::get_last(const LayerType _lt)
//	{
//		for (uint index = layers.size(); index-- > 0; )
//		{
//			if (layers[index]->type == _lt)
//			{
//				return index;
//			}
//		}
//		return nopt;
//	}

//	opt<uint> Net::get_nth(const LayerType _lt, const uint _n)
//	{
//		uint found(0);
//		for (uint index = 0; index < layers.size(); ++index)
//		{
//			if (layers[index]->type == _lt &&
//				++found == _n)
//			{
//				return index;
//			}
//		}
//		return nopt;
//	}

	LayerPtr Net::get_layer(const LayerType _type, const uint _index)
	{
		if (layers.empty() ||
			_index >= layers.size())
		{
			return nullptr;
		}

		uint found(0);
		for (uint index = 0; index < layers.size(); ++index)
		{
			if ((_type == LayerType::Undef ||
				layers[index]->type == _type) &&
				found++ == _index)
			{
				return layers[index];
			}
		}

		return nullptr;
	}

	DNA Net::get_source_layers(const LayerPtr& _layer, const LinkType _lt)
	{
		if (!_layer ||
			_layer->net != this)
		{
			return {};
		}

		std::vector<LayerPtr> source_layers;

		/// @todo Check if links other than forward ones
		/// are supported by the backend for non-regular layers.
		if (_layer->type != LayerType::Regular)
		{
			switch (_lt)
			{
			case LinkType::Recurrent:
			case LinkType::Lateral:
			case LinkType::Skip:
				return source_layers;

			default:
				break;
			}
		}

		/// Default begin and end indices for forward links.
		uint index(_layer->get_index().value());
		uint end(index);
		uint begin(end == 0 ? 0 : end - 1);

		switch (_lt)
		{
		case LinkType::Skip:
			begin = 0;
			end = (index == 0 ? 0 : index - 1);
			break;

		case LinkType::Recurrent:
			begin = (_layer == 0 ? 1 : index);
			end = layers.size();
			break;

		case LinkType::Lateral:
			begin = index;
			end = index + 1;
			break;

		default:
			break;
		}

		while (begin < end)
		{
			source_layers.emplace_back(layers[begin].get());
			++begin;
		}

		return source_layers;
	}

	DNA Net::get_target_layers(const LayerPtr& _layer, const LinkType _lt)
	{
		if (!_layer ||
			_layer->net != this)
		{
			return {};
		}

		std::vector<LayerPtr> target_layers;

		/// @todo Check if links other than forward ones
		/// are supported by the backend for non-regular layers.
		if (_layer->type != LayerType::Regular)
		{
			switch (_lt)
			{
			case LinkType::Recurrent:
			case LinkType::Lateral:
			case LinkType::Skip:
				return target_layers;

			default:
				break;
			}
		}

		/// Default begin and end indices for forward links.
		uint index(_layer->get_index().value());
		uint begin(index + 1);
		uint end(begin == layers.size() ? layers.size() : begin + 1);

		switch (_lt)
		{
		case LinkType::Skip:
			begin = (begin == layers.size() ? layers.size() : begin + 1);
			end = layers.size();
			break;

		case LinkType::Recurrent:
			begin = 0;
			end = index;
			break;

		case LinkType::Lateral:
			begin = index;
			end = index + 1;
			break;

		default:
			break;
		}

		while (begin < end)
		{
			target_layers.emplace_back(layers[begin].get());
			++begin;
		}

		return target_layers;
	}

	/// @todo Implementation
	real Net::get_link_density() const
	{
		return 0.0;
	}

	const DNA& Net::get_dna() const
	{
		return layers;
	}

	GenomePtr Net::get_genome() const
	{
		if (conf.species.enabled)
		{
			return Genome::match(layers);
		}
		return nullptr;
	}

	NodePtr Net::get_node(const NodeID& _id)
	{
		if ((_id.layer &&
			 _id.node &&
			 _id.layer.value() < layers.size() &&
			 _id.node.value() < layers[_id.layer.value()]->nodes.size()))
		{
			return layers[_id.layer.value()]->nodes[_id.node.value()];
		}
		return nullptr;
	}

	real Net::get_cull_chance(const SMAStat& _age_stat)
	{
		/// @note Using tanh instead of the default logistic
		///	function protects new offspring as their age is 0.
		real chance(_age_stat.get_offset<FuncType::Tanh>(age) * (1.0 - fitness.rel));

		/// If speciation is enabled, the culling chance is
		/// further scaled by the relative fitness of the genome.
		return (conf.species.enabled ? chance  * (1.0 - get_genome()->fitness.rel) : chance);
	}

	LayerPtr Net::add_layer(const LayerDef& _def, const LayerPtr& _next)
	{
		opt<uint> index(layers.size());
		if (_next)
		{
			if (_next->net != this)
			{
				return nullptr;
			}
			index = _next->get_index().value();
		}

		return *(layers.emplace(layers.begin() + index.value(), Layer::create(_def, this)));
	}

	LayerPtr Net::add_layer(const LayerPtr& _ref, const LayerPtr& _next)
	{
//		opt<uint> index(layers.size());
//		if (_next)
//		{
//			if (_next->net != this)
//			{
//				return nullptr;
//			}
//			index = _next->get_index().value();
//		}

//		return *(layers.emplace(layers.begin() + index.value(), Layer::create(_ref, this)));
		return add_layer(_ref->get_def(), _next);
	}

	///=====================================
	/// Structural changes
	///=====================================

	//	void Net::add_link()
	//	{
	//		/// Pick a random layer for the source node
	//		uint max_src_layer(layers.size() - 2);
	//		if (conf.link.type.lateral ||
//			conf.link.type.recurrent)
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
//		if (conf.link.type.lateral)
//		{
//			tgt_layer_range.first = src->id.layer;
//		}

//		/// If recurrent links are enabled, extend the range to include
//		/// all layers before the one of the source node
//		if (conf.link.type.recurrent)
//		{
//			tgt_layer_range.first = 0;
//		}

//		/// If skip links are enabled, extend the range to include
//		/// all layers after the one of the source node
//		if (conf.link.type.skip)
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

//	void Net::erase_link()
//	{
//		rnd_elem(layers)->erase_link();
//	}

	void Net::mutate()
	{
		/// Get a copy of the network's genome
		/// so we can manipulate it.
		GenomePtr genome(Genome::create(*get_genome()));

		/// Mutate the genome.
		MutationPtr mut;
		while (	!(mut = genome->mutate()) );

		/// Apply the genome mutation to the genotype.
		switch (mut->element)
		{
		case ElemType::Layer:
			if (mut->action == Action::Inc)
			{
				add_layer(genome->layers[mut->layer.index], layers[mut->layer.index]);
			}
			else
			{
				layers.erase(layers.begin() + mut->layer.index);
			}
			break;

		case ElemType::Node:
			layers[mut->layer.index]->mutate(mut);
			break;

		default:
			break;
		}

		/// Compile the vectorised network representation
		/// from the network description (this).
		model->compile(*this);
	}

	DNA Net::crossover(const Net& _parent1, const Net& _parent2)
	{
		///=================
		/// Preliminary steps
		///=================

		const NetPtr p1(const_cast<Net&>(_parent1).self());
		const NetPtr p2(const_cast<Net&>(_parent2).self());

		/// Roulette wheel for selecting chromosomes
		/// (layers) and genes (nodes) from the two
		/// parents based on their fitness.
		Roulette<NetPtr> parent_wheel
		{
			{p1, _parent1.fitness.rel.value},
			{p2, _parent2.fitness.rel.value}
		};

		/// Basic sanity check.
		require(!_parent1.layers.empty(), "Error: Parent ", _parent1.id, " is empty.");
		require(!_parent2.layers.empty(), "Error: Parent ", _parent2.id, " is empty.");

		/// First layers of the respective genotypes
		LayerPtr layer1(_parent1.layers.front());
		LayerPtr layer2(_parent2.layers.front());

//		/// Last layers of the respective genotypes
//		LayerPtr end1(_parent1.layers.back());
//		LayerPtr end2(_parent2.layers.back());

		/// Make sure that the input and output layers have the same type.
		/// If they are not, we have a bigger problem.
		require(layer1->type == layer2->type, "Error: Input layer types do not match.");
//		require(end1->type == end2->type, "Error: Output layer types do not match.");

		/// Build reference genotypes containing layers
		/// collected from the two parents.
		///
		/// @note The reference genotypes will end up
		/// having the same size even if the two parents
		/// have genotypes with different sizes.
		/// This is necessary in case speciation is disabled
		/// and crossover can happen between any two networks.
		DNA dna1;
		DNA dna2;

		/// Iterate over the layers of the two parents.
		while (layer1 && layer2)
		{
			/// Store the parents' layers into the reference genotypes.
			dna1.push_back(layer1);
			dna2.push_back(layer2);

			layer1 = layer1->get_next();
			layer2 = layer2->get_next();

			/// Ensure that reference layers match in type.
			if (layer1 && layer2 && layer1->type != layer2->type)
			{
				/// Store the last matching type from one of the genotypes
				/// (it doesn't matter which one, they should be the same).
				LayerType last_type(dna1.back()->type);

				/// Select a layer at random.
				/// If we select the layer that matches the
				/// last stored layer type, store all subsequent
				/// layers of the same type from that parent.
				LayerPtr extra_layer(parent_wheel.spin() == p1 ? layer1 : layer2);

				while (extra_layer && extra_layer->type == last_type)
				{
					dna1.push_back(extra_layer);
					dna2.push_back(extra_layer);
					extra_layer = extra_layer->get_next();
				}
			}
		}

		/// Check if we have reached the end of
		/// one network but not the other.
		if (layer1 || layer2)
		{
			/// Optionally store the extra layers.
			LayerPtr extra_layer(parent_wheel.spin() == p1 ? layer1 : layer2);
//				LayerPtr end(extra_layer == layer1 ? end1 : end2);

			while (extra_layer)
			{
				dna1.push_back(extra_layer);
				dna2.push_back(extra_layer);
				extra_layer = extra_layer->get_next();
			}
		}

		///=================
		/// Crossover
		///=================

		/// Perform crossover for each pair of chromosomes (layers).
		DNA dna;

		for (uint layer = 0; layer < dna1.size(); ++layer)
		{
			/// Create an empty layer
			dna.emplace_back(Layer::create(this));

			/// First nodes
			NodePtr node1(dna1[layer]->nodes.front());
			NodePtr node2(dna2[layer]->nodes.front());

//			/// Last nodes
//			NodePtr end1(genotype1[layer]->nodes.back());
//			NodePtr end2(genotype2[layer]->nodes.back());

			/// Iterate over nodes in this layer.
			while (node1 && node2)
			{
				dna.back()->nodes.push_back(parent_wheel.spin() == p1 ? node1 : node2);
				node1 = node1->get_next();
				node2 = node2->get_next();
			}

			/// Check if we have reached the end of
			/// one genotype but not the other.
			if (node1 || node2)
			{
				/// Store all subsequent nodes if we pick the node
				/// which is *not* at the end of its layer.
				NodePtr node(parent_wheel.spin() == p1 ? node1 : node2);

				while (node)
				{
					dna.back()->nodes.push_back(node);
					node = node->get_next();
				}
			}
		}

		///=================
		/// Add layers and nodes
		///=================

		for (const auto& layer : dna)
		{
			layers.push_back(Layer::create(*layer, this));
		}

		return dna;
	}

	///=====================================
	/// Evaluation
	///=====================================

	void Net::evaluate(const Sample& _sample)
	{
//		model->evaluate(_sample);
	}

	///=====================================
	/// Printing and (de)serialisation
	///=====================================

	os& operator << (os& _os, const Net& _net)
	{
		_os << "\n----------------[ Network" << _net.id << "]----------------\n";

		_os << "\nAge:\t" << _net.age
			<< "\nGenome:\t" << _net.get_genome()->id
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

		for (const auto& type : conf.link.types)
		{
			_os << "\n\t" << type << ": " << _net.count(type);
		}

		_os << "\nFitness:" << _net.fitness
			<< "\n-----------------------------------------------\n\n";

		return _os;
	}

	void Net::load(const json& _j)
	{

	}

	void Net::save(json& _j) const
	{

	}

	///=====================================
	/// Connections
	///=====================================

	void Net::connect(const DNA& _dna)
	{
//		dlog("Connecting network ", id, "...");

		require(_dna.empty() || _dna.size() == layers.size(), "[Net::connect] Invalid number of reference layers.");

		for (uint layer = 0; layer < layers.size(); ++layer)
		{
			_dna.empty() ? layers[layer]->connect() : layers[layer]->connect(_dna[layer]->nodes);
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
	/// Evolution
	///=====================================

	void Net::evolve()
	{
		dlog d(">>> Evolving environment");
		/// Increase the age of all networks.
		for (const auto& net : Net::nets)
		{
			++net->age;
		}

		/// Reset the champion.
		Net::champ = nullptr;

		/// Compute the relative fitness of networks and genomes.
		d << "\t`-> Calibrating...";
		calibrate();

		/// Evolve networks in each genome.
		d << "\t`-> Evolving networks...";
		for (const auto& genome : Genome::genomes)
		{
			genome->evolve();
		}

		/// Eliminate unfit networks and empty genomes.
		d << "\t`-> Culling...";
		cull();
	}

	void Net::calibrate()
	{
		/// A statistics package for collecting
		/// genome statistics on the fly.
		SMAStatPack spc_stat;

		/// Highest fitness seen so far.
		real top_fitness(std::numeric_limits<real>::min());

		for (const auto& genome : Genome::genomes)
		{
			/// Compute the relative network fitness
			/// and the absolute genome fitness.
			NetPtr genome_champ(genome->calibrate());

			if (!Net::champ ||
				(genome_champ &&
				 genome_champ->fitness.abs > top_fitness))
			{
				Net::champ = genome_champ;
				top_fitness = genome_champ->fitness.abs;
			}
		}

		uint gen(0);
		for (const auto& genome : Genome::genomes)
		{
			/// Compute the relative genome fitness
			genome->fitness.rel.update( genome->fitness.abs.get_offset() );

			dlog("Genome ", ++gen, "fitness:"
				 "\t\tAbsolute: ", genome->fitness.abs,
				 "\t\tRelative: ", genome->fitness.rel);
		}
	}

	void Net::cull()
	{
		/// Net cemetery.
		hset<NetPtr> cemetery;

		/// Network age statistics.
		SMAStat age_stat;
		for (const auto& net : Net::nets)
		{
			age_stat.update(net->age);
		}

		for (const auto& net : Net::nets)
		{
			/// The probabiliy of culling a network is directly
			/// proportional to its age and inversely proportional
			/// to the relative fitness of the network and its genome.
			real cull_chance(net->get_cull_chance(age_stat));

			if (rnd_chance(cull_chance))
			{
				cemetery.emplace(net);
			}
		}

		/// Erase culled networks.
		while (!cemetery.empty())
		{
			auto net(*cemetery.begin());
			dlog("Erasing network ", net);

			/// Erase the network from the genome.
			net->get_genome()->nets.erase(net);

			/// Erase the network from the environment.
			Net::nets.erase(net);

			/// Funeral.
			cemetery.erase(net);
		}

		/// Remove extinct genomes.
		for (auto it = Genome::genomes.begin(); it != Genome::genomes.end(); )
		{
			(*it)->nets.empty() ? it = Genome::genomes.erase(it) : ++it;
		}
	}
}
