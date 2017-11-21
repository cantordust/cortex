#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	Link::Link(Node& _src, Node& _tgt, const LT _type, Config& _cfg)
		:
		  type(_type),
		  src(_src),
		  tgt(_tgt),
		  weight(_cfg, _cfg.link.weight)
	{
		add_to_target();
	}

	Link::Link(Node& _src, Node& _tgt, const Link& _other)
		:
		  type(_other.type),
		  src(_src),
		  tgt(_tgt),
		  weight(_other.weight)
	{
		add_to_target();
	}

	void Link::add_to_target()
	{
		tgt.links.sources.at(type).at(src.id.role).emplace(src.id.idx, *this);
	}

	void Link::remove_from_target()
	{
		tgt.links.sources.at(type).at(src.id.role).erase(src.id.idx);
	}

	/// Links methods
	Links::Links()
	{
		/// Iterate over link types
		for (const auto& ltype : Enum<LT>::entries)
		{
			for (const auto& nrole : Enum<NR>::entries)
			{
				/// Iterate over source-target
				/// role pairs for this link type
				targets[ltype.first][nrole.first] = hmap<uint, LinkPtr>();
				sources[ltype.first][nrole.first] = hmap<uint, LinkRef>();
			}
		}
	}

	void Links::add(const LT _lt, Node& _src, Node& _tgt, Config& _cfg)
	{
		targets.at(_lt).at(_tgt.id.role).emplace(_tgt.id.idx, std::make_unique<Link>(_src, _tgt, _lt, _cfg));
	}

	void Links::add(const LT _lt, Node& _src, Node& _tgt, const Link& _other)
	{
		targets.at(_lt).at(_tgt.id.role).emplace(_tgt.id.idx, std::make_unique<Link>(_src, _tgt, _other));
	}

	void Links::erase(const LT _lt, const NodeID& _id)
	{
		/// Remove the link ref from the target
		targets.at(_lt).at(_id.role).at(_id.idx)->remove_from_target();

		/// Remove the actual link
		targets.at(_lt).at(_id.role).erase(_id.idx);
	}

	void Links::reindex(const NodeID& _id)
	{
		for (auto& ltype : targets)
		{
			hmap<uint, LinkPtr> tmp_map;
			for (auto it = ltype.second.at(_id.role).begin(); it != ltype.second.at(_id.role).end(); )
			{
				if (it->first > _id.idx)
				{
					tmp_map.emplace(it->first - 1, std::move(it->second));
					it = ltype.second.at(_id.role).erase(it);
				}
				else
				{
					++it;
				}
			}

			for (auto& lnk : tmp_map)
			{
				ltype.second.at(_id.role).emplace(lnk.first, std::move(lnk.second));
			}
		}

		for (auto& ltype : sources)
		{
			hmap<uint, LinkRef> tmp_map;
			for (auto it = ltype.second.at(_id.role).begin(); it != ltype.second.at(_id.role).end(); )
			{
				if (it->first > _id.idx)
				{
					tmp_map.emplace(it->first - 1, it->second);
					it = ltype.second.at(_id.role).erase(it);
				}
				else
				{
					++it;
				}
			}

			for (auto& lnk : tmp_map)
			{
				ltype.second.at(_id.role).emplace(lnk.first, lnk.second);
			}
		}
	}
}
