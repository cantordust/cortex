#ifndef SPECIES_HPP
#define SPECIES_HPP

#include "Config.hpp"
#include "Genotype.hpp"

namespace Cortex
{
	class Species
	{

	public:

		// Species ID.
		uint id;

		ConfigRef cfg;

	private:

		// The genotype is shared by all networks belonging
		// to this species and can be used to check if a
		// species already exists.
		Genotype genotype;

		struct
		{
			// ID of the fittest network
			uint champ_id = 0;

			// Secies fitness
			Stat fit;
		} stats;

		struct
		{
			// Network population
			emap<uint, NetRef> nets;

			// Elite networks
			std::vector<uint> elite;

			struct
			{
				// Network ID => relative fitness
				emap<uint, real> fit;

				// Network ID => unfitness.
				emap<uint, real> unfit;
			} rank;

			struct
			{
				// Absolute fitness of the population
				Stat abs;

				// Relative fitness of the population
				Stat rel;
			} fit;
		} pop;

	public:

		explicit Species(const uint _id,
						 const Genotype& _gen,
						 const ConfigRef& _cfg);

		real compute_fitness();

		real get_abs_fitness() const;

		real get_progress() const;

		real get_rel_fitness() const;

		void set_rel_fitness(const real _rel_fit);

		bool is_empty() const;

		const auto& get_genome() const;

		const auto& get_genotype() const;

		uint get_parent() const;

		void add_net(Net& _net);

		uint erase_net();

		void erase_net(const uint _id);

		void mutate();

		real mating_chance();

		real culling_chance();

		bool operator == (const Species& _other);

		bool operator == (const Genotype& _gen);

	private:

		void reset_stats();

		std::vector<uint> get_mutable_nets();

		void compute_abs_net_fitness();

		void compute_rel_net_fitness();
	};
}
#endif // SPECIES_HPP
