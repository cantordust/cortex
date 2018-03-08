#ifndef CORTEX_DATA_HPP
#define CORTEX_DATA_HPP

#include "Sample.hpp"
#include "Stat.hpp"

namespace Cortex
{
	class Data
	{
	public:

		Conf& conf;

	private:

		/// Statistics about the variables
		std::vector<Stat> stat;

		/// Label dictionary
		hmap<std::string, uint> labels;
		hmap<uint, std::string> label_idx;

		/// Collection of samples
		std::vector<Sample> samples;

		/// Samples in training, validation and test sets
		emap<Set, std::vector<SampleRef>> by_set;

		/// @brief Samples by label
		hmap<uint, std::vector<SampleRef>> by_label;

		/// @brief This is a hack.
		/// Returned when requesting a nonexistent set or label.
		///
		/// @note This should *not* be populated with samples.
		std::vector<SampleRef> undef;

	public:

		Data(Conf& _conf);

		void set_defaults();

		std::string validate();

		/// @brief Get a random sample
		const Sample& rnd_sample();

		/// @brief Get all samples.
		const std::vector<Sample>& get_samples();

		/// @brief Retrieve a specific set of samples.
		const std::vector<SampleRef>& get_set(const Set _set);

		/// @brief Partition the samples into sets.
		bool partition();

		/// @brief Generate a new random sample
		/// @todo Implementation
		/// Most likely using overloaded operator()()
//		Sample generate();

		/// @brief Get the total size of the data set.
		uint get_set_size(const Set _set = Set::Undef);

		///=========================================
		/// Templates
		///=========================================

		/// @brief Get all samples by label.
		template<typename LabelType,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value || std::is_same<LabelType, std::string>::value>::type ... >
		const std::vector<SampleRef>& get_samples(const LabelType& _label)
		{
			std::string label(conv_label(_label));
			if (label.empty() ||
				by_label.find(_label) == by_label.end())
			{
				dlog() << "### Invalid label!";
				return undef;
			}
			return by_label.at(label_idx.at(_label));
		}

		/// @brief Add a new data sample.
		/// The sample can contain several variables.
		/// The data type determines whether or not
		/// we should collect statistics.
		template<typename LabelType = std::string,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value || std::is_same<LabelType, std::string>::value>::type ... >
		void add_sample(const std::vector<real>& _sample, const LabelType& _label = LabelType())
		{
			add_sample(Sample(_sample), conv_label(_label));
		}

		/// @brief Add a new data sample.
		/// The sample can contain several variables.
		/// The data type determines whether or not
		/// we should collect statistics.
		template<typename LabelType = std::string,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value || std::is_same<LabelType, std::string>::value>::type ... >
		void add_sample(const std::vector<std::vector<real>>& _sample, const LabelType& _label = LabelType())
		{
			add_sample(Sample(_sample), conv_label(_label));
		}

		std::string get_label(const uint _idx);

	private:

		/// @brief Convert the label to a string
		template<typename LabelType,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value || std::is_same<LabelType, std::string>::value>::type ... >
		std::string conv_label(const LabelType& _label)
		{
			std::stringstream ss;
			ss << _label;
			return ss.str();
		}

		/// @brief Add a sample to the data set
		void add_sample(Sample&& _sample, std::string&& _label);
	};
}

#endif // CORTEX_DATA_HPP
