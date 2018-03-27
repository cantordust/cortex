#ifndef CORTEX_CONVCONF_HPP
#define CORTEX_CONVCONF_HPP

#include "ConfBase.hpp"

namespace Cortex
{
	class ConvConf final : public ConfBase
	{
	public:

		/// @brief Input width
		uint w;

		/// @brief Input height
		uint h;

		/// @brief Input depth
		uint d;

		/// @brief Filter count
		uint k;

		/// @brief Filter size
		///
		/// Filters are assumed to be square,
		/// so knowing one side is enough
		uint f;

		/// @brief Units (e.g., pixels for images)
		/// by which the filter is slid across the input
		uint s;

		/// @brief Thickness of the artificial border
		/// around the input added in order to make
		/// the filters fit nicely with stride @a s.
		uint p;

	public:

		ConvConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const ConvConf& _conf);

	};
}

#endif // CORTEX_CONVCONF_HPP
