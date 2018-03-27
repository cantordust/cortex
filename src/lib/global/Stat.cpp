#include "Conf.hpp"
#include "Stat.hpp"

namespace Cortex
{
	Stat::Stat(const bool _track, const MA _ma, const real _alpha)
		:
		  track(_track),
		  last(0.0),
		  min(0.0),
		  max(0.0),
		  count(0),
		  mean(0.0),
		  var(0.0),
		  sd(0.0),
		  ma(_ma),
		  alpha(_alpha)
	{}

	Stat::Stat(const MA _ma, const real _alpha)
		:
		  Stat(true, _ma, _alpha)
	{}

	Stat::Stat(const StatConf& _conf)
		:
		  Stat(_conf.track, _conf.ma, _conf.alpha)
	{}

	template<>
	void Stat::update<MA::SMA>(const real _val)
	{
		real old_mean(mean);
		mean += (_val - old_mean) / count;
		if (count > 1)
		{
			var += (_val - old_mean) * (_val - mean) / count;
		}
	}

	template<>
	void Stat::update<MA::EMA>(const real _val)
	{
		if (count <= std::floor(2.0 / alpha) - 1 )
		{
			return update<MA::SMA>(_val);
		}
		real tmp_diff(_val - mean);
		real tmp_inc(alpha * tmp_diff);
		mean += tmp_inc;
		var = (1.0 - alpha) * (var + tmp_diff * tmp_inc);
	}

	void Stat::update(const real _new_val)
	{
		if (track)
		{
			if (count == 0)
			{
				min = _new_val;
				max = _new_val;
			}
			else
			{
				if (_new_val > max)
				{
					max = _new_val;
				}
				if (_new_val < min)
				{
					min = _new_val;
				}
			}

			++count;

			switch (ma)
			{
			case MA::SMA:
				update<MA::SMA>(_new_val);
				break;

			case MA::EMA:
				update<MA::EMA>(_new_val);
				break;

			default:
				break;
			}

			sd = std::sqrt(var);
		}

		last = _new_val;
	}

	real Stat::get_offset() const
	{
		return (track) ? (last - mean) / ((sd > 0.0) ? sd
														 : (mean != 0.0 ? std::fabs(mean)
																		: (last != 0.0 ? last
																					  : 1.0)))
							: 0;
	}

	real Stat::get_offset(const real _val) const
	{
		return (track) ? (_val - mean) / ((sd > 0.0) ? sd
														  : (mean != 0.0 ? std::fabs(mean)
																		 : (_val != 0.0 ? _val
																						: 1.0)))
							: 0;
	}

	void Stat::reset()
	{
		last = 0.0;
		min = 0.0;
		max = 0.0;
		count = 0;
		mean = 0.0;
		var = 0.0;
		sd = 0.0;
	}

	std::ostream& operator << (std::ostream& _strm, const Stat& _stat)
	{
		_strm << "\n\tlast value: " << _stat.last
			  << "\n\tmin: " << _stat.min
			  << "\n\tmax: " << _stat.max
			  << "\n\tcount: " << _stat.count
			  << "\n\tmean: " << _stat.mean
			  << "\n\tvar: " << _stat.var
			  << "\n\tsd: " << _stat.sd;

		return _strm  << "\n";
	}
}
