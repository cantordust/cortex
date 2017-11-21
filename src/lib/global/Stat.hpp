#ifndef STAT_HPP
#define STAT_HPP

#include "Globals.hpp"
#include "Functions.hpp"

namespace Cortex
{
	struct Stat
	{
		/// Moving average type
		enum class MAType
		{
			Simple, /// Exact
			WMA, /// Windowed MA
			EMA, /// Exponential MA
			DEMA, /// Double EMA
			TEMA /// Triple EMA
		};

		MAType ma_type = MAType::EMA;

		/// Maximal value
		real max = 0.0;

		/// Minimal value
		real min = 0.0;

		/// Absolute value
		real abs = 0.0;

		/// Difference from last absolute value
		real diff = 0.0;

		/// Relative value.
		/// This is computed and set externally.
		real rel = 0.0;

		/// Number of data points
		uint count = 0;

		/// EMA coefficient
		real ema_coeff = 0.25;

		/// Window size for WMA.
		/// Also serves as the threshold for
		/// switching from SMA to EMA, DEMA, etc..
		/// Computed as floor(2 / ema_coeff) - 1.
		uint window_size = 7;

		/// Average value
		real avg = 0.0;

		/// Variance
		real var = 0.0;

		/// Standard deviation
		real sd = 0.0;

		/// \todo Add check for MA type
		inline void add(const real _new_point)
		{
			if (_new_point > max)
			{
				max = _new_point;
			}
			if (_new_point < min)
			{
				min = _new_point;
			}

			diff = _new_point - abs;

			real old_avg(avg);

			if (count <= window_size)
			{
				++count;
				avg += (_new_point - old_avg) / count;
				if (count > 1)
				{
					var += (_new_point - old_avg) * (_new_point - avg) / count;
				}
			}
			else
			{
				real tmp_diff(_new_point - avg);
				real tmp_inc(ema_coeff * tmp_diff);
				avg += tmp_inc;
				var = (1.0 - ema_coeff) * (var + tmp_diff * tmp_inc);
			}

			sd = std::sqrt(var);

			abs = _new_point;
		}

		inline real progress() const
		{
			if (count <= window_size)
			{
				return 1.0;
			}
			return sigmoid((abs - avg) / ((sd == 0.0) ? 1.0 : sd));
		}

		inline void reset()
		{
			abs = 0.0;
			diff = 0.0;
			rel = 0.0;
			count = 0;
			avg = 0.0;
			var = 0.0;
			sd = 0.0;
		}
	};
}

#endif // STAT_HPP
