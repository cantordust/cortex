#include "Conf.hpp"
#include "ConvConf.hpp"

namespace Cortex
{
	ConvConf::ConvConf()
	{}

	void ConvConf::set_defaults()
	{

	}

	std::string ConvConf::validate()
	{
		std::stringstream problems;

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const ConvConf& _conf)
	{
		return _strm << "\n";
	}
}
