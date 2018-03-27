#ifndef CORTEX_SPECIES_HPP
#define CORTEX_SPECIES_HPP

#include "Genotype.hpp"
#include "Fitness.hpp"

namespace Cortex
{
	class Species
	{

	public:

		uint id;

		Conf& conf;

	private:

		Env& env;

		/// The genotype is shared by all networks belonging
		/// to this species and can be used to check if a
		/// species already exists.
		Genotype genotype;

		/// @brief Fitness of the species
		Fitness fitness;

		/// ID of the fittest network
		uint champ = 0;

		/// @brief Network population.
		hmap<uint, NetRef> nets;

		/// Network ID | relative fitness
		hmap<uint, real> fit;

		/// Network ID | relative unfitness.
		hmap<uint, real> unfit;

		/// Elite networks
		std::vector<uint> elite;

	public:

		explicit Species(const uint _id,
						 const Genotype& _gen,
						 Env& _env);

		real update_fitness();

		bool is_empty() const;

		const hmap<NR, uint>& get_genome() const;

		const Genotype& get_genotype() const;

		hmap<uint, real> get_mutable_nets() const;

		uint get_rnd_parent() const;

		void add_net(Net& _net);

		uint erase_net();

		void erase_net(const uint _net_id);

		void mutate();

		real mating_chance();

		real culling_chance();

		bool operator == (const Species& _other);

		bool operator == (const Genotype& _gen);

	private:

		void reset_stats();

		friend class Env;
	};
}
#endif // CORTEX_SPECIES_HPP
