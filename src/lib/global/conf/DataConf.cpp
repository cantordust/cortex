#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, DataConf& _c)
	{
		_c.enabled = true;
		load(_j, "samples", _c.samples);
		load(_j, "training", _c.training);
		load(_j, "test", _c.test);
		load(_j, "validation", _c.validation);
		load(_j, "type", _c.type);
	}

	DataConf::DataConf(Conf& _conf)
		:
		  conf(_conf)
	{
		set_defaults();
	}

	void DataConf::set_defaults()
	{
		enabled = false;
		samples = 0;
		training = 0.0;
		test = 0.0;
		validation = 0.0;
		type = DataType::Real;
	}

	std::string DataConf::validate()
	{
		std::stringstream problems;

		if (enabled)
		{
			if (training <= 0.0)
			{
				problems << "\t	- Invalid training set proportion (" << training << ").\n";
			}

			if (test <= 0.0)
			{
				problems << "\t	- Invalid test set proportion (" << test << ").\n";
			}

			if (validation < 0.0)
			{
				problems << "\t	- Invalid validation set proportion (" << validation << ").\n";
			}

			real sum(training + validation + test);

			if (sum <= 0.0)
			{
				problems << "\t	- Invalid values for one or more data subset proportions:"
						 << "\n\t\tTraining: " << training
						 << "\n\t\tValidation: " << validation
						 << "\n\t\tTest: " << test;
			}
			else
			{
				training /= sum;
				validation /= sum;
				test /= sum;
			}
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const DataConf& _conf)
	{
		return _strm << "\n--- Dataset ---"
					 << "\nenabled: " << _conf.enabled
					 << "\nsamples: " << _conf.samples
					 << "\ntraining: " << _conf.training
					 << "\ntest: " << _conf.test
					 << "\nvalidation: " << _conf.validation
					 << "\ntype: " << _conf.type
					 << "\n";
	}
}

