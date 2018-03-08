#ifndef CORTEX_NOVELTYCONF_HPP
#define CORTEX_NOVELTYCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class NoveltyConf final
	{

	public:

		Conf& conf;

		/// @brief Toggle novelty search on or off.
		bool enabled;

		/// @brief Number of previous states to keep in the history.
		uint hist_size;

	public:

		NoveltyConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const NoveltyConf& _conf);
	};
}

#endif // CORTEX_NOVELTYCONF_HPP
