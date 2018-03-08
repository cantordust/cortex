#ifndef VARS_HPP
#define VARS_HPP

#include "Cortex.hpp"

namespace CartPole
{
	static real g = -9.81; /// [m/s^2]

	namespace Rnd
	{
		static std::mutex mtx;
		static std::mt19937_64 rng;
		static std::normal_distribution<real> nd(0.0,0.00001);
		real jitter();
	};

	namespace Max
	{
		/// Maximal force the controller can apply to the cart.
		static real force = 10.0;

		/// Maximal offset angle of any pole for any cart
		/// as measured from the ordinate.
		static real theta = 36.0;

		/// Maximal offset of the cart from the centre of the track
		static real pos = 0.0;

		/// Minimal number of timesteps that the controller has to
		/// balance the poles for before a solution is declared.
		static uint steps = 200000;
	};

	namespace RK4
	{
		/// Time step for RK4 integration.
		static real dt = 0.01; /// [s]
		static real dt2;
		static real dt6;
	};

}

#endif // VARS_HPP
