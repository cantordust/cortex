#include "Conf.hpp"
#include "Node.hpp"
#include "Link.hpp"
#include "Parameter.hpp"

namespace Cortex
{
	Link::Element(const LinkType _type)
		:
		  type(_type),
		  weight(conf->link.weight, conf->link.weight.initialise())
	{}

	Link::Element(const Link& _other)
		:
		  type(_other.type),
		  weight(conf->link.weight, _other.weight.value)
	{}

	void Link::ltp(const real _dw)
	{
		/// Both excitatory and inhibitory links are potentiated
		weight.value += _dw * (weight.value >= 0.0 ? conf->link.weight.ubound - weight.value
												   : conf->link.weight.lbound - weight.value);
	}

	void Link::ltd(const real _dw)
	{
		/// Excitatory links are depressed, inhibitory links are potentiated
		weight.value += _dw * (weight.value >= 0.0 ? - conf->learning.stdp.dp_ratio * weight.value
												   : conf->link.weight.lbound - weight.value);
	}

	os& operator << (os& _os, const Link& _link)
	{
		return _os << std::string(' ', 6) << "`<-- " << _link.weight.value << "\n";
	}

	bool operator == (const Link& _lhs, const Link& _rhs)
	{
		return _lhs.weight == _rhs.weight;
	}

	bool operator != (const Link& _lhs, const Link& _rhs)
	{
		return !(_lhs == _rhs);
	}
}
