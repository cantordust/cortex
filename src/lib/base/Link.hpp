#ifndef CORTEX_LINK_HPP
#define CORTEX_LINK_HPP

#include "Param.hpp"

namespace Cortex
{
	struct Link
	{
		/// @brief Always refers to the node in which
		/// the link was created.
		Node& src;

		/// @brief The target node.
		/// A reference to this link
		/// is also stored in the target node.
		Node& tgt;

		/// @brief Link type (forward or recurrent)
		LT type;

		/// @brief Synaptic weight
		Param weight;

		Link() = delete;

		Link(const Link& _other) = delete;

		Link(Link&& _other) = delete;

		explicit Link(Node& _src, Node& _tgt, const LT _type);

		explicit Link(Node& _src, Node& _tgt, const Link& _other);

		template<NT nt>
		void ltp();

		template<NT nt>
		void ltd();

		template<NT nt>
		real signal();

		friend std::ostream& operator<< (std::ostream& _strm, const Link& _link);
	};

	struct Links
	{
		hmap<NodePtr, LinkPtr> sources;
		hmap<NodePtr, LinkPtr> targets;

		void add(const NodePtr& _src, const NodePtr& _tgt, const LT _lt);

		void add(const NodePtr& _src, const NodePtr& _tgt, const LinkPtr& _other);

		void erase(const NodePtr& _src, const NodePtr& _tgt);
	};
}

#endif // CORTEX_LINK_HPP
