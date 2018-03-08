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
		load(_j, "type", _id.lt);
		load(_j, "src", _id.sr);
		load(_j, "tgt", _id.tr);
	}

	///=========================================
	/// operator ==
	///=========================================

	bool operator == (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role == _rhs.role && _lhs.idx == _rhs.idx);
	}

	bool operator == (const LinkDef& _lhs, const LinkDef& _rhs) noexcept
	{
		return (_lhs.lt == _rhs.lt && _lhs.sr == _rhs.sr && _lhs.tr == _rhs.tr);
	}

	///=========================================
	/// operator !=
	///=========================================

	bool operator != (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role != _rhs.role || _lhs.idx != _rhs.idx);
	}

	///=========================================
	/// operator >
	///=========================================

	bool operator > (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role == _rhs.role && _lhs.idx > _rhs.idx);
	}

	///=========================================
	/// operator <
	///=========================================

	bool operator < (const NodeID& _lhs, const NodeID& _rhs) noexcept
	{
		return (_lhs.role == _rhs.role && _lhs.idx < _rhs.idx);
	}

	///=========================================
	/// operator <<
	///=========================================

	std::ostream& operator << (std::ostream& _strm, const NodeID& _id)
	{
		return _strm << "[" << _id.role << "|" << _id.idx << "]";
	}

	std::ostream& operator << (std::ostream& _strm, const LinkDef& _id)
	{
		return _strm << "[" << _id.lt << "|" << _id.sr << "->" << _id.tr << "]";
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
		return AbacusHash::tuple_hash(make_tuple(_id.role, _id.idx));
	}

	size_t hash<Cortex::LinkDef>::operator()(const Cortex::LinkDef& _ld) const noexcept
	{
		return AbacusHash::tuple_hash(make_tuple(_ld.lt, _ld.sr, _ld.tr));
	}
}
