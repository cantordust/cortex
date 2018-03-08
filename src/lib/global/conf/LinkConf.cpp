#include "Conf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, LinkConf& _c)
	{
		load(_j, "weight", _c.weight);
		load(_j, "rec", _c.rec);
		load(_j, "defs", _c.defs);
	}

	LinkConf::LinkConf(Conf& _conf)
		:
		  conf(_conf),
		  weight(_conf)
	{
		set_defaults();
	}

	hset<LinkDef> LinkConf::by_type(const LT _lt)
	{
		hset<LinkDef> ldefs;
		for (const auto& def : defs)
		{
			if (def.lt == _lt)
			{
				ldefs.emplace(def);
			}
		}
		return ldefs;
	}

	hset<LinkDef> LinkConf::by_src(const NR _nr, const LT _lt)
	{
		hset<LinkDef> ldefs;
		for (const auto& def : defs)
		{
			if ((_nr == NR::Undef || def.sr == _nr) &&
				(_lt == LT::Undef || def.lt == _lt))
			{
				ldefs.emplace(def);
			}
		}
		return ldefs;
	}

	hset<LinkDef> LinkConf::by_tgt(const NR _nr, const LT _lt)
	{
		hset<LinkDef> ldefs;
		for (const auto& def : defs)
		{
			if ((_nr == NR::Undef || def.tr == _nr) &&
				(_lt == LT::Undef || def.lt == _lt))
			{
				ldefs.emplace(def);
			}
		}
		return ldefs;
	}

	void LinkConf::set_defaults()
	{
		weight.lbound = -5.0;
		weight.ubound = 5.0;

		rec = false;

		defs =
		{
			{LT::Fwd, NR::B, NR::H},
			{LT::Fwd, NR::B, NR::O},
			{LT::Fwd, NR::I, NR::H},
			{LT::Fwd, NR::I, NR::O},
			{LT::Fwd, NR::H, NR::H},
			{LT::Fwd, NR::H, NR::O},
			{LT::Rec, NR::I, NR::I},
			{LT::Rec, NR::O, NR::I},
			{LT::Rec, NR::O, NR::H},
			{LT::Rec, NR::O, NR::O},
			{LT::Rec, NR::H, NR::I},
			{LT::Rec, NR::H, NR::H}
		};
	}

	std::string LinkConf::validate()
	{
		std::stringstream problems;

		problems << weight.validate();

		if (!rec)
		{
			for (auto it = defs.begin(); it != defs.end();)
			{
				(it->lt == LT::Rec) ? it = defs.erase(it) : ++it;
			}
		}

		return problems.str();
	}

	std::ostream& operator << (std::ostream& _strm, const LinkConf& _conf)
	{
		_strm << "\n--- Links ---"
			  << "\nrec: " << _conf.rec
			  << "\ntypes:\n";

		for (const auto& lid : _conf.defs)
		{
			_strm << "\n\t" << lid;
		}

		_strm << "\nweight:\n" << _conf.weight
			  << "\n";

		return _strm;
	}
}
