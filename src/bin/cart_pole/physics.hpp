#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Config.hpp"
#include <fenv.h>

using namespace Cortex;

namespace CartPole
{
	static real g = -9.81; // [m/s^2]

	namespace Rnd
	{
		static std::mutex mtx;
		static std::mt19937_64 rng;
		static std::normal_distribution<real> nd(0.0,0.00001);
		static real jitter()
		{
			return nd(rng);
		}
	};

	namespace Max
	{
		// Maximal force the controller can apply to the cart.
		static real force = 10.0;

		// Maximal offset angle of any pole for any cart
		// as measured from the ordinate.
		static real theta = 36.0;

		// Maximal offset of the cart from the centre of the track
		static real pos = 0.0;

		// Minimal number of timesteps that the controller has to
		// balance the poles for before a solution is declared.
		static uint steps = 200000;
	};

	namespace RK4
	{
		// Time step for RK4 integration.
		static real dt = 0.01; // [s]
		static real dt2;
		static real dt6;
	};

	// cur			current value
	// tmp			temporary value
	// k1 ~ k3		estimated slopes
	struct Param
	{
		real cur;
		real tmp;
		real max;
		real k1;
		real k2;
		real k3;

		Param(const real _init = 0.0,
			  const real _max = 0.0)
			:
			  cur(_init),
			  tmp(_init),
			  max(_max),
			  k1(0.0),
			  k2(0.0),
			  k3(0.0)
		{}
	};

	// theta:		[rad]		pole angle
	// omega:		[rad/s]		pole angular velocity
	// mass		[kg]		pole mass
	// half_len:	[m]			half of the pole length
	// alpha:		[rad/s^2]	pole angular acceleration
	// hfc:					hinge friction coefficient
	//
	// ml:						mass * hlen
	// hml:					hfc / ml
	// coeff:					-0.75 / hlen
	//
	// rk4:					Struct holding the estimated slopes for RK4
	class Pole
	{
	public:

		enum class Var
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
			static real theta; // (-theta, theta) passed to the RNG
			static real omega;
			static real mass; // DPB: 0.1 for long pole, 0.01 for short pole
			static real len; // DPB: 1.0 for long pole, 0.1 for short pole
			static real alpha;
			static real hfc;
		};

		static inline real rnd_theta(const real _min = Max::theta / 10.0,
									 const real _max = Max::theta / 5.0,
									 const bool _rnd_sign = true)
		{
			glock lk(Rnd::mtx);
			static std::uniform_real_distribution<real> theta_dist(rad(_min), rad(_max));
			static std::uniform_real_distribution<real> theta_sign(0.0, 1.0);
			if (_rnd_sign &&
				theta_sign(Rnd::rng) < 0.5)
			{
				return -theta_dist(Rnd::rng);
			}
			return theta_dist(Rnd::rng);
		}

	private:

