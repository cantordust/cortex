#ifndef CORTEX_GENOME_HPP
#define CORTEX_GENOME_HPP

#include "Globals.hpp"

namespace Cortex
{
	struct Genome
	{
		/// Chromosomes (layer descriptions).
		std::vector<LayerConf> layers;

		///=====================================
		/// Constructors and destructors
		///=====================================

		/// @brief Default constructor.
		Genome();

		/// @brief Copy constructor.
		Genome(const Genome& _other);

		///=====================================
		/// Operators
		///=====================================

		/// @brief Equality test operator
		bool operator == (const Genome& _other) const;

		///=====================================
		/// Evolution
		///=====================================

		/// @brief Add or erase chromosomes (layer descriptions)
		/// or genes (nodes).
		Mutation mutate();

		/// @brief Computes the complexity of the genome.
		real complexity();

		///=====================================
		/// Friends
		///=====================================

		friend class Species;
		friend class Net;
	};
}

#endif // CORTEX_GENOME_HPP
