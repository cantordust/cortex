#ifndef CORTEX_FITNESS_HPP
#define CORTEX_FITNESS_HPP

#include "Stat.hpp"

namespace Cortex
{
	class Fitness
	{
	public:

		Conf& conf;

		/// Statistics on the absolute fitness
		Stat abs;

		/// Statistics on the relative fitness
		Stat rel;

	private:

		/// The effect of the last mutation.
		/// Used for tracking the effects of mutations.
		Eff eff;

		/// Parameters being updated.
		std::vector<ParamRef> params;

	public:

		Fitness(Conf& _conf);

		void reset();

		bool is_solved() const;

		void add_param(Param& _p);

		void feedback(const real _val);

	private:

		template<ParamOpt opt>
		void feedback();

		friend class Net;
		friend Param;
	};
}

#endif // CORTEX_FITNESS_HPP
