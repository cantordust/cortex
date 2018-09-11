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
		inline static real target{0.0};

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
		std::vector<rw<Parameter>> parameters;

		/// Effect of the last mutation.
		Effect effect = Effect::Undef;

	public:

		Fitness();

		/// @brief Add a parameter to the vector of parameters.
		void add_parameter(Parameter& _param);

		/// @brief Clear the vector of parameters.
		void clear_parameters();

		/// @brief Optimise the parameters currently stored
		/// in the vector of parameters.
		void set(const real _new_value);

		/// @brief Set the target fitness.
		static void set_target(const real _target);

		/// @brief Output the fitness to a stream.
		friend os& operator << (os& _os, const Fitness& _fitness);

		friend class Env;
		friend class Net;
		friend class Species;
	};
}

#endif // CORTEX_FITNESS_HPPS
