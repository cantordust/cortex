#ifndef CORTEX_LINKCONF_HPP
#define CORTEX_LINKCONF_HPP

#include "ParamConf.hpp"

namespace Cortex
{
	class LinkConf final
	{

	public:

		Conf& conf;

		/// Allow / disallow recurrent links
		bool rec;

		/// Allowed link types
		hset<LinkDef> defs;

		/// Parameter configuration for link weights
		ParamConf weight;

	public:

		LinkConf(Conf& _conf);

		hset<LinkDef> by_type(const LT _lt);

		hset<LinkDef> by_src(const NR _nr, const LT _lt = LT::Undef);

		hset<LinkDef> by_tgt(const NR _nr, const LT _lt = LT::Undef);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const LinkConf& _conf);
	};
}

#endif // CORTEX_LINKCONF_HPP
