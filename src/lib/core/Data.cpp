#include "Data.hpp"

namespace Cortex
{
	const Sample& Data::rnd_sample()
	{
		static Sample undef;
		return (samples.empty() ? undef : rnd_elem(samples));
	}

	const std::vector<Sample>& Data::get_samples()
	{
		return samples;
	}

	const std::vector<SampleRef>& Data::get_set(const Stage _set)
	{
		static std::vector<SampleRef> undef;
		return (!is_key(by.set, _set) ? undef : by.set[_set]);
	}

	Sample Data::generate()
	{
		return Sample();
	}

	uint Data::get_dataset_size(const Stage _mode)
	{
		return ((_mode == Stage::Undef) ? samples.size() : by.set[_mode].size());
	}

	const std::vector<SampleRef>&Data::get_samples(const std::string& _label)
	{
		static const std::vector<SampleRef> undef;

		return ((_label.empty() ||
				 !is_key(labels.str2idx, _label) ||
				 !is_key(by.label, labels.str2idx[_label])) ? undef
															: by.label[labels.str2idx[_label]]);
	}

	void Data::add(const std::vector<real>& _sample, const std::string& _label)
	{
		if (wheel.is_empty())
		{
			wheel = conf.data.sets;
		}

		samples.emplace_back(_sample);

		/// Place the sample in a random set
		by.set[wheel.spin()].push_back(samples.back());

		if (!_label.empty())
		{
			if (!is_key(labels.str2idx, _label))
			{
				labels.str2idx[_label] = labels.idx2str.size();
				labels.idx2str[labels.str2idx[_label]] = _label;
			}
			samples.back().label = labels.str2idx[_label];
			by.label[labels.str2idx[_label]].push_back(samples.back());
		}
	}

	std::string Data::get_label(const uint _idx)
	{
		return (!is_key(labels.idx2str, _idx) ? "" : labels.idx2str[_idx]);
	}

	void Data::shuffle()
	{
		by.set.clear();

		for (auto& sample : samples)
		{
			by.set[wheel.spin()].push_back(sample);
		}
	}

} // namespace Cortex
