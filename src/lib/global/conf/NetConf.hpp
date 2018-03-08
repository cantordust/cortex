#ifndef CORTEX_NETCONF_HPP
#define CORTEX_NETCONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class NetConf final
	{
	public:

		Conf& conf;

		/// Network type.
		/// For now, this is limited to classical or spiking.
		/// @todo Boltzmann machines?
		NT type;

		struct RFConf
		{
			/// Receptive field type
			RF type;

			/// Number of receptor nodes (per variable)
			uint size;

			struct
			{
				/// Cutoff threshold for spike delays.
				real cutoff;

				/// This controls the width of GRFs
				real beta;
			} grf;
		} rf;

		struct SpikeConf
		{
			/// The activation level of LIF spiking nodes
			/// decays over time, whereas non-LIF nodes
			/// maintain their activation indefinitely.
			bool lif;

			/// The type of spike encoding determines
			/// the efficiency of information processing.
			Enc enc;

			/// Coefficient for rank order encoding
			real mod;

			struct
			{
				/// Maximal spike delay
				real delay;
			} max;
		} spike;

		struct
		{
			/// The maximal number of generations that a
			/// network is allowed to exist for.
			/// Set to 0 to disable.
			uint age;

		} max;

		/// @todo Topology
		/// @todo Substrates

	private:

		uint id;

	public:

		NetConf(Conf& _conf);

		/// @brief Obtain the next available net ID.
		uint next_id();

		/// @brief Reset the species ID.
		void reset_id();

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const NetConf& _conf);
	};
}

#endif // CORTEX_NETCONF_HPP
