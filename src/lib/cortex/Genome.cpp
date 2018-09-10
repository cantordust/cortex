#include "Conf.hpp"
#include "Env.hpp"
#include "Species.hpp"
#include "Genome.hpp"
#include "Layer.hpp"

namespace Cortex
{
	///=============================================================================
	///	Helper functions
	///=============================================================================

	///=====================================
	/// Count the number of evolvable layers
	/// or nodes inside evolvable layers.
	///
	/// @note By default, this function looks at
	/// evolvable layers only. Fixed layers	can be
	/// included by setting @p _with_fixed to true.
	///=====================================

	template<ElemType et, LayerType lt = LayerType::Undef>
	uint count(const Genome& _genome, const bool _with_fixed = false)
	{
		return Cortex::count(_genome.layers, [&](const uint _sum, const auto& _layer)
		{
			if ((_layer.evolvable ||
				 _with_fixed)
				 &&
				(lt == LayerType::Undef ||
				 lt == _layer.type))
			{
				switch (et)
				{
				case ElemType::Layer:
					return _sum + 1;

				case ElemType::Node:
					return _sum + _layer.nodes;

				default:
					return _sum;
				}
			}
			return _sum;
		});
	}

	///=====================================
	/// Functions to find the first, last and
	/// n-th layer of the specified type.
	///=====================================

	opt<uint> first(const Genome& _genome, const LayerType _lt)
	{
		for (uint c = 0; c < _genome.layers.size(); ++c)
		{
			if (_genome.layers[c].type == _lt)
			{
				return c;
			}
		}
		return nopt;
	}

	opt<uint> last(const Genome& _genome, const LayerType _lt)
	{
		for (uint c = _genome.layers.size(); c-- > 0; )
		{
			if (_genome.layers[c].type == _lt)
			{
				return c;
			}
		}
		return nopt;
	}

	opt<uint> nth(const Genome& _genome, const LayerType _lt, const uint _n)
	{
		uint found(0);
		for (uint c = 0; c < _genome.layers.size(); ++c)
		{
			if (_genome.layers[c].type == _lt)
			{
				if (found == _n)
				{
					return c;
				}
				++found;
			}
		}
		return nopt;
	}

	opt<uint> insert_pos(const Genome& _genome, const LayerType _lt)
	{
		return (_lt == LayerType::Regular ? last(_genome, LayerType::Regular)
										  : first(_genome, LayerType::Regular));
	}

	///=============================================================================
	///	Member functions
	///=============================================================================

	///=====================================
	/// Constructors and destructors
	///=====================================

	Genome::Genome()
	{
		for (const auto& layer : conf->net.init.layers)
		{
			layers.emplace_back(layer);
		}

//		dlog("[Genome] Initialised genome with", layers.size(), "layers");
	}

	Genome::Genome(const Genome& _other)
		:
		  layers(_other.layers)
	{}

	///=====================================
	/// Operators
	///=====================================

	bool Genome::operator == (const Genome& _other) const
	{
		if (layers.size() != _other.layers.size())
		{
			return false;
		}

		for (uint i = 0; i < layers.size(); ++i)
		{
			if (layers[i] != _other.layers[i])
			{
				return false;
			}
		}

		return true;
	}

	///=====================================
	/// Evolution
	///=====================================

