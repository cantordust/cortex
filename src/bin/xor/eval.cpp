#include "eval.hpp"

namespace Xor
{
	real boundary(0.0);

	void eval(Net& _net)
	{
		std::vector<real> output;

		{
			dlog d;
			d << "Net " << _net.id << " eval:\n";
			for ( const auto& sample : _net.conf.data->get_samples())
			{
				_net.eval(sample);
				output.push_back( _net.get_output().front() );
				for (const auto& var : sample.input)
				{
					for (const auto& val : var)
					{
						d << " " << val;
					}
				}
				d << ": " << _net.conf.data->get_label(sample.label)
				  << " (net: " << output.back() << ")\n";
			}
		}

		// Fitness calculated with the tanh or step functions
		real fitness(_net.conf.fit.tgt);

		for ( uint idx = 0; idx < output.size(); ++idx )
		{
			if ( ((idx == 0 || idx == 1) && output[idx] > boundary) ||
				 ((idx == 2 || idx == 3) && output[idx] <= boundary) )
			{
				fitness -= std::fabs(output[idx] - boundary);
			}
		}

//		dlog() << "Network " << _net.id << " fitness " << fitness;

		_net.set_fitness(fitness);
	}

	bool setup(Conf& _conf)
	{
		_conf.data->add_sample({0.0, 0.0}, 0);
		_conf.data->add_sample({1.0, 1.0}, 0);
		_conf.data->add_sample({0.0, 1.0}, 1);
		_conf.data->add_sample({1.0, 0.0}, 1);

		{
			dlog d;
			d << "Samples: \n";
			for (const auto& sample : _conf.data->get_samples())
			{
				for (const auto& i : sample.input)
				{
					for (const auto& j : i)
					{
						d << " " << j;
					}
					d << "\n";
				}
			}
		}

		_conf.fit.tgt = 4.0;

		if (_conf.node.tf.at(NR::O).size() == 0 ||
			_conf.node.tf.at(NR::O).size() > 1)
		{
			dlog() << "### Please specify exactly one transfer function at the output nodes.";
			exit(EXIT_FAILURE);
		}
		else
		{
			switch (*_conf.node.tf.at(NR::O).begin())
			{
			case TF::Tanh:
				boundary = 0.0;
				break;

			case TF::Logistic:
				boundary = 0.5;
				break;

			default:
				dlog() << "### Invalid transfer function (" << *_conf.node.tf.at(NR::O).begin()
					   << ") at the output";
				exit(EXIT_FAILURE);
			}
		}

		_conf.validate();

		dlog() << _conf;
		return true;
	}
}
