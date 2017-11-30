#ifndef PHENOME_HPP
#define PHENOME_HPP

#include "Globals.hpp"

namespace Cortex
{
	class Phenome
	{
	private:

		enum class Mark
		{
			Temp,
			Perm
		};

		emap<uint, Mark> marks;
		std::vector<uint> graph;

		//	Lookup tables for forward and recurrent links
		emap<LT, LinkLUT> tables;

		inline bool visit(const uint _node,
						  const bool _update_graph)
		{
//			dlog() << "Visiting node " << _node;
			if (marks.find(_node) != marks.end())
			{
				switch (marks.at(_node))
				{
					case Mark::Perm: return true;
					case Mark::Temp: return false; // Not a DAG
				}
			}
			else
			{
				marks[_node] = Mark::Temp;
				for (const auto& tgt : tables.at(LT::F).at(_node))
				{
					if (!visit(tgt, _update_graph))
					{
						return false;
					}
				}
				marks[_node] = Mark::Perm;
				if (_update_graph)
				{
					graph.push_back(_node);
				}
				return true;
			}
		}

	public:

		Phenome()
		{
			tables[LT::F] = LinkLUT();
			tables[LT::R] = LinkLUT();
			tables[LT::S] = LinkLUT();
		}

		inline void insert(const uint _id)
		{
			for (auto& t : tables)
			{
				t.second[_id] = {};
			}
		}

		inline void add_link(const LT _lt,
							 const uint _src,
							 const uint _tgt)
		{
			tables[_lt][_src].insert(_tgt);
			if (_lt == LT::F)
			{
				tables[LT::S][_tgt].insert(_src);
			}
		}

		inline void erase_link(const LT _lt,
							   const uint _src,
							   const uint _tgt)
		{
			tables[_lt][_src].erase(_tgt);
			if (_lt == LT::F)
			{
				tables[LT::S][_tgt].erase(_src);
			}
		}

		inline void erase_node(const uint _nrn_id)
		{
			// Adjust the IDs of nodes with IDs *higher*
			// than that of the node to be deleted.
			// ltype.first = LT
			// ltype.second = LinkLUT
			for (auto& ltype : tables)
			{
				LinkLUT& lut(ltype.second);
				for (auto& links : lut)
				{
					links.second.erase(_nrn_id);
					for (auto it = links.second.begin(); it != links.second.end(); )
					{
						if (*it > _nrn_id)
						{
							links.second.insert(*it - 1);
							it = links.second.erase(it);
						}
						else
						{
							++it;
						}
					}
				}

				// Erase the node.
				lut.erase(_nrn_id);

				// Shift node IDs down by 1.
				for (auto it = lut.find(_nrn_id + 1); it != lut.end(); )
				{
					lut[it->first - 1] = it->second;
					it = lut.erase(it);
				}
			}
		}

		inline void clear()
		{
			for (auto& t : tables)
			{
				t.second.clear();
			}
		}

		inline const auto& get() const
		{
			return tables;
		}

		inline const auto& get(const LT _t) const
		{
			return tables.at(_t);
		}

		inline const auto& get(const LT _t, const uint _id) const
		{
			return tables.at(_t).at(_id);
		}

		inline bool is_empty(const LT _lt, const uint _id) const
		{
			return tables.at(_lt).at(_id).empty();
		}

		inline bool exists(const LT _lt, const uint _id) const
		{
			return tables.at(_lt).find(_id) != tables.at(_lt).end();
		}

		inline bool exists(const LT _lt, const uint _src, const uint _tgt) const
		{
			return tables.at(_lt).at(_src).find(_tgt) != tables.at(_lt).at(_src).end();
		}

		inline bool exists(const uint _src, const uint _tgt) const
		{
			return ( exists(LT::F, _src, _tgt) || exists(LT::R, _src, _tgt) );
		}

		inline uint link_count(const LT _lt) const
		{
			// Sum links
			return std::accumulate(tables.at(_lt).begin(), tables.at(_lt).end(), 0, [](uint _sum, const auto& _it)
			{
				return _sum + _it.second.size();
			});
		}

		inline uint link_count() const
		{
			return (link_count(LT::F) + link_count(LT::R));
		}

		inline bool is_allowed(const LT _lt, const uint _src, const uint _tgt)
		{
			switch (_lt)
			{
				case LT::F:
					return (!exists(_lt, _src, _tgt) && !forms_cycle(_src, _tgt));

				case LT::R:
					return !exists(_lt, _src, _tgt);

				default:
					return false;
			}
		}

		inline bool is_eraseable(const LT _lt, const uint _src, const uint _tgt)
		{
			switch (_lt)
			{
				case LT::F:
					return (exists(_lt, _src, _tgt) &&
							get(LT::F, _src).size() > 1 &&
							get(LT::S, _tgt).size() > 1);

				case LT::R:
					return exists(_lt, _src, _tgt);

				default:
					return false;
			}
		}

		// Check if a path from one node to another already exists.
		// Used when trying to add a link which might turn out to be recurrent.
		inline bool forms_cycle(const uint _src, const uint _tgt)
		{
			add_link(LT::F, _src, _tgt);

			bool cycle(!make_graph(false));

			erase_link(LT::F, _src, _tgt);

			return cycle;
		}

		// Topologically sorted graph constructed
		// using depth-first search
		inline bool make_graph(const bool _update_graph = true)
		{
			if (_update_graph)
			{
				graph.clear();
			}
			marks.clear();

			for (const auto& n_id : tables.at(LT::F))
			{
				if (marks.find(n_id.first) == marks.end() &&
					!visit(n_id.first, _update_graph))
				{
					return false;
				}
			}

			// The graph is constructed backwards,
			// so we need to reverse it.
			if (_update_graph &&
				!graph.empty())
			{
				std::reverse(graph.begin(), graph.end());
			}
//			dlog() << "Graph created.";
			return true;
		}

		inline const std::vector<uint>& get_graph() const
		{
			return graph;
		}
	};
}

#endif // PHENOME_HPP
