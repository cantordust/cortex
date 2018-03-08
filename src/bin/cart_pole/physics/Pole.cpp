#include "Pole.hpp"

namespace CartPole
{

	Pole::Pole(const real _theta, const real _max_theta, const real _omega, const real _mass, const real _len, const real _alpha, const real _hfc)
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


	real Pole::rnd_theta(const real _min, const real _max, const bool _rnd_sign)
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

	void Pole::init()
	{
		theta.cur = rnd_theta();
	}

	void Pole::set_var(const Pole::Var _var, const real _val)
	{
		switch (_var)
		{
		case Var::theta:
			theta.cur = rad(_val);
			break;

		case Var::max_theta:
			theta.max = rad(_val);
			break;

		case Var::omega:
			omega.cur = _val;
			break;

		case Var::max_omega:
			omega.max = _val;
			break;

		case Var::mass:
			mass = _val;
			break;

		case Var::len:
			half_len = _val / 2.0;
			break;

		case Var::alpha:
			alpha = _val;
			break;

		case Var::hfc:
			hfc = _val;
			break;

		default:
			break;
		}
	}

	bool Pole::is_in_range()
	{
		return std::fabs(theta.cur) < theta.max;
	}

	real Pole::compute_alpha(const real _cart_acc)
	{
		alpha = coeff * (_cart_acc * cos_theta + g * sin_theta + omega.tmp * hml);
		return alpha;
	}

	void Pole::set_tmp(const real _dt)
	{
		omega.tmp = omega.cur + alpha * _dt;
		theta.tmp = theta.cur + omega.tmp * _dt;
		sin_theta = std::sin(theta.tmp);
		cos_theta = std::cos(theta.tmp);
	}

	real Pole::m_eff()
	{
		return mass * (1.0 - 0.75 * std::pow(cos_theta, 2));
	}

	real Pole::f_eff()
	{
		return ml * std::pow(omega.tmp, 2) * sin_theta +
				0.75 * mass * cos_theta * (omega.tmp * hml + g * sin_theta );
	}

	std::ostream& operator<< (std::ostream& _strm, const Pole& _pole)
	{
		return _strm << "\n\n\tPole:"
					 << "\n\t\tmass: " << _pole.mass << " kg"
					 << "\n\t\tlength: " << _pole.half_len * 2.0 << " m"
					 << "\n\t\toffset angle: " << deg(_pole.theta.cur) << " deg (" << _pole.theta.cur << " rad)"
					 << "\n\t\tangular velocity: " << _pole.omega.cur << " rad/s"
					 << "\n\t\tangular acceleration: " << _pole.alpha << " rad/s^2"
					 << "\n\t\thinge friction coeff.: " << _pole.hfc;
	}
}
