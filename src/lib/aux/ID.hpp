#ifndef CORTEX_ID_HPP
#define CORTEX_ID_HPP

#include "Types.hpp"

namespace Cortex
{
	class IDGenerator
	{
		uint id{0};

	public:

		uint next();

		void reset();

		uint current();
	};

	///=============================================================================
	/// @struct Node ID (layer index + node index).
	///=============================================================================
	struct NodeID
	{
		opt<uint> layer{nopt};
		opt<uint> node{nopt};

		///=====================================
		/// Constructors and destructors
		///=====================================

		NodeID(const opt<uint> _layer = nopt, const opt<uint> _node = nopt);

		///=====================================
		/// Operators
		///=====================================

		bool operator == (const NodeID& _other) const noexcept;

		bool operator != (const NodeID& _other) const noexcept;

		friend os& operator << (os& _os, const NodeID& _id);
	};

} // namespace Cortex

#endif // CORTEX_ID_HPP
