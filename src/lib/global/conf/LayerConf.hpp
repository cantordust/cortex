#ifndef CORTEX_LAYERCONF_HPP
#define CORTEX_LAYERCONF_HPP

#include "ConvConf.hpp"

namespace Cortex
{
	class LayerConf final
	{
	public:

		LayerType type;

		std::vector<ConvConf> conv;

		LayerConf();

		void set_defaults();

		std::string validate();

		friend 	std::ostream& operator << (std::ostream& _strm, const LayerConf& _conf);
	};
}

#endif // CORTEX_LAYERCONF_HPP
