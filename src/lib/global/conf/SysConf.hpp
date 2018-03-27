#ifndef CORTEX_SYSCONF_HPP
#define CORTEX_SYSCONF_HPP

#include "ConfBase.hpp"

namespace Cortex
{
	class SysConf final : public ConfBase
	{

	public:

		/// @brief Number of times to run each experiment.
		uint runs;

		/// @brief Number of threads to use for network evaluation.
		uint threads;

	public:

		SysConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const SysConf& _conf);
	};
}

#endif // CORTEX_SYSCONF_HPP
