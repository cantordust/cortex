#ifndef CORTEX_FITCONF_HPP
#define CORTEX_FITCONF_HPP

#include "Globals.hpp"
#include "Stat.hpp"

namespace Cortex
{
	class FitConf final
	{
	public:

		Conf& conf;

		/// The target fitness that all
		/// networks are striving towards.
		real tgt;

		/// Fitness statistics
		Stat stat;

	public:

		FitConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const FitConf& _conf);
	};
}

#endif // CORTEX_FITCONF_HPP
