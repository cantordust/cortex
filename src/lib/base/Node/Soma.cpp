#include "Conf.hpp"
#include "Link.hpp"
#include "Node.hpp"

namespace Cortex
{
	Soma::Soma(StatConf& _conf)
		:
		  stat(_conf),
		  potential(0.0),
		  last_event(0.0)
	{}

	void Soma::reset()
	{
		values.clear();
		potential = 0.0;
	}

	template<>
	void Soma::add<NT::Classical>(const real _input)
	{
		values.push_back(_input);
		potential += values.back();
	}

	template<>
	void Soma::add<NT::Classical>(const LinkPtr& _link)
	{
		add<NT::Classical>(_link->signal<NT::Classical>());
	}

	template<>
	void Soma::add<Enc::Direct>(const LinkPtr& _link)
	{
		/// Compute the current activation level
		/// based on the last evaluation time.
		if (_link->src.conf.net.spike.lif)
		{
			potential *= std::exp( - (_link->src.axon.time - last_event) / _link->tgt.tau.val() );
		}
		potential += _link->signal<NT::Spiking>();
		last_event = _link->src.axon.time;
	}
}

