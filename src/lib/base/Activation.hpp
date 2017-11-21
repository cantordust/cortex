#ifndef ACTIVATION_HPP
#define ACTIVATION_HPP

#include "Config.hpp"

namespace Cortex
{
	/// \brief Various activation functions.
	class Activation
	{

	private:

		Fn fn;

		emap<Fn, std::function<real(const std::vector<real>&)>> functions;

		friend class Node;

	public:

		Activation(const NR _role,
				   Config& _cfg)
			:
			  fn(_cfg.node.init.fn.at(_role))
		{}

		/// \todo Turn this into a simple hashmap lookup
		inline real eval(const std::vector<real>& _input)
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

		inline Fn get_fn() const
		{
			return fn;
		}

		inline bool set_fn(const Fn _fn)
		{
			fn = _fn;
			return true;
		}

	private:

	};
}

#endif // ACTIVATION_HPP
