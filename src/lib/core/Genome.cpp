#include "Conf.hpp"
#include "Net.hpp"
#include "Genome.hpp"
#include "Layer.hpp"
#include "Mutation.hpp"

namespace Cortex
{
	///=============================================================================
	///	Static methods
	///=============================================================================

	GenomePtr Genome::create(const std::vector<LayerDef>& _defs)
	{
		/// Check if genomes are disabled or
		/// if we have reached the limit.
		if (!conf.species.enabled ||
			genomes.size() >= conf.species.max.count)
		{
			return nullptr;
		}

		auto it(genomes.emplace(mksp<Genome>(_defs)));
		return *(it.first);
	}

	GenomePtr Genome::create(const Genome& _other)
	{
		return create(_other.layers);
	}

	GenomePtr Genome::match(const DNA& _dna)
	{
		for (const auto& genome : genomes)
		{
			if (*genome == _dna)
			{
				return genome;
			}
		}
		return nullptr;
	}

	GenomePtr Genome::match(const Genome& _other)
	{
		for (const auto& genome : genomes)
		{
			if (*genome == _other)
			{
				return genome;
			}
		}
		return nullptr;
	}

	///=============================================================================
	///	Instance methods
	///=============================================================================

	///=====================================
	/// Constructors and destructors
	///=====================================

	Genome::Genome(const std::vector<LayerDef>& _defs)
		:
		  id(id_generator.next()),
		  layers(_defs)
	{
		//		dlog("[Genome] Initialised genome with", layers.size(), "layers");
	}

	GenomePtr Genome::self()
	{
		return shared_from_this();
	}

	///=====================================
	/// Operators
	///=====================================

	bool Genome::operator == (const DNA& _dna) const
	{
		if (layers.size() != _dna.size())
		{
			return false;
		}

		for (uint layer = 0; layer < layers.size(); ++layer)
		{
			if (layers[layer] != _dna[layer]->get_def())
			{
				return false;
			}
		}
		return true;
	}

	bool Genome::operator == (const Genome& _other) const
	{
		if (layers.size() != _other.layers.size())
		{
			return false;
		}

		for (uint layer = 0; layer < layers.size(); ++layer)
		{
			if (layers[layer] != _other.layers[layer])
			{
				return false;
			}
		}
		return true;
	}

	///=====================================
	/// Evolution
	///=====================================

