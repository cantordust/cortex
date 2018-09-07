#ifndef CORTEX_FITNESS_HPP
#define CORTEX_FITNESS_HPP

#include "Parameter.hpp"

namespace Cortex
{
	/// @class Generic fitness class.
	/// It takes an arbitrary number of parameters
	/// and optimises them based on the computed fitness.
	class Fitness
	{

	protected:

		/// @brief Target fitness.
		static inline real target{0.0};

		/// Absolute fitness.
		EMAStatPack abs;

		/// Relative fitness (set externally).
		/// For networks, the relative fitness is set by
		/// comparing the absolute fintess values for all
		/// networks in the same species.
		/// For species, it is set by comparing the overall
		/// absolute fitness values of all species.
		EMAStatPack rel;

		/// Parameters being mutated / optimised.
		std::vector<rw<Parameter>> params;

		/// Effect of the last mutation.
		Effect effect = Effect::Undef;

	public:

		Fitness();

		void add_param(Parameter& _p);

		void optimise(const real _new_value);

		static void set_target(const real _target);

		/// @brief Output the fitness to a stream.
		friend os& operator << (os& _os, const Fitness& _fitness);

		friend class Env;
		friend class Net;
		friend class Species;
	};
}

#endif // CORTEX_FITNESS_HPPS
