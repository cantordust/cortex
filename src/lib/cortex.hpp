#ifndef CORTEX_LIB_HPP
#define CORTEX_LIB_HPP

#include "Net.hpp"
#include "Data.hpp"
#include "Conf.hpp"

namespace Cortex
{
	///=============================================================================
	/// Command line argument parsing
	///=============================================================================

	inline static clara::Parser opts =
			clara::Opt([](const std::string& _file = "default_config.json")
	{
		conf.generate(_file);
	}, "generate")["-g"]("Generate a default configuration file.") |
			clara::Opt([](const std::string& _file = "config.json")
	{
		conf.load(_file);
	}, "load")["-l"]("Load a configuration file.");

} // namespace Cortex

#endif // CORTEX_LIB_HPP
