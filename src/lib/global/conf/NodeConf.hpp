#ifndef CORTEX_NODECONF_HPP
#define CORTEX_NODECONF_HPP

#include "ParamConf.hpp"
#include "Stat.hpp"

namespace Cortex
{
	class NodeConf final
	{
	public:

		Conf& conf;

		/// Node roles and corresponding counts
		emap<NR, uint> roles;

		/// Transfer functions allowed for different roles
		emap<NR, eset<TF>> tf;

		/// Parameter configuration for
		/// membrane time constant
		ParamConf tau;

		/// Axon statistics
		Stat axon;

		/// Soma statistics
		Stat soma;

	public:

		NodeConf(Conf& _conf);

		TF rnd_tf(const NR _nr);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const NodeConf& _conf);
	};
}

#endif // CORTEX_NODECONF_HPP
