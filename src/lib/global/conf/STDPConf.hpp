#ifndef CORTEX_STDPCONF_HPP
#define CORTEX_STDPCONF_HPP

#include "ConfBase.hpp"

namespace Cortex
{
	class STDPConf final : public ConfBase
	{

	public:

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

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const STDPConf& _conf);
	};
}

#endif // CORTEX_STDPCONF_HPP
