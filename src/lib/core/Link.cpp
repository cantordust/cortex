#include "Conf.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	///=====================================
	/// Constructors and destructors
	///=====================================

	Link::Link(const LinkType _type, const opt<real> _weight)
		:
		  type(_type),
		  weight(_weight ? _weight.value() : Conf::init(ParamType::Weight))
	{}

	Link::Link(const Link& _other)
		:
		  type(_other.type),
		  weight(_other.weight)
	{}

	///=====================================
	/// Operators
	///=====================================

	bool operator == (const Link& _lhs, const Link& _rhs)
	{
		return _lhs.weight == _rhs.weight;
	}

	bool operator != (const Link& _lhs, const Link& _rhs)
	{
		return !(_lhs == _rhs);
	}

	///=====================================
	/// STDP
	///=====================================

	void Link::ltp(const real _dw)
	{
		/// Both excitatory and inhibitory links are potentiated
		weight += _dw * (weight >= 0.0 ? conf.link.weight.max - weight
									   : conf.link.weight.min - weight);
	}

	void Link::ltd(const real _dw)
	{
		/// Excitatory links are depressed, inhibitory links are potentiated
		weight += _dw * (weight >= 0.0 ? - conf.learning.stdp.dp_ratio * weight
									   : conf.link.weight.min - weight);
	}

	///=====================================
	/// Printing and (de)serialisation
	///=====================================

	os& operator << (os& _os, const Link& _link)
	{
		return _os << std::string(' ', 6) << "`<-- " << _link.weight << "\n";
	}

	void Link::load(const json& _j)
	{

	}

	void Link::save(json& _j) const
	{

	}
}
