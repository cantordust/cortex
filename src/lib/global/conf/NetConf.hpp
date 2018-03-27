#ifndef CORTEX_NETCONF_HPP
#define CORTEX_NETCONF_HPP

#include "LayerConf.hpp"

namespace Cortex
{
	class NetConf final : public ConfBase
	{
	public:

		/// Network type.
		/// For now, this is limited to classical or spiking.
		///
		/// @todo Move check for recurrent connections from LinkConf here
		/// @todo Boltzmann machines?
		/// @todo LSTM?
		NT type;

		/// Network layout
		Layout layout;

		/// Connectivity (how many saturation)
		///
		/// @todo Use this parameter when connecting networks.
		real conn;

		/// Network layers (only for layered layout)
		hmap<NR, std::vector<LayerConf>> layers;

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
		/// @note WIP (layers and weights implemented using Armadillo)
		///
		/// @todo Substrates - link with topology?

	private:

		uint id;

	public:

		NetConf(Conf& _conf);

		/// @brief Obtain the next available net ID.
		uint next_id();

		/// @brief Reset the species ID.
		void reset_id();

		virtual void set_defaults() override final;

		virtual void validate() override final;

		friend std::ostream& operator << (std::ostream& _strm, const NetConf& _conf);
	};
}

#endif // CORTEX_NETCONF_HPP
