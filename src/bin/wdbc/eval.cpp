#include "eval.hpp"

namespace WDBC
{
	void eval(Net& _net)
	{
		uint fitness(0);

//		for (const auto& ex : examples)
//		{
//			_net.eval(ex.second);
//			if (_net.get_output().front() == 1.0 &&
//				labels.at(ex.first))
//			{
//				++fitness;
//			}
//		}

		_net.set_fitness(fitness);
	}

	bool setup(Conf& _conf)
	{
		if (!load_data(_conf))
		{
			dlog() << "Error loading data.";
			return false;
		}

		_conf.fit.tgt = 1.0;
		_conf.validate();

		return true;
	}

	void test(Conf& _conf)
	{
		dlog() << "Training set: " << _conf.data.get_set_size(Set::Training) << " samples\n"
			   << "Validation set: " << _conf.data.get_set_size(Set::Validation) << " samples\n"
			   << "Test set: " << _conf.data.get_set_size(Set::Test) << " samples\n"
			   << "Total: " << _conf.data.get_set_size() << "\n" ;
	}

	bool load_data(Conf& _conf)
	{
		std::string data_file("data/wdbc.csv");
		uint label_idx(1);
		std::vector<std::string> skip_list;
		std::string pos("");
		std::string neg("");
		_conf.load("custom.data_file", data_file);
		_conf.load("custom.label_idx", label_idx);
		_conf.load("custom.skip_list", skip_list);

		std::ifstream data_stream(data_file);

		if ( !data_stream.is_open() )
		{
			dlog() << "Error opening file " << data_file << "\n";
			return false;
		}

		std::string line("");

		// Gobble the entries from the file and
		// organise them into tables for easy access
		while ( std::getline(data_stream, line) )
		{
			std::stringstream ss(line);

			std::string entry("");
			std::vector<std::string> entries;
			std::vector<real> input;
			std::string label("");

			while(std::getline(ss, entry, ','))
			{
				entries.push_back(entry);
			}

			bool skip(false);

			if (label_idx >= entries.size())
			{
				dlog() << "Invalid label index.";
				return false;
			}

			for (const auto& e : entries)
			{
				for (const auto& skip_cond : skip_list)
				{
					if (e == skip_cond)
					{
						skip = true;
						break;
					}
				}
			}

			if (!skip)
			{
				label = entries.at(label_idx);

				dlog() << "New sample: " << label;

				for (uint idx = 2; idx < 12; ++idx)
				{
					input.push_back(std::stod(entry));
				}
				_conf.data.add({input}, label);
			}
		}

		return true;
	}
}
