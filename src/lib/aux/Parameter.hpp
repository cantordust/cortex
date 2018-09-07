#ifndef CORTEX_PARAM_HPP
#define CORTEX_PARAM_HPP

#include "Random.hpp"
#include "Stat.hpp"
#include "Conf.hpp"

namespace Cortex
{
	///=====================================
	/// @struct Generic class for parameters
	/// such as synaptic weight and membrane
	/// time constant.
	///=====================================
	struct Parameter
	{
		/// Parameter value
		real value = 0.0;

		/// Parameter statistics
		EMAStat stat;

		struct Mutation
		{
			/// SD for mutations
			real sd;

			/// Action to take at the next round of mutation
			Action action;

			void scale_sd(const Action _act);

			void set_sd(const real _new_sd);

		} mutation;

		Parameter(ParamConf& _conf, const real _value = 0.0);

		bool mutate(real& _value);

		void optimise(const Effect _effect);

		real initialise(const ParamConf& _conf);

		friend bool operator == (const Parameter& _lhs, const Parameter& _rhs);

		friend bool operator != (const Parameter& _lhs, const Parameter& _rhs);
	};
}

#endif // CORTEX_PARAM_HPP
