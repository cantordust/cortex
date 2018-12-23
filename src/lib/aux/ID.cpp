#include "ID.hpp"

namespace Cortex
{
	NodeID::NodeID(const opt<uint> _layer, const opt<uint> _node)
		:
		  layer(_layer),
		  node(_node)
	{}

	bool NodeID::operator == (const NodeID& _other) const noexcept
	{
		return (layer == _other.layer &&
				node == _other.node);
	}

	bool NodeID::operator !=(const NodeID& _other) const noexcept
	{
		return !(*this == _other);
	}

	os& operator << (os& _os, const NodeID& _id)
	{
		return _os << "[L " << _id.layer << ", N " << _id.node << "]";
	}

	uint IDGenerator::next()
	{
		return ++id;
	}

	void IDGenerator::reset()
	{
		id = 0;
	}

	uint IDGenerator::current()
	{
		return id;
	}

} // namespace Cortex
