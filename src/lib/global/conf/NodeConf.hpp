#ifndef CORTEX_NODECONF_HPP
#define CORTEX_NODECONF_HPP

#include "ParamConf.hpp"
#include "StatConf.hpp"

namespace Cortex
{
	class NodeConf final : public ConfBase
	{
	public:

		/// Node roles and corresponding counts
		hmap<NR, uint> roles;

		/// Transfer functions allowed for different roles
		hmap<NR, hset<TF>> tf;

		/// Parameter configuration for
		/// membrane time constant
		ParamConf tau;

		/// Input statistics
		StatConf soma;

		/// Output statistics
		StatConf axon;

	public:

		NodeConf(Conf& _conf);

		TF rnd_tf(const NR _nr);

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const NodeConf& _conf);
	};
}

#endif // CORTEX_NODECONF_HPP
