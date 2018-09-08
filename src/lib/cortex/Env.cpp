#include "Task.hpp"
#include "Env.hpp"
#include "Conf.hpp"
#include "Species.hpp"
#include "Net.hpp"

namespace Cortex
{
	///=====================================
	/// Initialisation
	///=====================================

	bool Env::initialise()
	{
		dlog("---> Initialising environment...");

		///=================
		/// Reset everything
		///=================

		/// Set the phase to undefined.
		phase = Phase::Undef;

		/// Reset the champion.
		champ = nullptr;

		/// Reset static checkers and factory methods.
		Task::is_solved(nullptr, true);
		Species::get_id(true);
		Net::get_id(true);

		/// Clear the species and network containers.
		nets.clear();
		species.clear();

		///=================
		/// Generate species.
		///=================

		/// Population size (the number of networks per species).
		uint net_quota(conf->net.init.count / conf->species.init.count);

		/// Proto-genome.
		Genome genome;

		/// Generate proto-species and proto-nets
		while (species.size() < conf->species.init.count)
		{
			auto proto_species(insert_species(genome));
//			dlog(">>> Species", proto_species->id, "inserted.");

			if (!proto_species)
			{
				dlog("Environment initialisation failed: error creating species.");
				return false;
			}

			/// Add the proto-nets for this species.
			for (uint n = 0; n < net_quota; ++n)
			{
				auto proto_net(insert_net(genome));
				if (!proto_net)
				{
					dlog("Environment initialisation failed: error creating network.");
					return false;
				}
			}

			/// Mutate the initial genome to get a different species.
			Mutation mut;

			do
			{
//				dlog(">>> Waiting for mutation...");

				/// Mutate the genome until we get a valid mutation.
				while (!(mut = genome.mutate()));

				/// Check if a species with the same genome exists.
				/// If it does, start over.
			} while (get_species(genome));

//			dlog{mut};

			/// Break after one iteration
			/// if speciation is disabled.
			if (!conf->species.enabled)
			{
				break;
			}
		}

		///=================
		/// Final sanity check.
		///=================

		if (nets.empty() ||
			species.empty())
		{
			dlog("Environment initialisation failed.");
			return false;
		}

		dlog("Environment initialised with", nets.size(), "networks and", species.size(), "species.");
		return true;
	}

	///=====================================
	/// Setters, getters and counters
	///=====================================

	Species* const Env::get_species(const Genome& _genome)
	{
		if (!conf->species.enabled)
		{
			/// If speciation is disabled, return nothing if there
			/// are no species or the first one if there are.
			return species.empty() ? nullptr : species.begin()->get();
		}

		/// First, check if the species exists.
		for (const auto& spc : species)
		{
			if (spc->genome == _genome)
			{
				return spc.get();
			}
		}

		return nullptr;
	}

	Species* const Env::insert_species(const Genome& _genome)
	{
		/// If speciation is disabled, there will be only one species.
		/// Just return that.
		if (!conf->species.enabled &&
			!species.empty())
		{
			return species.begin()->get();
		}

		/// If the species doesn't exist, check if we can add it.
		if (conf->species.max.count > 0 &&
			species.size() >= conf->species.max.count)
		{
			/// The species doesn't exist and we can create it.
			/// Insert the new species into the environment.
			return nullptr;
		}

		auto it(species.emplace(mksp<Species>(_genome)));
		return it.second ? it.first->get() : nullptr;
	}

	Net* const Env::insert_net(const Genome& _genome)
	{
		auto it(nets.emplace(mksp<Net>(_genome)));
		return (it.second) ? it.first->get() : nullptr;
	}

	Net* const Env::insert_net(Net* const _p1, Net* const _p2)
	{
		auto it(nets.emplace(mksp<Net>(_p1, _p2)));
		return (it.second) ? it.first->get() : nullptr;
	}

	real Env::get_offset(const ElemType _elem, const uint _count)
	{
		SMAStat stat;
		for (const auto& net : nets)
		{
			stat.update(net->count(_elem));
		}

//		dlog("[Env::get_offset()] offset for", _elem, ":", stat.get_offset(_count));

		return stat.get_offset(_count);
	}

	///=====================================
	/// Evolution
	///=====================================

	void Env::evolve()
	{
		dlog d(">>> Evolving environment");
		/// Increase the age of all networks.
		for (const auto& net : nets)
		{
			net->increase_age();
		}

		/// Compute the relative fitness of networks and species.
		d << "\t`-> Calibrating...";
		calibrate();

		/// Evolve networks in each species.
		d << "\t`-> Evolving networks...";
		for (const auto& spc : species)
		{
			spc->evolve();
		}

		/// Eliminate unfit networks and empty species.
		d << "\t`-> Culling...";
		cull();
	}

	void Env::calibrate()
	{
		/// A statistics package for collecting
		/// species statistics on the fly.
		SMAStatPack spc_stat;

		/// Reset the champion.
		champ = nullptr;

		/// Highest fitness seen so far.
		real top_fitness(min<real>);

		for (const auto& spc : species)
		{
			/// Compute the relative network fitness
			/// and the absolute species fitness.
			Net* const species_champ(spc->calibrate());

			if (!champ ||
				(species_champ &&
				 species_champ->fitness.abs.value > top_fitness))
			{
				champ = species_champ;
				top_fitness = species_champ->fitness.abs.value;
			}
		}

		for (const auto& spc : species)
		{
			/// Compute the relative species fitness
			spc->fitness.rel.update( spc->fitness.abs.get_offset() );

			dlog("\tSpecies ", spc->id, ":",
				 "\n\t\tAbsolute fitness: ", spc->fitness.abs.value,
				 "\n\t\tRelative fitness: ", spc->fitness.rel.value);
		}
	}

	void Env::cull()
	{
		/// Net cemetery.
		hset<NetPtr> cemetery;

		/// Network age statistics.
		SMAStat age_stat;
		for (const auto& net : nets)
		{
			age_stat.update(net->age);
		}

		for (const auto& net : nets)
		{
			/// The probabiliy of culling a network is directly
			/// proportional to its age and inversely proportional
			/// to the relative fitness of the network and its species.
			///
			/// @note Using tanh instead of the default logistic
			///	function protects new offspring as their age is 0.
			real cull_prob(age_stat.get_offset<Func::Tanh>(net->age)
						   * (1.0 - net->fitness.rel.value)
						   * (1.0 - net->species->fitness.rel.value));

			if (rnd_chance(cull_prob))
			{
				cemetery.emplace(net);
			}
		}

		/// Erase culled networks.
		while (!cemetery.empty())
		{
			auto net(*cemetery.begin());
			dlog("Erasing network ", net->id);

			cemetery.erase(net);
			nets.erase(net);
		}

		/// Remove extinct species.
		for (auto it = species.begin(); it != species.end(); )
		{
			(*it)->is_empty() ? it = species.erase(it) : ++it;
		}
	}
}
