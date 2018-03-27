#ifndef CORTEX_STAT_HPP
#define CORTEX_STAT_HPP

#include "Enum.hpp"

namespace Cortex
{
	class Stat
	{
	public:

		/// Indicate whether we should collect statistics
		bool track;

		/// The current value.
		/// This is set each time the stats are updated.
		real last;

		/// Minimal and maximal values encountered so far.
		real min;
		real max;

		/// Number of data points seen so far.
		uint count;

		/// Mean and variance
		real mean;
		real var;
		real sd;

	private:

		/// Moving average type
		MA ma;

		/// "Forgetting rate" for EMA
		real alpha;

	public:

		Stat(const bool _track = true, const MA _ma = MA::EMA, const real _alpha = 0.25);

		Stat(const MA _ma = MA::EMA, const real _alpha = 0.25);

		Stat(const StatConf& _conf);

		/// @brief Add a new data point and update the statistics.
		void update(const real _new_val);

		/// @brief Compute the relative offset
		/// $\frac{val - mean}{sd}$
		real get_offset() const;

		/// @brief Compute the relative offset
		/// $\frac{val - mean}{sd}$
		real get_offset(const real _val) const;

		/// Clear all statistical information.
		void reset();

		void validate();

		friend std::ostream& operator << (std::ostream& _strm, const Stat& _stat);

	private:

		template<MA ma>
		void update(const real _new_point);
	};
}

#endif // CORTEX_STAT_HPP
