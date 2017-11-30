#include "Stat.hpp"

namespace Cortex
{
	inline void Stat::update(const real _new_point)
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

		real old_mean(mean);

		if (count <= window_size)
		{
			++count;
			mean += (_new_point - old_mean) / count;
			if (count > 1)
			{
				var += (_new_point - old_mean) * (_new_point - mean) / count;
			}
		}
		else
		{
			real tmp_diff(_new_point - mean);
			real tmp_inc(ema_coeff * tmp_diff);
			mean += tmp_inc;
			var = (1.0 - ema_coeff) * (var + tmp_diff * tmp_inc);
		}

		sd = std::sqrt(var);

		abs = _new_point;
	}

	inline real Stat::get_progress() const
	{
		if (count <= window_size)
		{
			return 1.0;
		}
		return std::tanh((abs - mean) / ((sd == 0.0) ? 1.0 : sd));
	}

	void Stat::reset()
	{
		max = 0.0;
		min = 0.0;
		abs = 0.0;
		diff = 0.0;
		rel = 0.0;
		count = 0;
		mean = 0.0;
		var = 0.0;
		sd = 0.0;
	}
}
