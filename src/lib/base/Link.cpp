#include "Conf.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	Link::Link(Node& _src, Node& _tgt, const LT _type)
		:
		  src(_src),
		  tgt(_tgt),
		  type(_type),
		  weight(_src.conf.link.weight)
	{}

	Link::Link(Node& _src, Node& _tgt, const Link& _other)
		:
		  src(_src),
		  tgt(_tgt),
		  type(_other.type),
		  weight(_other.weight)
	{}

	/// @todo Implementation
	template<>
	void Link::ltp<NT::Classical>()
	{
		/// Both excitatory and inhibitory links are potentiated
	}

	/// @todo Implementation
	template<>
	void Link::ltd<NT::Classical>()
	{
		/// Excitatory links are depressed, inhibitory links are potentiated
	}

	template<>
	void Link::ltp<NT::Spiking>()
	{
		/// Both excitatory and inhibitory links are potentiated
		real dw(src.conf.stdp.lr * std::exp( - (tgt.axon.time - src.axon.time) / src.conf.stdp.tau ));
		if (dw > src.conf.stdp.cutoff)
		{
			weight.update(weight.val() + dw * (weight.val() >= 0.0 ?
												   weight.pc.ubound - weight.val() :
												   weight.pc.lbound - weight.val()) );
		}
	}

	template<>
	void Link::ltd<NT::Spiking>()
	{
		/// Excitatory links are depressed, inhibitory links are potentiated
		real dw(src.conf.stdp.lr * std::exp( - (src.axon.time - tgt.axon.time) / src.conf.stdp.tau ));
		if (dw > src.conf.stdp.cutoff)
		{
			weight.update(weight.val() + dw * (weight.val() >= 0.0 ?
												   - src.conf.stdp.dp_ratio * weight.val() :
												   weight.pc.lbound - weight.val() ) );
		}
	}

	template<>
	real Link::signal<NT::Classical>()
	{
		if (src.conf.stdp.enabled)
		{
			ltd<NT::Classical>();
		}
		return weight.val() * src.axon.val.last;
	}

	template<>
	real Link::signal<NT::Spiking>()
	{
		if (src.conf.stdp.enabled)
		{
			ltd<NT::Spiking>();
		}
		return weight.val();
	}

	std::ostream& operator<< (std::ostream& _strm, const Link& _link)
	{
		return _strm << "\t  ,-> (" << _link.type << ") "
					 << _link.tgt.id
					 << " (" << _link.weight.val() << ")\n";
	}

	void Links::add(const NodePtr& _src, const NodePtr& _tgt, const LT _lt)
	{
		targets[_tgt] = std::make_shared<Link>(*_src, *_tgt, _lt);
		_tgt->links.sources[_src] = targets.at(_tgt);
	}

	void Links::add(const NodePtr& _src, const NodePtr& _tgt, const LinkPtr& _other)
	{
		targets[_tgt] = std::make_shared<Link>(*_src, *_tgt, *_other);
		_tgt->links.sources[_src] = targets.at(_tgt);
	}

	void Links::erase(const NodePtr& _src, const NodePtr& _tgt)
	{
		targets.erase(_tgt);
		_tgt->links.sources.erase(_src);
	}
}
