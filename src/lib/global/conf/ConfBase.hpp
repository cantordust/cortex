#ifndef CORTEX_CONFBASE_HPP
#define CORTEX_CONFBASE_HPP

#include "Globals.hpp"

namespace Cortex
{
	class ConfBase
	{

	public:

		/// @brief Reference to the main configuration
		Conf& conf;

		/// @brief Name of the subconfiguration
		std::string name;

	public:

		ConfBase(Conf& _conf, const std::string& _name);

		std::string header() const;

		void report(std::string&& _str);

		virtual void set_defaults() = 0;

		virtual void validate() = 0;

		template<typename ... Args>
		void check(const bool _statement, Args&& ... _args)
		{
			if (!_statement)
			{
				std::stringstream problems;
				std::array<int, sizeof...(_args)> status({ (problems << "[" << name << "]: " << _args << ".\n", 0)... });
				report(problems.str());
			}
		}
	};
}

#endif // CORTEX_CONFBASE_HPP
