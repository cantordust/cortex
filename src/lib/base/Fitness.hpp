#ifndef FITNESS_HPP
#define FITNESS_HPP

#include "Config.hpp"

namespace Cortex
{
	class Fitness
	{
	private:

		Config& cfg;

		Stat stat;

		/// The effect of the last mutation.
		/// Used for trend tracking.
		Eff eff;

		/// The parameters which were mutated
		/// in the last round of evolution.
		std::vector<ParamRef> params;

	public:

		Fitness(Config& _cfg);

		inline bool is_solved() const
		{
			return stat.abs >= cfg.fit.tgt;
		}

		inline void add_param(Param& _p)
		{
			params.emplace_back(_p);
		}

		inline real progress() const
		{
			return stat.progress();
		}

		inline real get_abs() const
		{
			return stat.abs;
		}

		void set_abs(const real _abs_fit);

		inline real diff() const
		{
			return stat.diff;
		}

		inline real get_rel() const
		{
			return stat.rel;
		}

		inline void set_rel(const real _rel)
		{
			stat.rel = _rel;
		}

		void feedback();
	};
}

#endif // FITNESS_HPP