		Param theta;
		Param omega;
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
			 const real _hfc = Pole::Def::hfc)
			:
			  theta(rad(_theta), rad(_max_theta)),
			  omega(_omega),
			  mass(_mass),
			  half_len(_len / 2.0),
			  alpha(_alpha),
			  hfc(_hfc),
			  ml(0.0),
			  hml(0.0),
			  coeff(0.0),
			  sin_theta(0.0),
			  cos_theta(0.0)
		{
			ml = mass * half_len;
			hml = hfc / ml;
			coeff = -0.75 / half_len;
		}

		inline void init()
		{
			theta.cur = rnd_theta();
		}

		inline void set(const Var _var, const real _val)
		{
			switch (_var)
			{
			case Var::theta: theta.cur = rad(_val); break;
			case Var::max_theta: theta.max = rad(_val); break;
			case Var::omega: omega.cur = _val; break;
			case Var::max_omega: omega.max = _val; break;
			case Var::mass: mass = _val; break;
			case Var::len: half_len = _val / 2.0; break;
			case Var::alpha: alpha = _val; break;
			case Var::hfc: hfc = _val; break;
			}
		}

		inline bool in_range()
		{
			return std::fabs(theta.cur) < theta.max;
		}

		inline real compute_alpha(const real _cart_acc)
		{
			alpha = coeff * (_cart_acc * cos_theta + g * sin_theta + omega.tmp * hml);
			return alpha;
		}

		inline void set_tmp(const real _dt)
		{
			omega.tmp = omega.cur + alpha * _dt;
			theta.tmp = theta.cur + omega.tmp * _dt;
			sin_theta = std::sin(theta.tmp);
			cos_theta = std::cos(theta.tmp);
		}

		// Effective mass
		inline real m_eff()
		{
			return mass * (1.0 - 0.75 * std::pow(cos_theta, 2));
		}

		// Effective force
		inline real f_eff()
		{
			return ml * std::pow(omega.tmp, 2) * sin_theta +
					0.75 * mass * cos_theta * (omega.tmp * hml + g * sin_theta );
		}

		friend std::ostream& operator << (std::ostream& _strm, const Pole& _pole)
		{
			return _strm << "\n\n\tPole:"
						 << "\n\t\tmass: " << _pole.mass << " kg"
						 << "\n\t\tlength: " << _pole.half_len * 2.0 << " m"
						 << "\n\t\toffset angle: " << deg(_pole.theta.cur) << " deg (" << _pole.theta.cur << " rad)"
						 << "\n\t\tangular velocity: " << _pole.omega.cur << " rad/s"
						 << "\n\t\tangular acceleration: " << _pole.alpha << " rad/s^2"
						 << "\n\t\thinge friction coeff.: " << _pole.hfc;
		}
	};

	// pos			[m]			position relative to the centre of the track
	// vel			[m/s]		velocity of the cart
	// mass		[kg]		cart mass
	// acc			[m/s^2]		acceleration of the cart
	// k_l						coefficient of the left spring
	// k_r						coefficient of the right spring
	// tfc						track friction coefficient
	//
	// force		[N]			force applied to the cart
	class Cart
	{
	public:

		enum class Var
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

		static inline real rnd_pos(const real _min = Max::pos / 10.0,
								   const real _max = Max::pos / 5.0,
								   const bool _rnd_sign = true)
		{
			glock lk(Rnd::mtx);
			static std::uniform_real_distribution<real> pos_dist(_min, _max);
			static std::uniform_real_distribution<real> pos_sign(0.0, 1.0);
			if (_rnd_sign &&
				pos_sign(Rnd::rng) < 0.5)
			{
				return -pos_dist(Rnd::rng);
			}
			return pos_dist(Rnd::rng);
		}

	private:

		Param pos;
		Param vel;
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
			 const real _tfc = Cart::Def::tfc)
			:
			  pos(_pos, _max_pos),
			  vel(_vel),
			  mass(_mass),
			  acc(0.0),
			  k_l(_k_l),
			  k_r(_k_r),
			  tfc(_tfc),
			  force(0.0)
		{}

		inline uint pole_count() const
		{
			return poles.size();
		}

		inline void init()
		{
			pos.cur = rnd_pos();
			for (Pole& pole : poles)
			{
				pole.init();
			}
		}

		inline void set(const Var _var, const real _val)
		{
			switch (_var)
			{
			case Var::pos: pos.cur = _val; break;
			case Var::max_pos: pos.max = _val; break;
			case Var::vel: vel.cur = _val; break;
			case Var::max_vel: vel.max = _val; break;
			case Var::mass: mass = _val; break;
			case Var::acc: acc = _val; break;
			case Var::k_l: k_l = _val; break;
			case Var::k_r: k_r = _val; break;
			case Var::tfc: tfc = _val; break;
			}
		}

		inline bool in_range()
		{
			if (std::fabs(pos.cur) > pos.max)
			{
				return false;
			}
			for (Pole& pole : poles)
			{
				if (!pole.in_range())
				{
					return false;
				}
			}
			return true;
		}

		inline real compute_acc()
		{
			// @todo Add coupled oscillator mode.
			// We have to define an equilibrium position
			// depending on the spring constants.
			// real cart_acc(force - tfc * sgn(vel.tmp) - (k_l + k_r) * (pos.tmp - pos.eq));
			acc = force - tfc * sgn(vel.tmp);
			real total_mass(mass);
			for (Pole& pole : poles)
			{
				acc += pole.f_eff();
				total_mass += pole.m_eff();
			}

			acc /= total_mass;
			return acc;
		}

		inline void add_pole(const Pole& _pole)
		{
			poles.emplace_back(_pole);
		}

		inline std::vector<Pole>& get_poles()
		{
			return poles;
		}

		// Set the temporary values for all parameters
		inline void set_tmp(const real _dt)
		{
			vel.tmp = vel.cur + acc * _dt;
			pos.tmp = pos.cur + vel.tmp * _dt;
			for (Pole& p : poles)
			{
				p.set_tmp(_dt);
			}
		}

		// Update the state given the applied force
		// using the RK4 integration method.
		inline void update(const real _action)
		{
			force = _action * Max::force;

			// k1
			set_tmp(0.0);
			vel.k1 = compute_acc();
			pos.k1 = vel.tmp;
			for (Pole& pole : poles)
			{
				pole.omega.k1 = pole.compute_alpha(acc);
				pole.theta.k1 = pole.omega.tmp;
			}

			// k2
			set_tmp(RK4::dt2);
			vel.k2 = compute_acc();
			pos.k2 = vel.tmp;
			for (Pole& pole : poles)
			{
				pole.omega.k2 = pole.compute_alpha(acc);
				pole.theta.k2 = pole.omega.tmp;
			}

			// k3
			set_tmp(RK4::dt2);
			vel.k3 = compute_acc();
			pos.k3 = vel.tmp;
			for (Pole& pole : poles)
			{
				pole.omega.k3 = pole.compute_alpha(acc);
				pole.theta.k3 = pole.omega.tmp;
			}

			// New value
			set_tmp(RK4::dt);
			vel.cur += RK4::dt6 * (vel.k1 + 2.0 * (vel.k2 + vel.k3) + compute_acc());
			pos.cur += RK4::dt6 * (pos.k1 + 2.0 * (pos.k2 + pos.k3) + vel.tmp);
			pos.cur += Rnd::jitter();
			for (Pole& pole : poles)
			{
				pole.omega.cur += RK4::dt6 * (pole.omega.k1 + 2.0 * (pole.omega.k2 + pole.omega.k3) + pole.compute_alpha(acc));
				pole.theta.cur += RK4::dt6 * (pole.theta.k1 + 2.0 * (pole.theta.k2 + pole.theta.k3) + pole.omega.tmp);
				pole.theta.cur += Rnd::jitter();
			}
		}

		friend std::ostream& operator << (std::ostream& _strm, const Cart& _cart)
		{
			_strm << "\n\nCart:"
				  << "\n\toffset: " << _cart.pos.cur << " m"
				  << "\n\tvelocity: " << _cart.vel.cur << " m/s"
				  << "\n\tmass: " << _cart.mass << " kg"
				  << "\n\tacceleration: " << _cart.acc << " m/s^2"
				  << "\n\tleft spring coeff.: " << _cart.k_l
				  << "\n\tright spring coeff.: " << _cart.k_r
				  << "\n\ttrack friction coeff.: " << _cart.tfc
				  << "\n\tforce: " << _cart.force << " N";

			for (const Pole& pole : _cart.poles)
			{
				_strm << pole;
			}
			return _strm;
		}
	};

	// len		[m]		track length
	// steps			time steps since the beginning of the simulation
	class Track
	{
	public:

		struct Def
		{
			// Total track length.
			// Centred at the origin, so carts
			// are limited to (-len/2, len/2)
			static real track_len;

			// Is the controller given information
			// about velocities or just the position?
			static bool with_vel;

			// Coupled oscillator mode.
			// Only valid if carts > 1.
			// @todo Currently unused.
			static bool coupled;

			// Spring constants.
			// @todo Currently unused.
			static std::vector<real> ks;
		};

	private:

		std::vector<Cart> carts;

	public:

		inline void init()
		{
			for (Cart& cart : carts)
			{
				cart.init();
			}
		}

		inline void add_cart(const Cart& _cart)
		{
			carts.emplace_back(_cart);
			// @todo Implement coupled oscillator
		}

		inline std::vector<Cart>& get_carts()
		{
			return carts;
		}

		inline void update(const std::vector<real>& _actions)
		{
			for (uint idx = 0; idx < carts.size(); ++idx)
			{
				carts[idx].update(_actions[idx]);
			}
		}

		inline std::string as_csv(const std::vector<real>& _v) const
		{
			if (_v.size() == 0)
			{
				return "";
			}
			else if (_v.size() == 1)
			{
				return std::to_string(_v.front());
			}
			else
			{
				return std::accumulate(std::next(_v.begin()),
									   _v.end(),
									   std::to_string(_v.front()),
									   [](std::string _cur, const real _s)
				{
					return _cur + "," + std::to_string(_s);
				});
			}
		}

		inline real restrict(const real _theta)
		{
			real theta(fmod(_theta + pi, two_pi));
			if (theta < 0.0)
			{
				theta += two_pi;
			}
			return theta - pi;
		}

		inline std::vector<real> state()
		{
			std::vector<real> s;
			for (Cart& cart : carts)
			{
				s.emplace_back(cart.pos.cur);
				if (Track::Def::with_vel)
				{
					s.emplace_back(cart.vel.cur);
				}
				for (Pole& pole : cart.poles)
				{
					s.emplace_back(restrict(pole.theta.cur));
					if (Track::Def::with_vel)
					{
						s.emplace_back(pole.omega.cur);
					}
				}
			}
			return s;
		}

		inline std::vector<real> norm_state()
		{
			std::vector<real> s;
			for (Cart& cart : carts)
			{
				s.emplace_back(cart.pos.cur / cart.pos.max);
				if (Track::Def::with_vel)
				{
					s.emplace_back(cart.vel.cur);
				}
				for (Pole& pole : cart.poles)
				{
					s.emplace_back(restrict(pole.theta.cur) / pole.theta.max);
					if (Track::Def::with_vel)
					{
						s.emplace_back(pole.omega.cur);
					}
				}
			}
			return s;
		}

		inline bool in_range()
		{
			for (Cart& cart : carts)
			{
				if (!cart.in_range())
				{
					return false;
				}
			}
			return true;
		}

		inline uint cart_count()
		{
			return carts.size();
		}

		inline uint pole_count()
		{
			uint poles(0);

			for (Cart& cart : carts)
			{
				poles += cart.pole_count();
			}
			return poles;
		}

		friend std::ostream& operator << (std::ostream& _strm, const Track& _env)
		{
			_strm.setf(std::ios_base::boolalpha);

			_strm << "\n-------------------- Env -------------------------\n";
			_strm << "\n\nMax:"
				  << "\n\tforce: " << Max::force << " N"
				  << "\n\toffset angle: " << Max::theta << " deg (" << rad(Max::theta) << " rad)"
				  << "\n\tcart offset: " << Max::pos << " m"
				  << "\n\tsteps: " << Max::steps
				  << "\n\nRK4:"
				  << "\n\tdt: " << RK4::dt << " s"
				  << "\n\tGravitational acceleration: " << g << " m/s^2"
				  << "\n\tTrack length: " << Track::Def::track_len << " m"
				  << "\n\tProvide velocity information: " << Track::Def::with_vel
				  << "\n\tCoupled oscillator mode: " << Track::Def::coupled
				  << "\n\tSpring coefficients:" << _env.as_csv(Track::Def::ks);

			for (const Cart& cart : _env.carts)
			{
				_strm << cart;
			}
			_strm << "\n--------------------------------------------------\n";
			return _strm;
		}
	};

	namespace Gen
	{
		enum class Var
		{
			pos,
			theta,
			vel,
			omega
		};
		static std::vector<real> pos;
		static std::vector<real> theta;
		static std::vector<real> vel;
		static std::vector<real> omega;
		static std::vector<Var> vars;
		static std::vector<Track> tracks;
	};
}

#endif // PHYSICS_HPP
