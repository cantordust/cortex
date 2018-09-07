#include "Cart.hpp"

namespace CartPole
{
	Cart::Cart(const real _pos,
			   const real _max_pos,
			   const real _vel,
			   const real _mass,
			   const real _acc,
			   const real _k_l,
			   const real _k_r,
			   const real _tfc)
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

	real Cart::rnd_pos(const real _min, const real _max, const bool _rnd_sign)
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

	uint Cart::pole_count() const
	{
		return poles.size();
	}

	void Cart::init()
	{
		pos.cur = rnd_pos();
		for (Pole& pole : poles)
		{
			pole.init();
		}
	}

	void Cart::set_var(const Cart::Var _var, const real _val)
	{
		switch (_var)
		{
		case Var::pos:
			pos.cur = _val;
			break;

		case Var::max_pos:
			pos.max = _val;
			break;

		case Var::vel:
			vel.cur = _val;
			break;

		case Var::max_vel:
			vel.max = _val;
			break;

		case Var::mass:
			mass = _val;
			break;

		case Var::acc:
			acc = _val;
			break;

		case Var::k_l:
			k_l = _val;
			break;

		case Var::k_r:
			k_r = _val;
			break;

		case Var::tfc:
			tfc = _val;
			break;

		default:
			break;
		}
	}

	bool Cart::is_in_range()
	{
		if (std::fabs(pos.cur) > pos.max)
		{
			return false;
		}
		for (Pole& pole : poles)
		{
			if (!pole.is_in_range())
			{
				return false;
			}
		}
		return true;
	}

	real Cart::compute_acc()
	{
		/// @todo Add coupled oscillator mode.
		/// We have to define an equilibrium position
		/// depending on the spring constants.
		/// real cart_acc(force - tfc * sgn(vel.tmp) - (k_l + k_r) * (pos.tmp - pos.eq));
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

	void Cart::add_pole(const Pole& _pole)
	{
		poles.push_back(_pole);
	}

	std::vector<Pole>&Cart::get_poles()
	{
		return poles;
	}

	void Cart::set_tmp(const real _dt)
	{
		vel.tmp = vel.cur + acc * _dt;
		pos.tmp = pos.cur + vel.tmp * _dt;
		for (Pole& p : poles)
		{
			p.set_tmp(_dt);
		}
	}

	void Cart::update(const real _action)
	{
		force = _action * Max::force;

		/// k1
		set_tmp(0.0);
		vel.k1 = compute_acc();
		pos.k1 = vel.tmp;
		for (Pole& pole : poles)
		{
			pole.omega.k1 = pole.compute_alpha(acc);
			pole.theta.k1 = pole.omega.tmp;
		}

		/// k2
		set_tmp(RK4::dt2);
		vel.k2 = compute_acc();
		pos.k2 = vel.tmp;
		for (Pole& pole : poles)
		{
			pole.omega.k2 = pole.compute_alpha(acc);
			pole.theta.k2 = pole.omega.tmp;
		}

		/// k3
		set_tmp(RK4::dt2);
		vel.k3 = compute_acc();
		pos.k3 = vel.tmp;
		for (Pole& pole : poles)
		{
			pole.omega.k3 = pole.compute_alpha(acc);
			pole.theta.k3 = pole.omega.tmp;
		}

		/// New value
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

	os& operator<< (os& _os, const Cart& _cart)
	{
		_os << "\n\nCart:"
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
			_os << pole;
		}
		return _os;
	}
}
