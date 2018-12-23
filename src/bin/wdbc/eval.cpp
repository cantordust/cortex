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

	bool setup()
	{
		if (!load_data())
		{
			dlog("Error loading data.");
			return false;
		}

		conf.env.evo.fit.tgt = 1.0;
		conf.validate();

		return true;
	}

	void test()
	{
		dlog("Training set: ", data->get_set_size(SetType::Training), " samples\n",
		      "Validation set: ", data->get_set_size(SetType::Validation), " samples\n",
		      "Test set: ", data->get_set_size(SetType::Test), " samples\n",
		      "Total: ", data->get_set_size());
	}

	bool load_data()
	{
		std::string data_file("data/wdbc.csv");
		uint label_idx(1);
		std::vector<std::string> skip_list;
		std::string pos("");
		std::string neg("");
		conf.load("custom.data_file", data_file);
		conf.load("custom.label_idx", label_idx);
		conf.load("custom.skip_list", skip_list);

		std::ifstream data_stream(data_file);

		if ( !data_stream.is_open() )
		{
			dlog("Error opening file ", data_file);
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
				dlog(), "Invalid label index.";
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

				dlog("New sample: ", label);

				for (uint idx = 2; idx < 12; ++idx)
				{
					input.push_back(std::stod(entry));
				}
				data->add({input}, label);
			}
		}

		return true;
	}
}
