#ifndef CORTEX_GRF_HPP
#define CORTEX_GRF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class GRF
	{
	private:

		Vec mu;
		real denom; /// = 2 * sigma^2

	public:

		GRF() {}

		GRF(const real _mu,
			const real _denom)
			:
			  mu(_mu),
			  denom(_denom)
		{}

		Vec convert(const real _x) const
		{
			 return 1.0 - arma::exp(- arma::pow(Vec(mu.n_elem).fill(_x) - mu, 2) / denom);
		}
	};
}

#endif // CORTEX_GRF_HPP
