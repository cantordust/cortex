#ifndef CORTEX_PARAM_HPP
#define CORTEX_PARAM_HPP

#include "Globals.hpp"
#include "ParamConf.hpp"
#include "Stat.hpp"

namespace Cortex
{
	/// @class Param Generic class for parameters such as
	/// synaptic weight and node time constant.
	class Param
	{
	private:

		ParamConf& pc;

		/// Parameter statistics
		Stat stat;

		/// SD for mutation
		real sd;

		/// Action to take at the next mutation event
		Act act;

		/// Indicates how important this parameter is
		/// in terms of contribution to the fitness.
		/// @todo Use in crossover
		real importance;

		friend class Fitness;
		friend class Link;
		friend class Node;

	public:

		Param(ParamConf& _pc);

		Param(const Param& _other);

		void init();

		void update(const real _new_val);

		real val() const;

		void inc_sd();

		void dec_sd();

		bool mutate(Fitness& _fit);

		void flip_act();

		void set_trend(const Eff _eff);

		void anneal(const real _abs_fit);

	private:

		template<Opt opt>
		void optimise(const Fitness& _fit);

	};
}

#endif // CORTEX_PARAM_HPP
