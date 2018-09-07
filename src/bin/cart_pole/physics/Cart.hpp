#ifndef CART_HPP
#define CART_HPP

#include "Pole.hpp"

namespace CartPole
{
	/// pos			[m]			Position (relative to the centre of the track)
	/// vel			[m/s]		Velocity
	/// mass		[kg]		Mass
	/// acc			[m/s^2]		Acceleration
	/// k_l						Spring constant of the left spring
	/// k_r						Spring constant of the right spring
	/// tfc						Track friction coefficient
	///
	/// force		[N]			Force applied to the cart
	///
	/// poles					Poles attached to this cart
	class Cart
	{
	public:

		enum class Var : uchar
		{
			pos,
			max_pos,
			vel,
			max_vel,
			mass,
			acc,
			k_l,
			k_r,
			tfc
		};

		struct Def
		{
			static real pos;
			static real vel;
			static real mass;
			static real acc;
			static real k_l;
			static real k_r;
			static real tfc;
		};

	private:

		Parameter pos;
		Parameter vel;
		real mass;
		real acc;
		real k_l;
		real k_r;
		real tfc;

		real force;

		std::vector<Pole> poles;

		friend class Track;

	public:

		Cart(const real _pos = Cart::Def::pos,
			 const real _max_pos = Max::pos,
			 const real _vel = Cart::Def::vel,
			 const real _mass = Cart::Def::mass,
			 const real _acc = Cart::Def::acc,
			 const real _k_l = Cart::Def::k_l,
			 const real _k_r = Cart::Def::k_r,
			 const real _tfc = Cart::Def::tfc);

		static real rnd_pos(const real _min = Max::pos / 10.0,
								   const real _max = Max::pos / 5.0,
								   const bool _rnd_sign = true);

		uint pole_count() const;

		void init();

		void set_var(const Var _var, const real _val);

		bool is_in_range();

		real compute_acc();

		void add_pole(const Pole& _pole);

		std::vector<Pole>& get_poles();

		/// Set the temporary values for all parameters
		void set_tmp(const real _dt);

		/// Update the state given the applied force
		/// using the RK4 integration method.
		void update(const real _action);

		friend os& operator << (os& _os, const Cart& _cart);
	};
}

#endif /// CART_HPP
