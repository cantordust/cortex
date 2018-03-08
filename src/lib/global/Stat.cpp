#include "Conf.hpp"
#include "Stat.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, Stat& _stat)
	{
		load(_j, "track", _stat.track);
		load(_j, "cur", _stat.cur);
		load(_j, "min", _stat.min);
		load(_j, "max", _stat.max);
		load(_j, "count", _stat.count);
		load(_j, "mean", _stat.mean);
		load(_j, "var", _stat.var);
		load(_j, "sd", _stat.sd);
		load(_j, "ma", _stat.ma);
		load(_j, "alpha", _stat.alpha);
	}

	Stat::Stat(const MA _ma, const real _alpha)
		:
		  track(true),
		  cur(0.0),
		  min(0.0),
		  max(0.0),
		  count(0),
		  mean(0.0),
		  var(0.0),
		  sd(0.0),
		  ma(_ma),
		  alpha(_alpha)
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

		cur = _new_val;
	}

	real Stat::get_offset() const
	{
		return (track) ? (cur - mean) / ((sd == 0.0) ? (mean == 0.0 ? 1.0 : std::abs(mean)) : sd)
					   : 0;
	}

	real Stat::get_offset(const real _val) const
	{
		return (track) ? (_val - mean) / ((sd == 0.0) ? (mean == 0.0 ? 1.0 : std::abs(mean)) : sd)
					   : 0;
	}

	void Stat::reset()
	{
		cur = 0.0;
		min = 0.0;
		max = 0.0;
		count = 0;
		mean = 0.0;
		var = 0.0;
		sd = 0.0;
	}

	std::string Stat::validate()
	{
		std::stringstream problems;

			if (ma == MA::EMA &&
				(alpha >= 1.0 ||
				 alpha <= 0.0))
			{
				problems << "\t - EMA coefficient must be between 0 and 1 (current value: "
						 << alpha << ").\n";
			}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const Stat& _stat)
	{
		return _strm << "\n--- Stat ---"
					 << "\n\ttrack: " << _stat.track
					 << "\n\tval: " << _stat.cur
					 << "\n\tmin: " << _stat.min
					 << "\n\tmax: " << _stat.max
					 << "\n\tcount: " << _stat.count
					 << "\n\tmean: " << _stat.mean
					 << "\n\tvar: " << _stat.var
					 << "\n\tsd: " << _stat.sd
					 << "\n\tma: " << _stat.ma
					 << "\n\talpha: " << _stat.alpha
					 << "\n";
	}
}
