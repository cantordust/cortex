#ifndef CORTEX_DATA_HPP
#define CORTEX_DATA_HPP

#include "Conf.hpp"
#include "Sample.hpp"

namespace Cortex
{
	class Data
	{

	protected:

		/// Collection of data samples.
		std::vector<Sample> samples;

		/// Label dictionaries.
		struct Label
		{
			hmap<std::string, uint> str; /// label -> label index
			hmap<uint, std::string> idx; /// label index -> label
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
		template<typename LabelType,  OnlyIf<IsNum<LabelType> || is<LabelType, std::string>> ...>
		const std::vector<SampleRef>& get_samples(const LabelType& _label)
		{
			static const std::vector<SampleRef> undef;

			std::string label(conv_label(_label));
			return ((label.empty() || !is_key(by.label, _label)) ? undef
																 : by.label[labels.idx[_label]]);
		}

		/// @brief Add a new data sample.
		/// The sample can contain several variables.
		/// The data type determines whether or not
		/// we should collect statistics.
		template<typename M, typename LabelType = std::string,
				 OnlyIf<is<M, Mat> && (IsNum<LabelType> || is<LabelType, std::string>)> ...>
		void add(M&& _sample, const LabelType& _label = LabelType())
		{
			samples.emplace_back(_sample);

			std::string label(is<LabelType, std::string> ? _label : std::to_string(_label));

			/// Place in the correct set
			by.set[conf->data.sets.spin()].push_back(samples.back());

			if (!label.empty())
			{
				if (!is_key(labels.str, label))
				{
					labels.str[label] = labels.str.size();
					labels.idx[labels.str[label]] = label;
				}
				samples.back().label = labels.str[label];
				by.label[labels.str.at(label)].push_back(samples.back());
			}
		}

		std::string get_label(const uint _idx);

		void shuffle();
	};

	///=====================================
	/// A unique pointer to a global data container.
	/// All data samples are stored in this variable.
	///=====================================

	inline DataPtr data;
}

#endif // CORTEX_DATA_HPP
