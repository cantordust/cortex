#ifndef CORTEX_STATCONF_HPP
#define CORTEX_STATCONF_HPP

#include "ConfBase.hpp"

namespace Cortex
{
	class StatConf final : public ConfBase
	{

	public:

		/// @brief Indicate whether statistics should be collected
		bool track;

		/// Type of moving average.
		MA ma;

		/// EMA coefficient
		real alpha;

	public:

		StatConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const StatConf& _conf);
	};
}

#endif // CORTEX_STATCONF_HPP
