#ifndef STAT_HPP
#define STAT_HPP

#include "Globals.hpp"

namespace Cortex
{
	struct Stat
	{
		// @todo Make this a config parameter
		MAType ma_type = MAType::EMA;

		real max = 0.0;
		real min = 0.0;
		real abs = 0.0;

		// Difference from last absolute value
		real diff = 0.0;

		// Relative value.
		// This should be computed and set externally.
		real rel = 0.0;

		// Number of data points
		uint count = 0;

		// EMA coefficient (alpha)
		real ema_coeff = 0.25;

		// Window size for WMA.
		// Also serves as the threshold for
		// switching from SMA to EMA
		// Computed as floor(2 / ema_coeff) - 1.
		uint window_size = 7;

		real mean = 0.0;
		real var = 0.0;
		real sd = 0.0;

		// @todo Add check for MA type
		void update(const real _new_point);

		real get_progress() const;

		void reset();
	};
}

#endif // STAT_HPP
