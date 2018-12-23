#ifndef CORTEX_GENOME_HPP
#define CORTEX_GENOME_HPP

#include "ID.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	///=====================================
	/// @struct A network genome
	///=====================================
	struct Genome : public std::enable_shared_from_this<Genome>
	{
	public:

		///=============================================================================
		///	Static attributes
		///=============================================================================

		/// Species as a collection of genomes.
		inline static hset<GenomePtr> genomes;

		/// ID factory
		inline static IDGenerator id_generator;

		///=============================================================================
		///	Instance attributes
		///=============================================================================

		/// Genome ID
		const uint id{0};

		/// Overall fitness of the genome.
		Fitness fitness;

		/// Networks sharing this genome.
		hset<NetPtr> nets;

		/// Chromosomes (layer definitions).
		std::vector<LayerDef> layers;

	public:

		///=============================================================================
		///	Static methods
		///=============================================================================

		/// @brief Create a new genome.
		static GenomePtr create(const std::vector<LayerDef>& _def = {});

		/// @brief Clone an existing genome.
		static GenomePtr create(const Genome& _other);

		/// @brief Check if a DNA strand matches an existing genome.
		static GenomePtr match(const DNA& _dna);

		/// @brief Check if a genome matches another existing genome.
		static GenomePtr match(const Genome& _other);

		///=============================================================================
		///	Instance methods
		///=============================================================================

		/// @brief Get a shared pointer to this genome
		/// from within the genome.
		GenomePtr self();

		///=====================================
		/// Operators
		///=====================================

		/// @brief Equality test operator taking a DNA reference
		bool operator == (const DNA& _dna) const;

		/// @brief Equality test operator taking
		/// a reference to another genome.
		bool operator == (const Genome& _other) const;

		///=====================================
		/// Evolution
		///=====================================

		/// @brief Add or erase chromosomes (layers)
		/// or genes (nodes).
		MutationPtr mutate();

		/// @brief Apply an arbitrary mutation.
		void apply(const MutationPtr& _mutation);

		/// @brief Computes the complexity of the genome.
		real get_complexity();

		///=====================================
		/// Setters, getters and counters
		///=====================================

		/// @brief Get the offset of the number of elements
		/// relative to the mean number of elements
		/// of that type for this species.
		real get_offset(const ElemType _elem, const uint _count) const;

		NetPtr get_fit_net() const;

		NetPtr get_unfit() const;

		/// @brief Count the number of elements of type @p _et,
		/// optionally including non-evolvable layers.
		uint count(const ElemType _et);

		/// @brief Count the number of layers type @p _lt,
		/// optionally including non-evolvable layers.
		uint count(const LayerType _lt, const bool _with_fixed = false);

//		real crossover_chance();

//		real culling_chance();

		///=====================================
		/// Evolution
		///=====================================

		/// @brief Compute the relative fitness
		/// of all networks with this genome.
		NetPtr calibrate();

		/// @brief Evolve all networks in the species.
		void evolve();

	protected:

		///=====================================
		/// Constructors and destructors
		///=====================================

		/// @brief Default constructor.
		Genome(const std::vector<LayerDef>& _def = {});

		/// @brief Copy constructor.
		Genome(const Genome& _other) = delete;

		/// @brief Move constructor.
		Genome(Genome&& _other) = delete;
	};
}

#endif // CORTEX_GENOME_HPP
