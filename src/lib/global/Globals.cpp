#include "Conf.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	///=========================================
	/// JSON
	///=========================================

	template<>
	void load(const json& _j, LinkDef& _id)
	{
		if (_j.find("type") == _j.end() ||
			_j.find("src") == _j.end() ||
			_j.find("tgt") == _j.end())
		{
			dlog() << "### Malformed JSON object for link ID: \n"
				   << _j.dump(4);
			exit(EXIT_FAILURE);
		}
		load(_j, "type", _id.type);
		load(_j, "src", _id.src);
		load(_j, "tgt", _id.tgt);
	}

	///=========================================
	/// operator ==
	///=========================================

	bool operator == (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role == _rhs.role &&
				_lhs.layer == _rhs.layer &&
				_lhs.idx == _rhs.idx);
	}

	bool operator == (const LinkDef& _lhs, const LinkDef& _rhs) noexcept
	{
		return (_lhs.type == _rhs.type &&
				_lhs.src == _rhs.src &&
				_lhs.tgt == _rhs.tgt);
	}

	///=========================================
	/// operator !=
	///=========================================

	bool operator != (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role != _rhs.role ||
				_lhs.layer != _rhs.layer ||
				_lhs.idx != _rhs.idx);
	}

	///=========================================
	/// operator >
	///=========================================

	bool operator > (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role == _rhs.role &&
				_lhs.layer == _rhs.layer &&
				_lhs.idx > _rhs.idx);
	}

	///=========================================
	/// operator <
	///=========================================

	bool operator < (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role == _rhs.role &&
				_lhs.layer == _rhs.layer &&
				_lhs.idx < _rhs.idx);
	}

	///=========================================
	/// operator <<
	///=========================================

	std::ostream& operator << (std::ostream& _strm, const NodeID& _id)
	{
		return _strm << "[" << _id.role << "|" << _id.layer << "|" << _id.idx << "]";
	}

	std::ostream& operator << (std::ostream& _strm, const LinkDef& _def)
	{
		return _strm << "[" << _def.type << "|" << _def.src << "->" << _def.tgt << "]";
	}

	///=========================================
	/// operator ++
	///=========================================

	NodeID& NodeID::operator++ ()
	{
		++idx;
		return *this;
	}

	///=========================================
	/// operator --
	///=========================================

	NodeID& NodeID::operator-- ()
	{
		--idx;
		return *this;
	}
}

namespace std
{
	size_t hash<Cortex::NodeID>::operator()(const Cortex::NodeID& _id) const noexcept
	{
		return AbacusHash::tuple_hash(make_tuple(_id.role, _id.layer, _id.idx));
	}

	size_t hash<Cortex::LinkDef>::operator()(const Cortex::LinkDef& _ld) const noexcept
	{
		return AbacusHash::tuple_hash(make_tuple(_ld.type, _ld.src, _ld.tgt));
	}

	size_t hash<enum E>::operator()(const E _e) const noexcept
	{
		return static_cast<typename std::underlying_type<E>::type>(_e);
	}
}
