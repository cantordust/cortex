#ifndef CORTEX_SPCCONF_HPP
#define CORTEX_SPCCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class SpcConf final
	{
	public:

		Conf& conf;

		struct
		{
			// The initial number of species.
			uint count;
		} init;

		struct
		{
			// The maximal number of species.
			uint count;
		} max;

	private:

		uint id;

	public:

		SpcConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const SpcConf& _conf);

		/// @brief Obtain the next available species ID.
		uint next_id();

		/// @brief Reset the species ID.
		void reset_id();
	};
}

#endif // CORTEX_SPCCONF_HPP
