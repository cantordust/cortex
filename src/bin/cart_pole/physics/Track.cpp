#include "Track.hpp"

namespace CartPole
{
	void Track::init()
	{
		for (Cart& cart : carts)
		{
			cart.init();
		}
	}

	void Track::add_cart(const Cart& _cart)
	{
		carts.push_back(_cart);
	}

	std::vector<Cart>& Track::get_carts()
	{
		return carts;
	}

	void Track::update(const std::vector<real>& _actions)
	{
		for (uint idx = 0; idx < carts.size(); ++idx)
		{
			carts[idx].update(_actions[idx]);
		}
	}

	std::string Track::as_csv(const std::vector<real>& _v) const
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
			                       [](const std::string& _cur, const real _s)
			{
				return _cur + "," + std::to_string(_s);
			});
		}
	}

	real Track::restrict(const real _theta)
	{
		real theta(fmod(_theta + pi, two_pi));
		if (theta < 0.0)
		{
			theta += two_pi;
		}
		return theta - pi;
	}

	std::vector<real> Track::state()
	{
		std::vector<real> s;
		for (Cart& cart : carts)
		{
			s.push_back(cart.pos.cur);
			if (Track::Def::with_vel)
			{
				s.push_back(cart.vel.cur);
			}
			for (Pole& pole : cart.poles)
			{
				s.push_back(restrict(pole.theta.cur));
				if (Track::Def::with_vel)
				{
					s.push_back(pole.omega.cur);
				}
			}
		}
		return s;
	}

	std::vector<real> Track::norm_state()
	{
		std::vector<real> s;
		for (Cart& cart : carts)
		{
			s.push_back(cart.pos.cur / cart.pos.max);
			if (Track::Def::with_vel)
			{
				s.push_back(cart.vel.cur);
			}
			for (Pole& pole : cart.poles)
			{
				s.push_back(restrict(pole.theta.cur) / pole.theta.max);
				if (Track::Def::with_vel)
				{
					s.push_back(pole.omega.cur);
				}
			}
		}
		return s;
	}

	bool Track::is_in_range()
	{
		for (Cart& cart : carts)
		{
			if (!cart.is_in_range())
			{
				return false;
			}
		}
		return true;
	}

	uint Track::cart_count()
	{
		return carts.size();
	}

	uint Track::pole_count()
	{
		uint poles(0);

		for (Cart& cart : carts)
		{
			poles += cart.pole_count();
		}
		return poles;
	}

	os& operator<< (os& _os, const Track& _env)
	{
		_os.setf(std::ios_base::boolalpha);

		_os << "\n-------------------- Env -------------------------\n";
		_os << "\n\nMax:"
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
			_os << cart;
		}
		_os << "\n--------------------------------------------------\n";
		return _os;
	}
}