	MutationPtr Genome::mutate()
	{
		struct
		{
			hmap<bool, std::vector<GenomeIt>> fixed;
			hmap<LayerType, std::vector<GenomeIt>> type;
			hmap<uint, std::vector<GenomeIt>> nodes;
		} by;

		MutationPtr mutation(mksp<Mutation>());

//		dlog dmut("\n===[ Mutation ]===");
		for (auto it = layers.begin(); it < layers.end(); ++it)
		{
			by.fixed[it->fixed].emplace_back(it);
			by.type[it->type].emplace_back(it);
			by.nodes[it->nodes.size()].emplace_back(it);
		}

		if (by.fixed[false].empty())
		{
//			dmut << "\n>>> No evolvable layers.";

			/// No evolvable layers. Add one.
			mutation->element = ElemType::Layer;
			mutation->action = Action::Inc;
		}
		else
		{
			/// Compute the probability of adding
			/// a node as the deviation from the
			/// average number of nodes across
			/// the environment.
			real add_node_chance(1.0 - get_complexity());

//			dmut << "\n>>> Chance of adding a node:" << add_node;

			/// There are evolvable layers.
			/// The default element to add in this case is a node.
			/// However, there is an additional check to see if
			/// we should add a layer (with a node) instead.
			mutation->element = ElemType::Node;

			/// Determine whether we are adding or erasing a node.
			/// The probability of adding a node is inversely
			/// proportional to the genome's complexity.
			mutation->action = rnd_chance(add_node_chance) ? Action::Inc : Action::Dec;

			/// Check whether we should add	a new layer.
			/// This depends on the complexity of the
			/// genome and the total number of layers.
			real add_layer_chance(std::pow(add_node_chance, layers.size()));

//			dmut << "\n>>> Chance of adding a layer:" << add_layer;

			if (mutation->action == Action::Inc &&
				rnd_chance(add_layer_chance))
			{
				mutation->element = ElemType::Layer;
			}
		}

		/// Conditionally count the nodes while
		/// populating the roulette wheel.
		bool count_nodes(by.fixed[false].size() <= 2);

		/// Special check in case we are trying to remove
		/// a layer from a minimal network.
		if (count_nodes &&
			mutation->element == ElemType::Layer &&
			mutation->action == Action::Dec)
		{
			/// We have to have a minimum of
			/// two layers (input and output).
			return nullptr;
		}

		/// Populate the roulette wheel.
		Roulette<GenomeIt> wheel;
		for (auto it = layers.begin(); it < layers.end(); ++it)
		{
			if (/// In case of adding a layer, we are only
				/// trying to decide its type.
				/// Include all layers in the wheel.
				(mutation->element == ElemType::Layer &&
				 mutation->action == Action::Inc)
				||
				/// The layer is evolvable and either has
				/// two or more nodes or we we are not
				/// checking the node count.
				(!it->fixed &&
				 (it->nodes.size() > 1 || !count_nodes)))
			{
				wheel[it] = it->nodes.size();
			}
		}

		if (wheel.is_empty())
		{
			return nullptr;
		}

		//		dmut << "\nWheel size:" << wheel.elements.size();

		GenomeIt candidate(mutation->action == Action::Inc ? wheel.inverse_spin() : wheel.spin());

		/// Choose the layer to mutate.
		mutation->layer.index = candidate - layers.begin();

		//		dmut << "\nLayer index:" << mut.layer.index;

		/// Get the layer type from the selected layer.
		/// This is used when adding a layer to determine
		/// what type the new layer should be.
		mutation->layer.type = candidate->type;

		/// If we are removing the last node of a layer,
		/// remove the whole layer.
		if (mutation->element == ElemType::Node &&
			mutation->action == Action::Dec &&
			candidate->nodes.size() == 1)
		{
			mutation->element = ElemType::Layer;
		}

		if (mutation->element == ElemType::Layer &&
			mutation->action == Action::Inc)
		{
			mutation->layer.index = (by.type[candidate->type].back() - layers.begin()) + 1;
		}

		//		dmut << "\nMutation" << (mut ? "" : "un") << "successful:" << mut;

		return mutation;
	}

	void Genome::apply(const MutationPtr& _mutation)
	{
		/// Apply the mutation if it is valid
		if (_mutation)
		{
			/// Get the layer index
			uint idx(_mutation->layer.index);

			switch (_mutation->element)
			{
			case ElemType::Node:
				if (_mutation->action == Action::Inc)
				{
					layers[idx].nodes.emplace_back();
				}
				else
				{
					layers[idx].nodes.pop_back();
				}
				break;

			case ElemType::Layer:
				if (_mutation->action == Action::Inc)
				{
					layers.insert(layers.begin() + idx, {_mutation->layer.type, {}, true});
				}
				else
				{
					layers.erase(layers.begin() + idx);
				}
				break;

			default:
				break;
			}
		}
	}

	uint Genome::count(const ElemType _et)
	{
		switch (_et)
		{
		case ElemType::Layer:
			return layers.size();;

		case ElemType::Node:
			return Cortex::count(layers, [&](const uint _sum, const auto& _layer)
			{
				return _sum + _layer.nodes;
			});

		default:
			return 0;
		}
	}

	uint Genome::count(const LayerType _lt, const bool _with_fixed)
	{
		return Cortex::count(layers, [&](const uint _sum, const auto& _layer)
		{
			return _sum + (_lt == LayerType::Undef || _layer.type == _lt ? 1 : 0);
		});
	}

	real Genome::get_complexity()
	{
		dlog("Genome complexity:", Cortex::get_offset(ElemType::Node, count(ElemType::Node)));

		/// The complexity is computed as the deviation from
		/// the average number of nodes across the environment.
		return Cortex::get_offset(ElemType::Node, count(ElemType::Node));
	}
}
