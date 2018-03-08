#ifndef CORTEX_SYSCONF_HPP
#define CORTEX_SYSCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class SysConf final
	{

	public:

		Conf& conf;

		/// @brief Number of times to run each experiment.
		uint runs;

		/// @brief Number of threads to use for network evaluation.
		uint threads;

	public:

		SysConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const SysConf& _conf);
	};
}

#endif // CORTEX_SYSCONF_HPP
