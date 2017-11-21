#ifndef GRF_HPP
#define GRF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class GRF
	{
		private:

			real mu;
			real denom; /// = 2 * sigma^2

		public:

			GRF()
				:
				  mu(0.0),
				  denom(0.0)
			{}

			GRF(const uint _N, /// Number of receptive fields
				const uint _i, /// Index of the current node
				const real _beta, /// Gaussian spread parameter
				const real _i_min, /// Minimal value of the variable
				const real _i_max) /// Maximal value of the variable
				:
				  mu(_i_min + (2.0 * _i - 3.0) / ((2.0 * _N)*(_i_max - _i_min))),
				  denom(2.0 * std::pow((_i_max - _i_min) / (_beta * (_N - 2.0)), 2))
			{}

			inline real get_delay(const real _x)
			{
				return std::exp(- std::pow(_x - mu, 2) / denom);
			}

			inline real get_mu() const
			{
				return mu;
			}

			inline real get_denom() const
			{
				return denom;
			}
	};

}

#endif // GRF_HPP

