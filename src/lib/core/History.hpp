#ifndef CORTEX_HISTORY_HPP
#define CORTEX_HISTORY_HPP

#include "Stat.hpp"
#include "JSONOps.hpp"

namespace Cortex
{
	///=====================================
	/// @class Archive of values and
	/// statistics for a number of runs.
	///=====================================
	class History : Serialiser
	{
	private:

		bool closed{false};

		/// A record for a single run.
		struct Record
		{
			/// Statistics for a single run.
			SMAStat stats;

			/// Archive of individual values
			/// obtained throughout the run.
			std::vector<uint> archive;

			void add(const uint _val);
		};

		/// Global statistics
		hmap<Stat, SMAStat> stats;

		hmap<uint, hmap<Stat, Record>> archive;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		~History();

		/// @brief Insert an empty record for a new run.
		void new_run();

		/// @brief Add a new entry to the record for @p _stat
		void add(const Stat _stat, const uint _val);

		/// @brief Compute the global statistics and finalise.
		void close();

		/// @brief Get a reference to the statistics.
		const hmap<uint, hmap<Stat, Record>>& get();

		///=====================================
		/// Printing and (de)serialisation
		///=====================================

		/// @brief Tabulate statistics.
		void print();

		virtual void load (const json& _j) override;

		virtual void save (json& _j) const override;
	};
} // namespace Cortex

#endif // CORTEX_HISTORY_HPP
