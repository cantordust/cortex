#ifndef CORTEX_MUTCONF_HPP
#define CORTEX_MUTCONF_HPP

#include "ConfBase.hpp"

namespace Cortex
{
	class MutConf final : public ConfBase
	{
	public:

		/// Enables scaling the probability
		/// of adding / deleting links and nodes
		/// depending on the connectivity of the network
		bool adaptive;

		/// Mutations per generation.
		/// Set to 0 to disable.
		uint rate;

		/// Optimisation method
		ParamOpt opt;

		/// Scaling factor for the SD used for mutations.
		/// Default: 0.05 (5%)
		real scale;

		///	Probability weights for different mutation types.
		hmap<Mut, real> prob;

		/// Proportion of the population to consider as elite.
		/// Set to 0 to disable elitism.
		real elite;

	public:

		MutConf(Conf& _conf);

		friend std::ostream& operator << (std::ostream& _strm, const MutConf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		bool allowed(const Mut _mut);

	};
}

#endif // CORTEX_MUTCONF_HPP
