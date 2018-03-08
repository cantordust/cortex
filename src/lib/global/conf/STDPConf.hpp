#ifndef CORTEX_STDPCONF_HPP
#define CORTEX_STDPCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class STDPConf final
	{

	public:

		Conf& conf;

		/// @brief Toggle STDP on or off.
		bool enabled;

		/// @brief Learning rate for STDP.
		real lr;

		/// @brief Ratio of LTD to LTP strength.
		real dp_ratio;

		/// @brief STDP decay rate.
		real tau;

		/// @brief Cutoff for STDP update magnitude
		real cutoff;

	public:

		STDPConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const STDPConf& _conf);
	};
}

#endif // CORTEX_STDPCONF_HPP
