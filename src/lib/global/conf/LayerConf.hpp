#ifndef CORTEX_LAYERCONF_HPP
#define CORTEX_LAYERCONF_HPP

#include "ConvConf.hpp"

namespace Cortex
{
	class LayerConf final : public ConfBase
	{

	public:

		/// @brief Self-explanatory
		LayerType type;

		/// @brief Number of input nodes
		uint in;

		/// @brief Number of output nodes
		uint out;

		/// @brief Convolutional parameters
		ConvConf conv;

	public:

		LayerConf(Conf& _conf);

		virtual void set_defaults() override final;

		virtual void validate() override final;

	private:

		template<LayerType lt>
		void validate();

		friend std::ostream& operator << (std::ostream& _strm, const LayerConf& _conf);
	};
}

#endif // CORTEX_LAYERCONF_HPP
