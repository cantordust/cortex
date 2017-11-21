#ifndef PARAM_HPP
#define PARAM_HPP

#include "Config.hpp"

namespace Cortex
{
	/// Generic parameter for parameters
	/// such as weight, firing threshold,
	/// delay and time constant.

	class Fitness;

	class Param
	{
	private:

		Config& cfg;

		Config::ParamCfg stats;

		Act act;

		/// \todo Importance (used for crossover)
		real importance;

		friend class Fitness;

	public:

		Param(Config& _cfg,
			  Config::ParamCfg& _stats);

		void flip_act();

		inline real val() const
		{
			return stats.val;
		}

		inline void set(const real _val)
		{
			stats.val = _val;
		}

		inline void update(const real _delta)
		{
			stats.val += _delta;
		}

		inline real sd() const
		{
			return stats.sd;
		}

		inline void inc_sd()
		{
			stats.sd *= (1.0 + cfg.mutation.scale);
		}

		inline void dec_sd()
		{
			stats.sd *= (1.0 - cfg.mutation.scale);
		}

		void set_trend(const Eff _eff);

		void anneal(const real _abs_fit);

		bool mutate(Fitness& _fit);
	};
}

#endif // PARAM_HPP
