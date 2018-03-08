#ifndef CORTEX_PARAMCONF_HPP
#define CORTEX_PARAMCONF_HPP

#include "Globals.hpp"
#include "Stat.hpp"

namespace Cortex
{
	class ParamConf final
	{

	public:

		Conf& conf;

		/// Initialisation parameters
		Dist dist;

		/// Value, mean and standard deviation for initialisation
		real val;
		real mean;
		real sd;

		/// Upper and lower bounds.
		real lbound;
		real ubound;

		/// Configuration for the parameter statistics
		Stat stat;

	public:

		ParamConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const ParamConf& _c);
	};
}

#endif // CORTEX_PARAMCONF_HPP
