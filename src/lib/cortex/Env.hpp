#ifndef CORTEX_ENV_HPP
#define CORTEX_ENV_HPP

#include "Conf.hpp"

namespace Cortex
{
	class Env
	{

	protected:

		/// The phase that the environment is in.
		/// During the learning phase,
		/// networks optimise their parameters
		/// or add new connections,
		/// while in the evolution phase networks
		/// add new genes and participate in crossover.
		inline static Phase phase;

		/// Species.
		inline static hset<SpeciesPtr> species;

		/// Networks.
		inline static hset<NetPtr> nets;

		/// The fittest network.
		inline static Net* champ;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		/// @brief Default constructor.
		Env() = delete;

		/// @brief Copy constructor.
		Env(const Env& _other) = delete;

		/// @brief Move constructor.
		Env(Env&& _other) = delete;

		///=====================================
		/// Operators
		///=====================================

		/// @brief Copy assignment.
		Env& operator = (const Env& _other) = delete;

		/// @brief Move assignment.
		Env operator = (Env&& _other) = delete;

		///=====================================
		/// Initialisation
		///=====================================

		/// @brief Initialise the environment.
		static bool initialise();

		///=====================================
		/// Setters, getters and counters
		///=====================================

		/// @brief Get a species from a genome
		static Species* const get_species(const Genome& _genome);

		/// @brief Insert a new species into the environment
		///
		/// @param _genome Genome of the new species.
		static Species* const insert_species(const Genome& _genome);

		/// @brief Insert a new network into the environment
		static Net* const insert_net(const Genome& _genome);

		/// @brief Insert a new network into the environment
		/// as the offspring of two parent networks.
		///
		/// @param _p1 Parent 1
		/// @param _p2 Parent 2
		static Net* const insert_net(Net* const _p1, Net* const _p2);

		/// @brief Count elements of a specific type.
		static uint count(const ElemType _et = ElemType::Undef);

		/// @brief Count layers of a specific type.
		static uint count(const LayerType _lt = LayerType::Undef);

		/// @brief Count links of a specific type.
		static uint count(const LinkType _lt = LinkType::Undef);

		/// @brief Get the offset of the number of elements relative to
		/// the mean number of elements of that type.
		static real get_offset(const ElemType _elem, const uint _count);

		///=====================================
		/// Evolution
		///=====================================

		/// @brief Evolve networks.
		static void evolve();

		/// @brief Update the relative fitness values
		/// of networks and species.
		static void calibrate();

		/// @brief Cull empty species and unfit networks.
		static void cull();

		///=====================================
		/// Friends
		///=====================================

		friend class Task;
		friend class Species;
		friend class Net;
	};
}

#endif // CORTEX_ENV_HPP
