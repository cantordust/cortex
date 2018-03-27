#ifndef CORTEX_FITCONF_HPP
#define CORTEX_FITCONF_HPP

#include "ConfBase.hpp"
#include "StatConf.hpp"

namespace Cortex
{
	class FitConf final : public ConfBase
	{
	public:

		/// The target fitness that all
		/// networks are striving towards.
		real tgt;

		/// Statistics on absolute fitness
		StatConf abs;

		/// Statistics on relative fitness
		StatConf rel;

	public:

		FitConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const FitConf& _conf);
	};
}

#endif // CORTEX_FITCONF_HPP
