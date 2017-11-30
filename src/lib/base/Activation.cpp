#include "Activation.hpp"
#include "Functions.hpp"

namespace Cortex
{

	Activation::Activation(const NR _role, const ConfigRef& _cfg)
		:
		  fn(_cfg.get().node.init.fn.at(_role))
	{}

	inline real Activation::eval(const std::vector<real>& _input)
	{
		switch (fn)
		{
		case Fn::Sum:
			return Sum(_input);

		case Fn::Tanh:
			return Tanh(_input);

		case Fn::Logistic:
			return Sigmoid(_input);

		case Fn::ReLU:
			return ReLU(_input);

		case Fn::Gaussian:
			return Gaussian(_input);

		case Fn::Sin:
			return Sin(_input);

		case Fn::Cos:
			return Cos(_input);

		case Fn::Abs:
			return Abs(_input);

		case Fn::Min:
			return Min(_input);

		case Fn::Max:
			return Max(_input);

		case Fn::Avg:
			return Avg(_input);

		case Fn::Const:
			return 1.0;

		case Fn::Golden:
			return phi;

		default:
			return 0.0;
		}
	}

	inline Fn Activation::get_fn() const
	{
		return fn;
	}

	inline bool Activation::set_fn(const Fn _fn)
	{
		fn = _fn;
		return true;
	}


}
