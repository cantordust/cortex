#include "Conf.hpp"

namespace Cortex
{
	ConfBase::ConfBase(Conf& _conf, const std::string& _name)
		:
		  conf(_conf),
		  name(_name)
	{}

	std::string ConfBase::header() const
	{
		return "--- " + name + " ---\n";
	}

	void ConfBase::report(std::string&& _str)
	{
		conf.report << std::move(_str);
	}
}
