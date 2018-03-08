#ifndef CORTEX_MUTCONF_HPP
#define CORTEX_MUTCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class MutConf final
	{
	public:

		Conf& conf;

		/// Toggle mutation
		bool enabled;

		/// Enables scaling the probability
		/// of adding / deleting links and nodes
		/// depending on the connectivity of the network
		bool adaptive;

		/// Mutations per generation
		uint rate;

		/// Optimisation method
		Opt opt;

		/// Scaling factor for the SD used for mutations.
		/// Default: 0.05 (5%)
		real scale;

		/// Mutation probability weights for the network
		emap<Mut, real> prob;

		struct EliteConf
		{
			/// Toggle elitism
			bool enabled;

			/// Proportion of the population to consider as elite
			real prop;
		} elite;

	public:

		MutConf(Conf& _conf);

		friend std::ostream& operator << (std::ostream& _strm, const MutConf& _conf);

		void set_defaults();

		std::string validate();

		bool allowed(const Mut _mut);

	};
}

#endif // CORTEX_MUTCONF_HPP
