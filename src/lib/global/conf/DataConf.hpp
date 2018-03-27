#ifndef CORTEX_DATACONF_HPP
#define CORTEX_DATACONF_HPP

#include "ConfBase.hpp"
#include "Data.hpp"
#include "StatConf.hpp"

namespace Cortex
{
	class DataConf final : public ConfBase
	{

	public:

		/// Number of samples to use.
		/// Set to 0 to use all samples.
		/// A nonzero value can be used
		/// to process a small batch
		/// for debugging purposes.
		uint samples;

		/// Respective proportions of the training,
		/// validation and test sets relative
		/// to the entire data set.
		hmap<Set, real> sets;

		/// Type of data we are processing
		DataType type;

		/// Configuration for the data statistics
		StatConf stat;

	private:

		/// @brief The actual data, optionally
		/// organised into sets.
		DataPtr data;

	public:

		DataConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		void init();

		/// @brief Get a random sample
		const Sample& rnd_sample();

		/// @brief Get all samples.
		const std::vector<Sample>& get_samples();

		/// @brief Retrieve a specific set of samples.
		const std::vector<SampleRef>& get_set(const Set _set);

		/// @brief Generate a new random sample
		/// @todo Implementation
		/// Most likely using overloaded operator()()
		Sample generate();

		/// @brief Get the total size of the data set.
		uint get_set_size(const Set _set = Set::Undef);

		///=========================================
		/// Templates
		///=========================================

		/// @brief Get all samples by label.
		template<typename LabelType,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value ||
										 std::is_same<LabelType, std::string>::value>::type ... >
		const std::vector<SampleRef>& get_samples(const LabelType& _label)
		{
			std::string label(conv_label(_label));
			if (label.empty() ||
				data->by_label.find(_label) == data->by_label.end())
			{
				dlog() << "### Invalid label!";
				return data->undef_set;
			}
			return data->by_label.at(data->label_idx.at(_label));
		}

		/// @brief Add a new data sample.
		/// The sample can contain several variables.
		/// The data type determines whether or not
		/// we should collect statistics.
		template<typename LabelType = std::string,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value ||
										 std::is_same<LabelType, std::string>::value>::type ... >
		void add(const std::vector<real>& _sample, const LabelType& _label = LabelType())
		{
			add(Sample(_sample), conv_label(_label));
		}

		/// @brief Add a new data sample.
		/// The sample can contain several variables.
		/// The data type determines whether or not
		/// we should collect statistics.
		template<typename LabelType = std::string,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value ||
										 std::is_same<LabelType, std::string>::value>::type ... >
		void add(const std::vector<std::vector<real>>& _sample, const LabelType& _label = LabelType())
		{
			add(Sample(_sample), conv_label(_label));
		}

		std::string get_label(const uint _idx);

		friend std::ostream& operator << (std::ostream& _strm, const DataConf& _conf);

	private:

		/// @brief Convert the label to a string
		template<typename LabelType,
				 typename std::enable_if<std::is_arithmetic<LabelType>::value ||
										 std::is_same<LabelType, std::string>::value>::type ... >
		std::string conv_label(const LabelType& _label)
		{
			std::stringstream ss;
			ss << _label;
			return ss.str();
		}

		/// @brief Add a sample to the data set
		void add(Sample&& _sample, std::string&& _label);

	};
}

#endif // CORTEX_DATACONF_HPP
