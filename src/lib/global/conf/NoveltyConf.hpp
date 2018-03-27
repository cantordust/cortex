#ifndef CORTEX_NOVELTYCONF_HPP
#define CORTEX_NOVELTYCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class NoveltyConf final : public ConfBase
	{

	public:

		/// @brief Toggle novelty search on or off.
		bool enabled;

		/// @brief Number of previous states to keep in the history.
		uint hist_size;

	public:

		NoveltyConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const NoveltyConf& _conf);
	};
}

#endif // CORTEX_NOVELTYCONF_HPP
