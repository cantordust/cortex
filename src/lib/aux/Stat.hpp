#ifndef CORTEX_STAT_HPP
#define CORTEX_STAT_HPP

#include "Types.hpp"
#include "Enum.hpp"
#include "MathOps.hpp"

namespace Cortex
{
	///=====================================
	/// @struct Generic statistics class.
	/// Holds only the mean and variance for
	/// a minimal memory footprint.
	/// The current value is assumed to be
	/// stored elsewhere.
	///=====================================
	template<MAType ma>
	struct Statistics
	{
		/// Forgetting factor in case of EMA.
		/// Data point count in case of SMA.
		real alpha{0.0};

		/// Mean and variance
		real mean{0.0};
		real var{0.0};

		Statistics(const real _alpha = (ma == MAType::Simple ? 0.0 : 0.25))
			:
			  alpha(_alpha)
		{
			if (ma == MAType::Exponential)
			{
				require(alpha > 0.0 && alpha < 1.0, "[Stat] Invalid forgetting rate (", alpha, ") for exponential moving average");
			}
		}

		/// @brief Computes the standard deviation
		/// on demand from the variance.
		inline real sd() const
		{
			return std::sqrt(var);
		}

		/// @brief Compute the normalised offset
		/// logistic($\frac{_val - mean}{sd()}$)
		template<Func f = Func::Logistic>
		inline real get_offset(const real _val) const
		{
//			dlog("Stat::get_offset(", _val, "), mean:", mean, ", sd:", sd());

			return function<f>( (_val - mean) / (var > 0.0 ? sd()
														   : (_val != 0.0 ? std::fabs(_val)
																		  : (mean != 0.0 ? std::fabs(mean)
																						 : 1.0)
																			)
															 )
								);
		}

		/// @brief Update the statistics with a new value.
		inline void update(const real _new_value);

		inline void reset()
		{
			mean = 0.0;
			var = 0.0;
		}
	};

	template<>
	inline void Statistics<MAType::Simple>::update(const real _new_value)
	{
		alpha += 1;
		real old_mean(mean);
		mean += (_new_value - old_mean) / alpha;
		if (static_cast<uint>(alpha) > 1)
		{
			var += (_new_value - old_mean) * (_new_value - mean) / alpha;
		}

//		dlog("Stat<MAType::Simple>::update(", _new_value, "), mean:", mean, ", sd:", sd());
	}

	template<>
	inline void Statistics<MAType::Exponential>::update(const real _new_value)
	{
		real diff(_new_value - mean);
		real inc(alpha * diff);
		mean += inc;
		var = (1.0 - alpha) * (var + diff * inc);

//		dlog("Stat<MAType::Exponential>::update(", _new_value, "), mean: ", mean, ", sd: ", sd());
	}

	///=====================================
	/// Statistics package
	/// (value + statistics)
	///=====================================

	template<MAType ma>
	struct StatPack : Statistics<ma>
	{
		/// Current value.
		real value{0.0};

		StatPack(const real _alpha = (ma == MAType::Simple ? 0.0 : 0.25))
			:
			  Statistics<ma>(_alpha)
		{}

		/// @brief Replace the current value with a new value.
		inline void update(const real _new_value)
		{
			value = _new_value;
			Statistics<ma>::update(value);
		}

		/// @brief Add @p _delta to the current value
		/// and compute the statistics.
		inline void add(const real _delta)
		{
			update(value + _delta);
		}

		/// @brief Compute the normalised offset of the current
		/// or the provided value.
		inline real get_offset() const
		{
			return Statistics<ma>::get_offset(value);
		}

		/// @brief Compute the normalised offset of the current
		/// or the provided value.
		inline real get_offset(const real _value) const
		{
			return Statistics<ma>::get_offset(_value);
		}

		inline void reset()
		{
			value = 0.0;
			Statistics<ma>::reset();
		}
	};

	using EMAStat = Statistics<MAType::Exponential>;
	using SMAStat = Statistics<MAType::Simple>;

	using EMAStatPack = StatPack<MAType::Exponential>;
	using SMAStatPack = StatPack<MAType::Simple>;
}

#endif // CORTEX_STAT_HPP
