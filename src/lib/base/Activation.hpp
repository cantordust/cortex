#ifndef ACTIVATION_HPP
#define ACTIVATION_HPP

#include "Config.hpp"

namespace Cortex
{
	// \brief Various activation functions.
	class Activation
	{

	private:

		Fn fn;

		emap<Fn, std::function<real(const std::vector<real>&)>> functions;

		friend class Node;

	public:

		Activation() {}

		Activation(const NR _role,
				   const ConfigRef& _cfg);

		/// @todo Turn this into a simple hashmap lookup
		real eval(const std::vector<real>& _input);

		Fn get_fn() const;

		bool set_fn(const Fn _fn);

	private:

	};
}

#endif // ACTIVATION_HPP
