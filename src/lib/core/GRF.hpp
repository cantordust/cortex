#ifndef CORTEX_GRF_HPP
#define CORTEX_GRF_HPP

#include "Conf.hpp"

namespace Cortex
{
	/// @class Gaussian receptive fields for a single variable.
	struct GRF
	{
		std::vector<real> mu;
		real denom; /// = 2 * sigma^2

		GRF(const std::vector<real>& _mu,
			const real _denom)
			:
			  mu(_mu),
			  denom(_denom)
		{}

		std::vector<uint> convert(const real _x) const
		{
			std::vector<uint> active_nodes;
			for (uint i = 0; i < mu.size(); ++i)
			{
				if (1.0 - std::exp(- std::pow(_x - mu[i], 2) / denom) > conf.net.sensors.gaussian.threshold)
				{
					active_nodes.emplace_back(i);
				}
			}
			return active_nodes;
		}
	};
}

#endif // CORTEX_GRF_HPP

