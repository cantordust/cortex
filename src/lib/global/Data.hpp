#ifndef CORTEX_DATA_HPP
#define CORTEX_DATA_HPP

#include "Sample.hpp"
#include "Stat.hpp"

namespace Cortex
{
	class Data
	{

	private:

		/// Statistics about the variables
		std::vector<Stat> stat;

		/// Label dictionaries.
		/// labels: forward lookup (label -> label index)
		/// label_idx: reverse lookup (label index -> label)
		hmap<std::string, uint> labels;
		hmap<uint, std::string> label_idx;

		/// Collection of samples
		std::vector<Sample> samples;

		/// Samples organised into training,
		/// validation and test sets
		hmap<Set, std::vector<SampleRef>> by_set;

		/// @brief Samples by label
		hmap<uint, std::vector<SampleRef>> by_label;

		/// @warning These are hacks.
		/// Returned when requesting a nonexistent set, label or sample.
		///
		/// @note This should *not* be populated with samples.
		std::vector<SampleRef> undef_set;
		Sample undef_sample;

		friend class DataConf;

	};
}

#endif // CORTEX_DATA_HPP