	Mutation Genome::mutate()
	{
		Mutation mut;

//		dlog dmut("\n===[ Mutation ]===");

		/// Check if there are any evolvable layers at all.
		if (count<ElemType::Layer>(*this) == 0)
		{
//			dmut << "\n>>> No evolvable layers.";

			/// No evolvable layers. Add one.
			mut.element = ElemType::Layer;
			mut.action = Action::Inc;
		}
		else
		{
			/// Compute the probability of adding
			/// a node as the deviation from the
			/// average number of nodes across
			/// the environment.
			real add_node(1.0 - complexity());

//			dmut << "\n>>> Chance of adding node:" << add_node;

			/// There are evolvable layers.
			/// The default element to add in this case is a node.
			/// However, there is an additional check to see if
			/// we should add a layer (with a node) instead.
			mut.element = ElemType::Node;

			/// Determine whether we are adding or erasing a node.
			/// The probability of adding a node is inversely
			/// proportional to the genome's complexity.
			mut.action = rnd_chance(add_node) ? Action::Inc : Action::Dec;

			/// Check whether we should add	a new layer.
			/// This depends on the complexity of the
			/// genome and the total number of layers.

			real add_layer(std::pow(add_node, layers.size()));

//			dmut << "\n>>> Chance of adding layer:" << add_layer;

			if (mut.action == Action::Inc &&
				rnd_chance(add_layer))
			{
				mut.element = ElemType::Layer;
			}
		}

		/// Conditionally count the nodes while
		/// populating the roulette wheel.
		bool count_nodes(layers.size() <= 2);

		/// Special check in case we are trying to remove
		/// a layer from a minimal network.
		if (count_nodes &&
			mut.element == ElemType::Layer &&
			mut.action == Action::Dec)
		{
			/// We have to have a minimum of
			/// two layers (input and output).
			return Mutation();
		}

		/// Populate the roulette wheel.
		wmap<uint> wheel;
		uint index(0);
		for (const auto& layer : layers)
		{
			if (/// In case of adding a layer, we are only
				/// trying to decide its type.
				/// Include all layers in the wheel.
				(mut.element == ElemType::Layer &&
				 mut.action == Action::Inc)
				||
				/// The layer is evolvable and either has
				/// two or more nodes or we we are not
				/// checking the node count.
				(layer.evolvable &&
				 (layer.nodes > 1 || !count_nodes)))
			{
				wheel[index] = layer.nodes;
			}
			++index;
		}

		if (wheel.is_empty())
		{
			return Mutation();
		}

//		dmut << "\nWheel size:" << wheel.elements.size();

		/// Choose the layer to mutate.
		/// This is inversely proportional
		/// to the number of nodes in the layer.
		mut.layer.index = mut.action == Action::Inc ? wheel.flip_spin()
													: wheel.spin();

//		dmut << "\nLayer index:" << mut.layer.index;

		/// Get the layer type from the selected layer.
		/// This is used when adding a layer to determine
		/// what type the new layer should be.
		mut.layer.type = layers[mut.layer.index()].type;

//		dmut << "\nLayer type:" << mut.layer.type;

		///=================
		/// Final sanity checks.
		///=================

		/// If we are removing the last node of a layer,
		/// remove the whole layer.
		if (mut.element == ElemType::Node &&
			mut.action == Action::Dec &&
			layers[mut.layer.index()].nodes == 1)
		{
			mut.element = ElemType::Layer;
		}

		if (mut.element == ElemType::Layer &&
			mut.action == Action::Inc)
		{
			/// In case of adding a convolutional layer,
			/// check if its response would have a reasonable
			/// size after pooling.
			if (mut.layer.type == LayerType::Convolutional &&
				count<ElemType::Layer, LayerType::Convolutional>(*this) >= conf->data.image.conv_layer_limit())
			{
				return Mutation();
			}
			mut.layer.index = insert_pos(*this, mut.layer.type);
		}

		/// Apply the mutation if it is valid
		if (mut)
		{
			/// Shorthand for the layer index
			uint idx(mut.layer.index());

			switch (mut.element)
			{
			case ElemType::Node:
				mut.action == Action::Inc  ? ++layers[idx].nodes
										   : --layers[idx].nodes;
				break;

			case ElemType::Layer:
				mut.action == Action::Inc ? layers.insert(layers.begin() + idx, {true, mut.layer.type, 1})
										  : layers.erase(layers.begin() + idx);
				break;

			default:
				break;
			}
		}

//		dmut << "\nMutation" << (mut ? "" : "un") << "successful:" << mut;

		return mut;
	}

	real Genome::complexity()
	{
//		dlog("Genome complexity:", Env::get_offset(ElemType::Node, count<ElemType::Node>(*this, true)));

		/// The complexity is computed as the deviation from
		/// the average number of nodes across the environment.
		return Env::get_offset(ElemType::Node, count<ElemType::Node>(*this, true));
	}
}
