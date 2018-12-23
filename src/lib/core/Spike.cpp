#include "Spike.hpp"
#include "Node.hpp"

namespace Cortex
{
	Spike::Spike(Node* _node)
		:
		  node(_node)
	{}

	bool operator < (const Spike& _lhs, const Spike& _rhs) noexcept
	{
		return _lhs.node->last.out > _rhs.node->last.out;
	}

} // namespace Cortex
