#ifndef PARAM_HPP
#define PARAM_HPP

#include "Config.hpp"

namespace Cortex
{
	// Generic parameter for parameters such as
	// synaptic weight and node time constant.

	class Fitness;

	class Param
	{
	public:

		ConfigRef cfg;

	private:

		// Parameter statistics
		Stat stat;

		// Parameter configuration
		ParamConf pconf;

		// Action to take at the next mutation
		Act act;

		// Indicates how important this parameter is
		// in terms of contribution to the fitness.
		// @todo Use in crossover
		real importance;

		friend class Fitness;

	public:

		Param(const ConfigRef& _cfg, const ParamConf& _pconf);

		const Stat& get_stat() const;

		void set(const real _cur);

		void update(const real _delta);

		real cur() const;

		void inc_sd();

		void dec_sd();

		bool mutate(Fitness& _fit);

		void flip_act();

		void set_trend(const Eff _eff);

		void anneal(const real _abs_fit);

	};
}

#endif // PARAM_HPP
