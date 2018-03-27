#include "Env.hpp"
#include "Conf.hpp"
#include "Species.hpp"
#include "Net.hpp"

namespace Cortex
{

	Species::Species(const uint _id,
					 const Genotype& _gen,
					 Env& _env)
		:
		  id(_id),
		  conf(_env.conf),
		  env(_env),
		  genotype(_gen),
		  fitness(_env.conf)
	{}

	void Species::add_net(Net& _net)
	{
		dlog() << "(Species) Adding network " << _net.id << " to species " << id;
		nets.emplace(_net.id, std::ref(_net));
	}

	/// Erase a random network
	uint Species::erase_net()
	{
		if (unfit.empty())
		{
			return 0;
		}
		uint net_id(conf.w_dist(unfit));
		erase_net(net_id);
		return net_id;
	}

	/// Erase a network by ID
	void Species::erase_net(const uint _net_id)
	{
		nets.erase(_net_id);
		fit.erase(_net_id);
		unfit.erase(_net_id);
		for (auto it = elite.begin(); it != elite.end(); )
		{
			if (*it == _net_id)
			{
				elite.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
		if (champ == _net_id)
		{
			champ = 0;
		}
	}

	void Species::mutate()
	{
		reset_stats();
		update_fitness();

		for (const auto& net : get_mutable_nets())
		{
			nets.at(net.first).get().mutate();
		}
	}

	real Species::mating_chance()
	{
		return fitness.rel.last;
	}

	real Species::culling_chance()
	{
		return 1.0 - fitness.rel.last;
	}

	bool Species::operator == (const Genotype& _gen)
	{
		return genotype == _gen;
	}

	bool Species::operator == (const Species& _other)
	{
		return genotype == _other.genotype;
	}

	void Species::reset_stats()
	{
		champ = 0;
		fit.clear();
		unfit.clear();
		elite.clear();
	}

	real Species::update_fitness()
	{
		Stat net_stat(MA::SMA);

		/// Compute the absolute fitness of the species
		for (const auto& net : nets)
		{
			elite.push_back(net.first);

			real fit(net.second.get().fitness.abs.last);
			if (champ == 0 ||
				fit > net_stat.max)
			{
				champ = net.second.get().id;
			}

			net_stat.update(fit);
		}

		/// Generate the bourgeoisie
		if (conf.mut.elite > 0.0 &&
			!elite.empty())
		{
			/// Sort the elite in descending order
			std::sort(elite.begin(), elite.end(), [&](uint _lhs, uint _rhs)
			{
				return nets.at(_lhs).get().fitness.abs.last > nets.at(_rhs).get().fitness.abs.last;
			});

			/// Remove mediocre individuals
			while (elite.size() > std::floor(conf.mut.elite * nets.size()))
			{
				elite.pop_back();
			}
		}

		/// Compute the relative fitness of the networks
		/// belonging to this species
		for (const auto& net : nets)
		{
			/// Compute the relative fitness
			real rel_fit( Logistic( net.second.get().fitness.abs.get_offset() ) *
						  Logistic( net_stat.get_offset( net.second.get().fitness.abs.last ) ) );

			net.second.get().fitness.rel.update(rel_fit);

			fit.emplace(net.first, rel_fit);
			unfit.emplace(net.first, 1.0 - rel_fit);

			dlog() << "Network " << net.second.get().id << ":"
				   << "\n\tabs fitness: " << net.second.get().fitness.abs.last
				   << "\n\trel fitness: " << net.second.get().fitness.rel.last
				   << "\n\tunfitness: " << unfit.at(net.second.get().id);
		}

		if (conf.mut.elite > 0.0)
		{
			/// Remove elite nets from the unfitness table
			for (const auto& e : elite)
			{
				unfit.erase(e);
			}
		}

		fitness.abs.update(net_stat.mean);

		return fitness.abs.mean;
	}

	bool Species::is_empty() const
	{
		return nets.empty();
	}

	const Genotype& Species::get_genotype() const
	{
		return genotype;
	}

	hmap<uint, real> Species::get_mutable_nets() const
	{
		return unfit;
	}

	uint Species::get_rnd_parent() const
	{
		return (fit.size() > 0) ? conf.w_dist(fit)
								: 0;
	}

	const hmap<NR, uint>& Species::get_genome() const
	{
		return genotype.get_genome();
	}
}
