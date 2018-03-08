#ifndef CORTEX_MATINGCONF_HPP
#define CORTEX_MATINGCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class MatingConf final
	{

	public:

		Conf& conf;

		/// Allow or disallow mating
		bool enabled;

		/// Rate of producing offspring as a portion
		/// of the current ecosystem size.
		real rate;

	public:

		MatingConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const MatingConf& _conf);
	};
}

#endif // CORTEX_MATINGCONF_HPP
