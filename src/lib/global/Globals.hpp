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
	const std::string version("0.5.5");

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
		NR role = NR::Undef;
		uint idx = 0;
		uchar layer = 0;

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
		LT type = LT::Undef;
		NR src = NR::Undef;
		NR tgt = NR::Undef;
		friend bool operator == (const LinkDef& _lhs, const LinkDef& _rhs) noexcept;
		friend std::ostream& operator << (std::ostream& _strm, const LinkDef& _def);
	};

	struct Event
	{
		NodePtr node = nullptr;
		real time = 0.0;
	};

	template<typename K, typename V>
	std::ostream& operator << (std::ostream& _strm, const hmap<K, V>& _map)
	{
		for (const auto& elem : _map)
		{
			_strm << elem.first << "->" << elem.second;
		}
		return _strm;
	}
}

///=========================================
/// Hash specialisations
///=========================================

namespace std
{
	template<>
	struct hash<Cortex::NodeID>
	{
		size_t operator() (const Cortex::NodeID& _id) const noexcept;
	};

	template<>
	struct hash<Cortex::LinkDef>
	{
		size_t operator() (const Cortex::LinkDef& _lprob) const noexcept;
	};

	template<>
	struct hash<enum class E>
	{
		size_t operator() (const E _e) const noexcept;
	};
}

#endif // CORTEX_GLOBALS_HPP
