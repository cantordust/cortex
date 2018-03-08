#include "LayerConf.hpp"

namespace Cortex
{
	LayerConf::LayerConf()
	{}

	void LayerConf::set_defaults()
	{

	}

	std::string LayerConf::validate()
	{
		std::stringstream problems;

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const LayerConf& _conf)
	{
		return _strm << "\n";
	}
}
