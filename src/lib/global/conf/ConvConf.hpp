#ifndef CORTEX_CONVCONF_HPP
#define CORTEX_CONVCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class ConvConf final
	{
	public:

		uint depth;

		uint stride;

		uint zp;

		ConvConf();

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const ConvConf& _conf);
	};
}

#endif // CORTEX_CONVCONF_HPP
