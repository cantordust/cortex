#include "Conf.hpp"
#include "Data.hpp"

namespace Cortex
{
	Data::Data(Conf& _conf)
		:
		  conf(_conf)
	{}

	const Sample& Data::rnd_sample()
	{
		return conf.rnd_elem(samples);
	}

	const std::vector<Sample>& Data::get_samples()
	{
		return samples;
	}

	const std::vector<SampleRef>& Data::get_set(const Set _set)
	{
		if (by_set.find(_set) == by_set.end())
		{
			dlog() << "### Set " << _set << " is empty!";
			return undef;
		}
		return by_set.at(_set);
	}

	bool Data::partition()
	{
		emap<Set, uint> counts;
		counts[Set::Training] = std::round(conf.dataset.training * samples.size());
		counts[Set::Validation] = std::round(conf.dataset.validation * samples.size());
		counts[Set::Test] = std::round(conf.dataset.test * samples.size());

		if (counts.at(Set::Training) == 0)
		{
			dlog() << "Training set size is 0. Is the data set too small?";
			return false;
		}

		if (counts.at(Set::Test) == 0)
		{
			dlog() << "Test set size is 0. Is the data set too small?";
			return false;
		}

		hset<uint> sample_ids;

		for (uint i = 0; i < samples.size(); ++i)
		{
			sample_ids.emplace(i);
		}

		uint id(0);

		for (const auto& set : Enum<Set>::entries)
		{
			for (uint i = 0; i < counts.at(set.first); ++i)
			{
				id = conf.rnd_elem(sample_ids);
				by_set.at(set.first).push_back(std::ref(samples.at(id)));
				sample_ids.erase(id);
				if (sample_ids.empty())
				{
					break;
				}
			}
		}
		return true;
	}

	/// @todo Implementation
//	Sample Data::generate()
//	{
//		return Sample();
//	}

	///=========================================
	/// Dispatch functions
	///=========================================

	uint Data::get_set_size(const Set _set)
	{
		switch (_set)
		{
		case Set::Undef:
			return samples.size();

		default:
			return by_set.at(_set).size();
		}
	}

	void Data::add_sample(Sample&& _sample, std::string&& label)
	{
		samples.push_back(_sample);

		if (!label.empty())
		{
			if (labels.find(label) == labels.end())
			{
				labels[label] = labels.size();
				label_idx[labels[label]] = label;
			}
			samples.back().label = labels[label];
			by_label[labels.at(label)].push_back(samples.back());
		}

		/// Update the statistics
		if (conf.dataset.type == DataType::Real)
		{
			for (uint i = 0; i < samples.back().input.size(); ++i)
			{
				stat.at(i).update(samples.back().input.at(i).front());
			}
		}
	}

	std::string Data::get_label(const uint _idx)
	{
		if (label_idx.find(_idx) != label_idx.end())
		{
			return label_idx[_idx];
		}
		return "";
	}

	void Data::set_defaults()
	{
		stat = std::vector<Stat>(conf.node.roles.at(NR::I), Stat(MA::SMA));

		for (const auto& set : Enum<Set>::entries)
		{
			by_set[set.first] = {};
		}
	}

	std::string Data::validate()
	{
		std::stringstream problems;

		return problems.str();
	}
}
