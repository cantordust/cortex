#ifndef CORTEX_DATA_HPP
#define CORTEX_DATA_HPP

#include "Conf.hpp"
#include "Sample.hpp"

namespace Cortex
{
	inline class Data
	{

	protected:

		inline static Roulette<Stage> wheel;

		/// Collection of data samples.
		std::vector<Sample> samples;

		/// Label dictionaries.
		struct Label
		{
			hmap<std::string, uint> str2idx; /// label -> label index
			hmap<uint, std::string> idx2str; /// label index -> label
		} labels;

		struct By
		{
			/// Samples organised into training,
			/// validation and test sets
			hmap<Stage, std::vector<SampleRef>> set;

			/// @brief Samples by label
			hmap<uint, std::vector<SampleRef>> label;
		} by;

	public:

		/// @brief Get a random sample.
		const Sample& rnd_sample();

		/// @brief Get all samples.
		const std::vector<Sample>& get_samples();

		/// @brief Retrieve a specific set of samples.
		const std::vector<SampleRef>& get_set(const Stage _set);

		/// @brief Generate a new random sample.
		/// @todo Implementation (most likely using overloaded operator()())
		Sample generate();

		/// @brief Get the total size of the data set.
		uint get_dataset_size(const Stage _mode = Stage::Undef);

		/// @brief Get all samples by label.
		const std::vector<SampleRef>& get_samples(const std::string& _label);

		/// @brief Templated version for non-string labels.
		template<typename LabelType>
		const std::vector<SampleRef>& get_samples(LabelType&& _label)
		{
			return get_samples(std::to_string(std::forward<LabelType>(_label)));
		}

		/// @brief Add a new data sample.
		/// The sample can contain several variables.
		/// The data type determines whether or not
		/// we should collect statistics.
		void add(const std::vector<real>& _sample, const std::string& _label = "");

		/// @brief Templated version for non-string labels.
		template<typename LabelType>
		void add(const std::vector<real>& _sample, LabelType&& _label)
		{
			add(_sample, std::to_string(std::forward<LabelType>(_label)));
		}

		std::string get_label(const uint _idx);

		/// @brief Shuffle the samples
		void shuffle();

	} dataset;
}

#endif // CORTEX_DATA_HPP
