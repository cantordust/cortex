#include "eval.hpp"

namespace Xor
{
	void eval(Net& _net)
	{
		std::vector<real> output;

		{
//			dlog d;
//			d << "Net " << _net.id << " eval:\n";
			for ( const auto& in : input )
			{
				_net.eval( in );
				output.emplace_back( _net.get_output().front() );
//				d << "\t";
//				for (const auto v : in)
//				{
//					d << "\t" << v;
//				}
//				d << ": " << output.back() << "\n";
			}
		}

		/// Fitness calculated with the tanh or step functions
		real fitness(_net.cfg.fit.tgt);

		for ( uint idx = 0; idx < output.size(); ++idx )
		{
			if ( ((idx == 0 || idx == 3) && output[idx] > boundary) ||
				 ((idx == 1 || idx == 2) && output[idx] <= boundary) )
			{
				fitness -= std::fabs(output[idx] - boundary);
			}
		}

//		dlog() << "Network " << _net.id << " fitness " << fitness;

		_net.set_abs_fitness(fitness);
	}

	bool setup(Config& _config)
	{
		_config.fit.tgt = 4.0;

		if (_config.node.init.fn.at(NR::O) == Fn::Tanh)
		{
			boundary = 0.0;
		}
		else if (_config.node.init.fn.at(NR::O) == Fn::Logistic)
		{
			boundary = 0.5;
		}
		else
		{
			/// Bad transfer function at the output.
			/// Change this to set an appropriate boundary
			/// in case of a custom function.
			return false;
		}

		return _config.validate();
	}
}
