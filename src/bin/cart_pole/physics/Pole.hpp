#ifndef POLE_HPP
#define POLE_HPP

#include "Parameter.hpp"
#include "Vars.hpp"

namespace CartPole
{
	/// theta:		[rad]		Angle
	/// omega:		[rad/s]		Angular velocity
	/// mass		[kg]		Mass
	/// half_len:	[m]			Half of the pole length
	/// alpha:		[rad/s^2]	Angular acceleration
	/// hfc:					Hinge friction coefficient
	///
	/// ml:						= mass  * half_len
	/// hml:					= hfc / ml
	/// coeff:					= -0.75 / hlen
	class Pole
	{
	public:

		enum class Var : uchar
		{
			theta,
			max_theta,
			omega,
			max_omega,
			mass,
			len,
			alpha,
			hfc
		};

		struct Def
		{
			static real theta; /// (-theta, theta) passed to the RNG
			static real omega;
			static real mass; /// DPB: 0.1 for long pole, 0.01 for short pole
			static real len; /// DPB: 1.0 for long pole, 0.1 for short pole
			static real alpha;
			static real hfc;
		};

	private:

		Parameter theta;
		Parameter omega;
		real mass;
		real half_len;
		real alpha;
		real hfc;

		real ml;
		real hml;
		real coeff;

		real sin_theta;
		real cos_theta;

		friend class Cart;
		friend class Track;

	public:

		Pole(const real _theta = Pole::Def::theta,
			 const real _max_theta = Max::theta,
			 const real _omega = Pole::Def::omega,
			 const real _mass = Pole::Def::mass,
			 const real _len = Pole::Def::len,
			 const real _alpha = Pole::Def::alpha,
			 const real _hfc = Pole::Def::hfc);

		static real rnd_theta(const real _min = Max::theta / 10.0,
							  const real _max = Max::theta / 5.0,
							  const bool _rnd_sign = true);

		void init();

		void set_var(const Var _var, const real _val);

		bool is_in_range();

		real compute_alpha(const real _cart_acc);

		void set_tmp(const real _dt);

		/// Effective mass
		real m_eff();

		/// Effective force
		real f_eff();

		friend os& operator<< (os& _os, const Pole& _pole);
	};
}

#endif // POLE_HPP
