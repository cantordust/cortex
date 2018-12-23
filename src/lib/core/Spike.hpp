#ifndef CORTEX_SPIKE_HPP
#define CORTEX_SPIKE_HPP

#include "Types.hpp"

namespace Cortex
{
	///=============================================================================
	///	@struct A spike event.
	///=============================================================================
	struct Spike
	{
		/// Alias for a queue of spike events
		using Scheduler = std::priority_queue<Spike, std::vector<Spike>>;

		Node* node{nullptr};

		///=====================================
		/// Constructors and destructors
		///=====================================

		Spike(Node* _node);

		///=====================================
		/// Operators
		///=====================================

		friend bool operator < (const Spike& _lhs, const Spike& _rhs) noexcept;
	};

} // namespace Cortex

#endif // CORTEX_SPIKE_HPP
