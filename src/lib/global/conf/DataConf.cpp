#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, DataConf& _c)
	{
		load(_j, "samples", _c.samples);
		load(_j, "sets", _c.sets);
		load(_j, "type", _c.type);
		load(_j, "stat", _c.stat);
	}

	DataConf::DataConf(Conf& _conf)
		:
		  ConfBase(_conf, "Data configuration"),
		  stat(_conf),
		  data(std::make_shared<Data>())
	{
		set_defaults();
	}

	void DataConf::set_defaults()
	{
		samples = 0;

		type = DataType::Real;

		stat.track = false;
		stat.ma = MA::SMA;
	}

	void DataConf::validate()
	{
		if (!sets.empty())
		{
			real sum(0.0);
			for (const auto& set : sets)
			{
				check(set.second > 0.0, "Invalid ", set.first, " set proportion (", set.second, ")");
				sum += set.second;
			}

			for (auto& set : sets)
			{
				set.second /= sum;
			}
		}
	}

	std::ostream& operator << (std::ostream& _strm, const DataConf& _conf)
	{
		_strm << _conf.header()
			  << "\n\tsamples: " << _conf.samples
			  << "\n\tsets:\n" << _conf.sets
			  << "\n\ttype: " << _conf.type
			  << "\n\tstat: " << _conf.stat;

		return _strm << "\n";
	}


	const Sample& DataConf::rnd_sample()
	{
		if (data->samples.empty())
		{
			return data->undef_sample;
		}
		return conf.rnd_elem(data->samples);
	}

	const std::vector<Sample>& DataConf::get_samples()
	{
		return data->samples;
	}

	const std::vector<SampleRef>& DataConf::get_set(const Set _set)
	{
		if (data->by_set.find(_set) == data->by_set.end())
		{
			dlog() << "### Set " << _set << " is empty!";
			return data->undef_set;
		}
		return data->by_set.at(_set);
	}

	/// @todo Implementation
	Sample DataConf::generate()
	{
		return Sample();
	}

	uint DataConf::get_set_size(const Set _set)
	{
		return (_set == Set::Undef) ? data->samples.size() : data->by_set[_set].size();
	}

	void DataConf::add(Sample&& _sample, std::string&& label)
	{
		data->samples.push_back(_sample);

		/// Place in the correct set
		if (!conf.data.sets.empty())
		{
			data->by_set[conf.w_dist(conf.data.sets)].push_back(data->samples.back());
		}

		if (!label.empty())
		{
			if (data->labels.find(label) == data->labels.end())
			{
				data->labels[label] = data->labels.size();
				data->label_idx[data->labels[label]] = label;
			}
			data->samples.back().label = data->labels[label];
			data->by_label[data->labels.at(label)].push_back(data->samples.back());
		}

		/// Update the statistics
		if (conf.data.stat.track &&
			conf.data.type == DataType::Real)
		{
			for (uint i = 0; i < data->samples.back().input.size(); ++i)
			{
				data->stat.at(i).update(data->samples.back().input.at(i).front());
			}
		}
	}

	std::string DataConf::get_label(const uint _idx)
	{
		if (data->label_idx.find(_idx) != data->label_idx.end())
		{
			return data->label_idx[_idx];
		}
		return "";
	}

	void DataConf::init()
	{
		if (conf.node.roles.at(NR::I) == 0)
		{
			dlog() << "### Error: 0 input node, exiting.";
			exit(EXIT_FAILURE);
		}
		data->stat = std::vector<Stat>(conf.node.roles.at(NR::I), conf.data.stat);
	}

}

