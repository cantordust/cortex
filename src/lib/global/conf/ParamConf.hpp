#ifndef CORTEX_PARAMCONF_HPP
#define CORTEX_PARAMCONF_HPP

#include "ConfBase.hpp"
#include "StatConf.hpp"

namespace Cortex
{
	class ParamConf final : public ConfBase
	{

	public:

		/// Initialisation parameters
		Dist dist;

		/// Value, mean and standard deviation for initialisation
		real val;
		real mean;
		real sd;

		/// Upper and lower bounds.
		real lbound;
		real ubound;

		/// Configuration for the parameter statistics
		StatConf stat;

	public:

		ParamConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const ParamConf& _c);
	};
}

#endif // CORTEX_PARAMCONF_HPP
