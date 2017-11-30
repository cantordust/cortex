#include "Species.hpp"
#include "Net.hpp"
#include "Functions.hpp"

namespace Cortex
{

	Species::Species(const uint _id,
					 const Genotype& _gen,
					 const ConfigRef& _cfg)
		:
		  id(_id),
		  cfg(_cfg),
		  genotype(_gen)
	{}

	inline void Species::add_net(Net& _net)
	{
		//		dlog() << "(Species) Adding network " << _net.id << " to species " << id;
		pop.nets.emplace(_net.id, _net);
	}

	// Erase a random network
	inline uint Species::erase_net()
	{
		if (pop.rank.unfit.size() > 0)
		{
			uint net_id(cfg.get().w_dist(pop.rank.unfit));
			erase_net(net_id);
			return net_id;
		}
		return 0;
	}

	// Erase a network by ID
	inline void Species::erase_net(const uint _id)
	{
		pop.nets.erase(_id);
		pop.rank.fit.erase(_id);
		pop.rank.unfit.erase(_id);
	}

	inline void Species::mutate()
	{
		for (const auto net_id : get_mutable_nets())
		{
			pop.nets.at(net_id).get().mutate();
		}
	}

	inline real Species::mating_chance()
	{
		return stats.fit.rel;
	}

	inline real Species::culling_chance()
	{
		return 1.0 - stats.fit.rel;
	}

	inline bool Species::operator ==(const Genotype& _gen)
	{
		return genotype == _gen;
	}

	inline bool Species::operator ==(const Species& _other)
	{
		return genotype == _other.genotype;
	}

	inline void Species::reset_stats()
	{
		stats.fit.reset();
		pop.elite.clear();
		pop.rank.fit.clear();
		pop.rank.unfit.clear();
		pop.fit.abs.reset();
		pop.fit.rel.reset();
	}

	inline std::vector<uint> Species::get_mutable_nets()
	{
		if (pop.rank.unfit.empty())
		{
			compute_fitness();
		}

		std::vector<uint> mutable_nets;
		for (const auto& n : pop.rank.unfit)
		{
			mutable_nets.emplace_back(n.first);
		}
		return mutable_nets;
	}

	inline real Species::compute_fitness()
	{
		reset_stats();
		compute_abs_net_fitness();
		compute_rel_net_fitness();

		for (const auto& net : pop.nets)
		{
			// Sum of the relative fitness values of individuals
			stats.fit.update(net.second.get().get_rel_fitness());
		}

		return stats.fit.abs;
	}

	inline real Species::get_abs_fitness() const
	{
		return stats.fit.abs;
	}

	inline real Species::get_progress() const
	{
		return stats.fit.get_progress();
	}

	inline real Species::get_rel_fitness() const
	{
		return stats.fit.rel;
	}

	inline void Species::set_rel_fitness(const real _rel_fit)
	{
		stats.fit.rel = _rel_fit;
	}

	inline bool Species::is_empty() const
	{
		return pop.nets.empty();
	}

	inline const auto& Species::get_genotype() const
	{
		return genotype;
	}

	inline uint Species::get_parent() const
	{
		if (pop.rank.fit.size() > 0)
		{
			return cfg.get().w_dist(pop.rank.fit);
		}
		return 0;
	}

	inline const auto& Species::get_genome() const
	{
		return genotype.get_genome();
	}

	void Species::compute_abs_net_fitness()
	{
		for (const auto& net : pop.nets)
		{
			pop.elite.push_back(net.first);

			real abs_fit(net.second.get().get_abs_fitness());
			pop.fit.abs.update(abs_fit);

			if (stats.champ_id == 0 ||
				abs_fit > pop.fit.abs.max)
			{
				stats.champ_id = net.second.get().id;
			}
		}

		if (cfg.get().mutation.elitism.enabled &&
			!pop.elite.empty())
		{
			// Sort the elite in descending order
			std::sort(pop.elite.begin(), pop.elite.end(), [&](const auto& _l, const auto& _r)
			{
				return pop.nets.at(_l).get().get_abs_fitness() > pop.nets.at(_r).get().get_abs_fitness();
			});

			// Remove extraneous individuals
			while (pop.elite.size() > std::floor(cfg.get().mutation.elitism.prop * pop.nets.size()))
			{
				pop.elite.pop_back();
			}
		}
	}

	void Species::compute_rel_net_fitness()
	{
		for (const auto& net : pop.nets)
		{
			// Compute the relative fitness
			real rel_fit(sigmoid(net.second.get().get_progress() *  (net.second.get().get_abs_fitness() - pop.fit.abs.mean) / (pop.fit.abs.sd == 0.0 ? 1.0 : pop.fit.abs.sd)));
			net.second.get().set_rel_fitness(rel_fit);
			pop.fit.rel.update(rel_fit);

			pop.rank.fit.emplace(net.first, rel_fit);
			pop.rank.unfit.emplace(net.first, 1.0 - rel_fit);

			dlog() << "Network " << net.second.get().id << ":"
				   << "\n\tabs fitness: " << net.second.get().get_abs_fitness()
				   << "\n\trel fitness: " << net.second.get().get_rel_fitness()
				   << "\n\tunfitness: " << pop.rank.unfit.at(net.second.get().id);
		}

		if (cfg.get().mutation.elitism.enabled &&
			!pop.elite.empty())
		{
			// Remove the elite from the unfitness table
			for (const auto& e : pop.elite)
			{
				pop.rank.unfit.erase(e);
			}
		}
	}

}
