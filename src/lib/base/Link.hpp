#ifndef LINK_HPP
#define LINK_HPP

#include "Config.hpp"
#include "Globals.hpp"
#include "Param.hpp"

namespace Cortex
{
	struct Link
	{
		// Link type (forward or recurrent)
		LT type;

		// Always refers to the node in which
		// the link was created.
		Node& src;

		// The target node.
		// Only a reference_wrapper to this link
		// is stored at the target node.
		Node& tgt;

		// Parameters subject to mutation
		Param weight;

		Link() = delete;

		Link(const Link& _other) = delete;

		Link(Link&& _other) = delete;

		explicit Link(Node& _src, Node& _tgt, const LT _type, ConfigRef& _cfg);

		explicit Link(Node& _src, Node& _tgt, const Link& _other);

		void add_to_target();

		void remove_from_target();

		friend std::ostream& operator<< (std::ostream& _strm, const Link& _link);
	};

	struct NodeID;

	struct Links
	{
		emap<LT, emap<NR, hmap<uint, LinkPtr>>> targets;
		emap<LT, emap<NR, hmap<uint, LinkRef>>> sources;

		Links();

		void add(const LT _lt, Node& _src, Node& _tgt, ConfigRef& _cfg);

		void add(const LT _lt, Node& _src, Node& _tgt, const Link& _other);

		void erase(const LT _lt, const NodeID& _id);

		void reindex(const NodeID& _id);
	};
}

#endif // LINK_HPP
