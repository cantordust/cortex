#ifndef CORTEX_SPECIES_HPP
#define CORTEX_SPECIES_HPP

#include "Genome.hpp"
#include "Fitness.hpp"
#include "Stat.hpp"

namespace Cortex
{
	class Species : public std::enable_shared_from_this<Species>
	{
	public:

		/// Species ID.
		/// This is generated automatically.
		const uint id;

	protected:

		/// The genome is shared by all networks belonging
		/// to this species and can be used to check if a
		/// species already exists.
		Genome genome;

		/// Overall fitness of the species.
		Fitness fitness;

		/// Networks sorted in order of decreasing fitness.
		std::vector<Net*> nets;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		/// @brief Default constructor.
		Species() = delete;

		/// @brief Copy constructor.
		Species(const Species& _other) = delete;

		/// @brief Move constructor.
		Species(Species&& _other) = delete;

		/// @brief Explicit constructor taking a genome.
		explicit Species(const Genome& _gen);

		///=====================================
		/// Bookkeeping
		///=====================================

		/// @brief Function providing consecutive ID values for species.
		static uint get_id(const bool _reset = false);

		/// @brief Check if the species contains any networks.
		bool is_empty() const;

		///=====================================
		/// Operators
		///=====================================

		/// @brief Equality test operator taking a genome.
		bool operator == (const Genome& _genome) const;

		///=====================================
		/// Setters, getters and counters
		///=====================================

		/// @brief Get the offset of the number of elements
		/// relative to the mean number of elements
		/// of that type for this species.
		real get_offset(const ElemType _elem, const uint _count) const;

		/// @brief Add a network to this species.
		void add(Net* const _net);

		/// @brief Erase a network from the species.
		void erase(Net* const _net);

		Net* const get_fit_net() const;

		Net* const get_unfit() const;

//		real crossover_chance();

//		real culling_chance();

		///=====================================
		/// Evolution
		///=====================================

		/// @brief Compute the relative fitness
		/// of all networks in this species.
		Net* const calibrate();

		/// @brief Evolve all networks in the species.
		void evolve();

		///=====================================
		/// Friends
		///=====================================

		friend class Env;
		friend class Net;
		friend class Genome;
	};
}
#endif // CORTEX_SPECIES_HPP
