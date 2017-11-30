#ifndef GRF_HPP
#define GRF_HPP

#include "Config.hpp"

namespace Cortex
{
	class GRF
	{
	private:

		real mu;
		real denom; // = 2 * sigma^2

		std::vector<real> means;

	public:

		GRF() {}

		GRF(const ConfigRef& _cfg,
			const uint _idx)
			:
			  mu(( 0.5 * (2.0 * _idx - 3.0) / _cfg.get().net.spiking.receptors)),
			  denom(2.0 * std::pow(1.0 / (_cfg.get().net.spiking.beta * (_cfg.get().net.spiking.receptors - 2.0)), 2))
		{}

		inline real convert(const real _x)
		{
			 return std::exp(- std::pow(_x - mu, 2) / denom);
		}
	};

}

#endif // GRF_HPP

