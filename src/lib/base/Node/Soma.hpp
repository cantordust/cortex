#ifndef CORTEX_SOMA_HPP
#define CORTEX_SOMA_HPP

#include "Globals.hpp"
#include "Stat.hpp"

namespace Cortex
{
	struct Soma
	{
		/// Input statistics
		Stat stat;

		/// Individual incoming signals.
		/// This is used for transfer functions
		/// which require all inputs to be
		/// discernible (such as max()).
		std::vector<real> values;

		/// Membrane potential
		real potential;

		/// Timing of the last input
		real last_event;

		Soma(Stat& _stat);

		void reset();

		template<NT nt>
		void add(const real _input);

		template<NT nt>
		void add(const LinkPtr& _link);

		template<Enc enc>
		void add(const LinkPtr& _link);
	};
}

#endif // CORTEX_SOMA_HPP
