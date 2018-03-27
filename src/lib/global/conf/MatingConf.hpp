#ifndef CORTEX_MATINGCONF_HPP
#define CORTEX_MATINGCONF_HPP

#include "ConfBase.hpp"

namespace Cortex
{
	class MatingConf final : public ConfBase
	{

	public:

		/// Allow or disallow mating
		bool enabled;

		/// Rate of producing offspring as a portion
		/// of the current Environment size.
		real rate;

	public:

		MatingConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const MatingConf& _conf);
	};
}

#endif // CORTEX_MATINGCONF_HPP
