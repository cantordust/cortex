#include "Env.hpp"
#include "Conf.hpp"
#include "Species.hpp"
#include "Net.hpp"

namespace Cortex
{
	///=====================================
	/// Constructors and destructors
	///=====================================

	Species::Species(const Genome& _genome)
		:
		  id(get_id()),
		  genome(_genome)
	{}

	///=====================================
	/// Bookkeeping
	///=====================================

	uint Species::get_id(const bool _reset)
	{
		static uint id(0);
		if (_reset)
		{
			id = 0;
			return id;
		}
		Env::stats[Stat::Species].add(1);
		return ++id;
	}

	bool Species::is_empty() const
	{
		return nets.empty();
	}

	///=====================================
	/// Operators
	///=====================================

	bool Species::operator == (const Genome& _genome) const
	{
		return (!conf->species.enabled) ? true : genome == _genome;
	}

	///=====================================
	/// Setters, getters and counters
	///=====================================

	real Species::get_offset(const ElemType _elem, const uint _count) const
	{
		SMAStat stat;
		for (const auto& net : nets)
		{
			stat.update(net->count(_elem));
		}
		return stat.get_offset(_count);
	}

	void Species::add(Net* const _net)
	{
		if (!_net)
		{
			die("Trying to add a non-existent network to species ", id, ", exiting");
		}
		nets.emplace_back(_net);
	}

	void Species::erase(Net* const _net)
	{
		if (!_net)
		{
			die("Trying to erase a nonexistent network from species ", id, ", exiting");
		}

		for (auto it = nets.begin(); it != nets.end(); )
		{
			if (*it == _net)
			{
				nets.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	Net* const Species::get_fit_net() const
	{
		return nullptr;
	}

	Net* const Species::get_unfit() const
	{
		return nullptr;
	}

	///=====================================
	/// Evolution
	///=====================================

	Net* const Species::calibrate()
	{
		if (nets.empty())
		{
			return nullptr;
		}

		SMAStatPack net_stat;

		real max_fit(min<real>);

		/// Compute the absolute fitness of the species
		for (const auto& net : nets)
		{
			real abs_fit(net->fitness.abs.value);
			nets.push_back(net);

			if (abs_fit > max_fit)
			{
				max_fit = abs_fit;
			}

			net_stat.update(abs_fit);
		}

		/// Sort the networks in order of decreasing fitness
		std::sort(nets.begin(), nets.end(), [](Net* const _lhs, Net* const _rhs)
		{
			return _lhs->fitness.abs.value > _rhs->fitness.abs.value;
		});

		/// Compute the relative fitness of the networks
		/// belonging to this species
		for (auto& net : nets)
		{
			/// Compute the relative fitness
			real rel_fit(net->fitness.abs.get_offset());

			net->fitness.rel.update(rel_fit);

			dlog("Network ", net->id, " fitness:",
				 "\n\tAbsolute: ", net->fitness.abs.value,
				 "\n\tRelative: ", net->fitness.rel.value);
		}

		fitness.abs.update(net_stat.value);

		/// Return the champion for this species
		return nets.front();
	}

	void Species::evolve()
	{
		/// Map of nets and corresponding
		/// relative fitness values for
		/// selecting parents for crossover.
		wmap<Net*> xover_wheel;
		for (const auto& net : nets)
		{
			xover_wheel[net] = net->fitness.rel.value;
		}

		/// @note It is important to iterate over a copy
		/// of the population (conveniently provided by
		/// the crossover wheel) because if the network's
		/// structure is mutated and speciation is enabled,
		/// it will be removed from this species and the
		/// iterators will become invalid.
		for (const auto& net : xover_wheel.elements)
		{
			/// The probability of this network participating
			/// in crossover depends on its relative fitness.
			if (rnd_chance(net->fitness.rel.value))
			{
				/// Crossover.
				Env::insert_net(net, xover_wheel.spin());
			}
			else
			{
				/// Structural mutation.
				net->evolve();
			}
		}
	}
}
