#ifndef CORTEX_GLOBALS_HPP
#define CORTEX_GLOBALS_HPP

#include "Functions.hpp"
#include "Enum.hpp"
#include "json.hpp"
#include "abacus_hash.hpp"
#include "dlog.hpp"
#include "threadpool.hpp"

namespace Cortex
{
	///=========================================
	/// Version string
	///=========================================

	/// @todo This is clunky.
	/// Automate the version bump process.
	const std::string version("0.5.3");

	///=========================================
	/// Foreign namespaces
	///=========================================

	using namespace AbacusHash;
	using namespace Async;
	using json = nlohmann::json;

	///=========================================
	/// Other structs and helper classes
	///=========================================

	struct NodeID
	{
		NR role;
		uint idx;
		friend bool operator == (const NodeID& _lhs, const NodeID& _rhs) noexcept;
		friend bool operator != (const NodeID& _lhs, const NodeID& _rhs) noexcept;
		friend bool operator > (const NodeID& _lhs, const NodeID& _rhs) noexcept;
		friend bool operator < (const NodeID& _lhs, const NodeID& _rhs) noexcept;
		friend std::ostream& operator << (std::ostream& _strm, const NodeID& _id);
		NodeID& operator++ ();
		NodeID& operator-- ();
	};

	struct LinkDef
	{
		LT lt;
		NR sr;
		NR tr;
		friend bool operator == (const LinkDef& _lhs, const LinkDef& _rhs) noexcept;
		friend std::ostream& operator << (std::ostream& _strm, const LinkDef& _id);
	};

	struct Event
	{
		NodePtr node;
		real time = 0.0;
	};
}

///=========================================
/// Hash specialisations
///=========================================

namespace std
{
	template <>
	struct hash<Cortex::NodeID>
	{
		size_t operator() (const Cortex::NodeID& _id) const noexcept;
	};

	template <>
	struct hash<Cortex::LinkDef>
	{
		size_t operator() (const Cortex::LinkDef& _lprob) const noexcept;
	};
}

#endif // CORTEX_GLOBALS_HPP
