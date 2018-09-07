#include "Data.hpp"

namespace Cortex
{
	const Sample& Data::rnd_sample()
	{
		static Sample undef;
		return (samples.empty() ? undef
								: rnd_elem(samples));
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
		return ((_mode == Stage::Undef) ? samples.size()
										   : by.set[_mode].size());
	}

	std::string Data::get_label(const uint _idx)
	{
		return (!is_key(labels.idx, _idx) ? "" : labels.idx[_idx]);
	}

//	Mat Data::stretch(const Mat& _image, const uint _max_fs)
//	{

//	}

	void Data::shuffle()
	{
		by.set.clear();

		for (auto& sample : samples)
		{
			by.set[conf->data.sets.spin()].push_back(sample);
		}
	}

} // namespace Cortex
