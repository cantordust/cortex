#ifndef CORTEX_ENVCONF_HPP
#define CORTEX_ENVCONF_HPP

#include "ConfBase.hpp"

namespace Cortex
{
	class EnvConf final : public ConfBase
	{

	public:

		/// Optimisation mode
		OptMode opt;

		/// Environment type
		EnvType type;

		/// Search mode to use (fitness, novelty, etc.)
		Search search;

		struct
		{
			/// Initial number of networks.
			uint size;
		} init;

		struct
		{
			/// Maximal number of individuals in the Environment
			uint size;

			/// Maximal number of generations
			uint age;
		} max;

	public:

		EnvConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const EnvConf& _conf);
	};
}

#endif // CORTEX_ENVCONF_HPP
