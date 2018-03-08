#ifndef CORTEX_ECOCONF_HPP
#define CORTEX_ECOCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class EcoConf final
	{

	public:

		Conf& conf;

		/// Search mode to use (fitness, novelty, etc.)
		Search search;

		struct
		{
			/// Initial number of networks.
			uint size;
		} init;

		struct
		{
			/// Maximal number of individuals in the ecosystem
			uint size;

			/// Maximal number of generations
			uint age;
		} max;

	public:

		EcoConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const EcoConf& _conf);

	};
}

#endif // CORTEX_ECOCONF_HPP
