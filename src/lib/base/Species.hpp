#ifndef SPECIES_HPP
#define SPECIES_HPP

#include "Config.hpp"

namespace Cortex
{
	class Genotype
	{
	private:

		emap<NR, uint> genome;

	public:

		Genotype(const emap<NR, uint>& _genome)
		{
			for (const auto& nr : Enum<NR>::entries)
			{
				if (_genome.find(nr.first) != _genome.end())
				{
					genome[nr.first] = _genome.at(nr.first);
				}
				else
				{
					genome[nr.first] = 0;
				}
			}
		}

		inline bool add(const NR _nr, const uint _count = 1)
		{
			if (genome.find(_nr) == genome.end())
			{
				genome.emplace(_nr, _count);
			}
			else
			{
				genome.at(_nr) += _count;
			}
			return true;
		}

		inline bool erase(const NR _nr, const uint _count = 1)
		{
			if (genome.find(_nr) == genome.end() ||
				genome.at(_nr) < _count)
			{
				return false;
			}
			genome.at(_nr) -= _count;
			return true;
		}

		inline uint count(const NR _nr)
		{
			return genome.at(_nr);
		}

		inline const auto& get_genome() const
		{
			return genome;
		}

		inline bool operator == (const Genotype& _other)
		{
			for (const auto& gene : genome)
			{
				if (_other.genome.find(gene.first) == _other.genome.end() ||
					gene.second != _other.genome.at(gene.first))
				{
					return false;
				}
			}
			return true;
		}
	};

	class Species
	{
	public:

		/// Species ID.
		uint id;

		Config& cfg;

	private:

		/// The genotype is shared by all networks belonging
		/// to this species and can be used to check if a
		/// species already exists.
		Genotype genotype;

		struct
		{
			/// ID of the fittest network
			uint champ_id = 0;

			/// Secies fitness
			Stat fit;
		} stats;

		struct
		{
			/// Network population
			emap<uint, NetRef> nets;

			/// Elite networks
			std::vector<uint> elite;

			struct
			{
				/// Network ID => relative fitness
				emap<uint, real> fit;

				/// Network ID => unfitness.
				emap<uint, real> unfit;
			} rank;

			struct
			{
				/// Absolute fitness of the population
				Stat abs;

				/// Relative fitness of the population
				Stat rel;
			} fit;
		} pop;

		inline void reset_stats()
		{
			stats.fit.reset();
			pop.elite.clear();
			pop.rank.fit.clear();
			pop.rank.unfit.clear();
			pop.fit.abs.reset();
			pop.fit.rel.reset();
		}

	public:

		explicit Species(const uint _id,
						 const Genotype& _genotype,
						 Config& _cfg);

		real compute_fitness();

		inline real get_abs_fitness() const
		{
			return stats.fit.abs;
		}

		inline real progress() const
		{
			return stats.fit.progress();
		}

		inline real get_rel_fitness() const
		{
			return stats.fit.rel;
		}

		inline void set_rel_fitness(const real _rel_fit)
		{
			stats.fit.rel = _rel_fit;
		}

		inline bool is_empty() const
		{
			return pop.nets.empty();
		}

		inline auto get_genome()
		{
			return genotype.get_genome();
		}

		inline auto get_genotype()
		{
			return genotype;
		}

		inline uint get_parent() const
		{
			if (pop.rank.fit.size() > 0)
			{
				return cfg.w_dist(pop.rank.fit);
			}
			return 0;
		}

		void add_net(Net& _net);

		uint erase_net();

		void erase_net(const uint _id);

		void mutate();

		inline real mating_chance()
		{
			return stats.fit.rel;
		}

		inline real culling_chance()
		{
			return 1.0 - stats.fit.rel;
		}

		inline bool operator == (const Species& _other)
		{
			return genotype == _other.genotype;
		}

		inline bool operator == (const Genotype& _gen)
		{
			return genotype == _gen;
		}

	private:

		std::vector<uint> get_mutable_nets();

		void compute_abs_net_fitness();

		void compute_rel_net_fitness();

		void compute_mean_age();
	};
}
#endif // SPECIES_HPP
