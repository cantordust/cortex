#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Axon.hpp"
#include "Soma.hpp"
#include "Link.hpp"

namespace Cortex
{

	template<>
	void Node::receive_input<NT::Classical>()
	{
		for (const auto& link : links.sources)
		{
			if (link.second->src.axon.val.last != 0.0)
			{
				soma.add<NT::Classical>(link.second);
			}
		}
	}

	template<>
	void Node::receive_input<Enc::Direct>(const LinkPtr& _link)
	{
		soma.add<Enc::Direct>(_link);
	}

	/// @todo Implementation
	template<>
	void Node::receive_input<Enc::RankOrder>(const LinkPtr& _link)
	{

	}

	template<>
	void Node::eval<NT::Classical>()
	{
		/// Receive input from source nodes
		receive_input<NT::Classical>();

		/// Pass the input to the node through
		/// the transfer function to compute the output
		transfer<NT::Classical>();

		/// Clear the input in preparation
		/// for the next evaluation
		soma.reset();
	}

	template<>
	void Node::eval<Enc::Direct>(const LinkPtr& _link)
	{
		/// Receive input from source nodes.
		receive_input<Enc::Direct>(_link);

		/// Schedule the target nodes for evaluation
		/// if the activation has crossed the threshold.
		transfer<Enc::Direct>();
	}

	/// @todo Implementation.
	template<>
	void Node::eval<Enc::RankOrder>(const LinkPtr& _link)
	{
		receive_input<Enc::RankOrder>(_link);
	}
}
