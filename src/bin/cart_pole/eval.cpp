#include "eval.hpp"

namespace CartPole
{
	real Pole::Def::theta = 6.0;
	real Pole::Def::omega = 0.0;
	real Pole::Def::mass = 0.1;
	real Pole::Def::len = 1.0;
	real Pole::Def::alpha = 0.0;
	real Pole::Def::hfc = 0.000002;

	real Cart::Def::pos = 0.0;
	real Cart::Def::vel = 0.0;
	real Cart::Def::mass = 1.0;
	real Cart::Def::acc = 0.0;
	real Cart::Def::k_l = 0.0;
	real Cart::Def::k_r = 0.0;
	real Cart::Def::tfc = 0.0005;

	real Track::Def::track_len = 4.8;
	bool Track::Def::with_vel = true;
	bool Track::Def::coupled = false;
	std::vector<real> Track::Def::ks = {};

	void to_csv(const std::vector<std::string>& _histograms)
	{
		ulock lk(dump_mtx);
		for (uint h = 0; h < _histograms.size(); ++h)
		{
			std::string file_name("env_" + std::to_string(h) + ".dat");
			std::ofstream csv(file_name);
			if (csv.is_open())
			{
				csv << _histograms.at(h);
			}
		}
	}

	void eval(Net& _net)
	{
		std::vector<std::string> histograms;
		uint steps(0);

		// Initialise with random cart position and pole angles
		Track env(def_env());
		env.init();

		// Evaluate the network and set the fitness
		steps = eval_env(_net, env, histograms);

		if (steps >= Max::steps)
		{
			dlog() << "Network " << _net.id << " passed the initial test!\n"
				   << "Testing generalisation...";

			uint gen_count(1);
			uint gen_steps(0);
			bool champ(true);

			for (const Track& gen_env : Gen::tracks)
			{
				gen_steps = eval_env(_net, gen_env, histograms, true);
				steps += gen_steps;
				if (gen_steps < Max::steps)
				{
					champ = false;
					break;
				}
				else
				{
					dlog() << "\tNetwork " << _net.id << " passed generalisation test " << gen_count;
				}
				++gen_count;
			}

			if (champ)
			{
				dlog() << "Network " << _net.id << " has passed all generalisation tests!\n"
					   << "Saving histograms...";
				to_csv(histograms);
				dlog() << "done!";
			}
		}

		_net.set_abs_fitness(steps);
	}

	uint eval_env(Net& _net, const Track& _env, std::vector<std::string>& _histograms, bool _add_hist)
	{
		uint steps(0);
		Track env(_env);
		std::stringstream hist;
		std::vector<real> actions(_net.get_output().size());

		// Evaluate the network
		while (steps <= Max::steps &&
			   env.in_range())
		{
			_net.eval(env.norm_state());
			actions = _net.get_output();
			env.update(actions);
			if (_add_hist)
			{
				hist << steps << "," << env.as_csv(env.state());
				for (const auto& act : actions)
				{
					hist << "," << act;
				}
				hist << "\n";
			}
			++steps;
		}

		if (_add_hist)
		{
			_histograms.push_back(hist.str());
		}

		return steps;
	}

	bool load_gen(const json& _gen, std::vector<real>& _vec)
	{
		// Check if the JSON actually contains something
		if (_gen.size() > 0)
		{
			// Load the settings
			real start(0.0);
			real step(0.0);
			uint count(0);
			bool pos(true);
			bool neg(true);

			load(_gen, "start", start);
			load(_gen, "step", step);
			load(_gen, "count", count);
			load(_gen, "pos", pos);
			load(_gen, "neg", neg);

			// Sanity check
			if (count == 0 ||
				step == 0.0 ||
				!(pos || neg))
			{
				return false;
			}

			// Clear the container
			_vec.clear();

			real val(0.0);
			for (uint c = 0; c <= count; ++c)
			{
				val = start + c * step;
				if (pos)
				{
					_vec.emplace_back(val);
				}
				if (neg)
				{
					_vec.emplace_back(-val);
				}
			}

			return true;
		}
		return false;
	}

	std::vector<Cart>& def_carts()
	{
		static std::vector<Cart> carts;
		return carts;
	}

	bool setup(Config& _config)
	{
		Rnd::rng.seed(std::chrono::high_resolution_clock().now().time_since_epoch().count());

		std::stringstream problems;

		_config.load("custom.dt", RK4::dt);
		RK4::dt2 = RK4::dt / 2.0;
		RK4::dt6 = RK4::dt / 6.0;

		_config.load("custom.max.force", Max::force);
		_config.load("custom.max.theta", Max::theta);
		_config.load("custom.max.steps", Max::steps);
		if (Max::force <= 0.0)
		{
			problems << "\t- Invalid max force (" << Max::force << ").\n";
		}
		if (Max::theta <= 0.0)
		{
			problems << "\t- Invalid max angle (" << Max::theta << ").\n";
		}
		if (Max::steps <= 0.0)
		{
			problems << "\t- Invalid max steps (" << Max::steps << ").\n";
		}

		// Default environment parameters
		_config.load("custom.def.env.g", g);
		if (g >= 0.0)
		{
			problems << "\t- Invalid gravitational acceleration (" << g << ").\n";
		}

		_config.load("custom.def.env.track_len", Track::Def::track_len);
		if (Track::Def::track_len <= 0.0)
		{
			problems << "\t- Invalid track length (" << Track::Def::track_len << ").\n";
		}
		Max::pos = Track::Def::track_len / 2.0;

		_config.load("custom.def.env.with_vel", Track::Def::with_vel);
		_config.load("custom.def.env.coupled", Track::Def::coupled);
		if (Track::Def::coupled)
		{
			_config.load("custom.def.env.ks", Track::Def::ks);
			for (uint i = 0; i < Track::Def::ks.size(); ++i)
			{
				if (Track::Def::ks.at(i) <= 0.0)
				{
					problems << "\t- Invalid coefficient for spring " << (i + 1)
							 << " (" << Track::Def::ks.at(i) << ").\n";
				}
			}
		}

		// Default cart parameters
		_config.load("custom.def.cart.pos", Cart::Def::pos);
		_config.load("custom.def.cart.vel", Cart::Def::vel);
		_config.load("custom.def.cart.mass", Cart::Def::mass);
		if (Cart::Def::mass <= 0.0)
		{
			problems << "\t- Invalid default cart mass (" << Cart::Def::mass << ").\n";
		}

		_config.load("custom.def.cart.acc", Cart::Def::acc);
		_config.load("custom.def.cart.k_l", Cart::Def::k_l);
		_config.load("custom.def.cart.k_r", Cart::Def::k_r);
		_config.load("custom.def.cart.tfc", Cart::Def::tfc);
		if (Cart::Def::tfc <= 0.0)
		{
			problems << "\t- Invalid default track friction coefficient (" << Cart::Def::tfc << ").\n";
		}

		// Default pole parameters
		_config.load("custom.def.pole.theta", Pole::Def::theta);
		_config.load("custom.def.pole.omega", Pole::Def::omega);
		_config.load("custom.def.pole.mass", Pole::Def::mass);
		if (Pole::Def::mass <= 0.0)
		{
			problems << "\t- Invalid default pole mass (" << Pole::Def::mass << ").\n";
		}

		_config.load("custom.def.pole.len", Pole::Def::len);
		if (Pole::Def::len <= 0.0)
		{
			problems << "\t- Invalid default pole length (" << Pole::Def::len << ").\n";
		}

		_config.load("custom.def.pole.alpha", Pole::Def::alpha);
		_config.load("custom.def.pole.hfc", Pole::Def::hfc);
		if (Pole::Def::hfc <= 0.0)
		{
			problems << "\t- Invalid default hinge friction coefficient (" << Pole::Def::hfc << ").\n";
		}

		// Load the defaults into static vars
		std::vector<Cart>& carts(def_carts());

		// Check that we have at least one cart...
		json json_carts(_config["custom.env.carts"]);
		if (json_carts.size() == 0)
		{
			problems << "\t- No carts defined.\n";
		}

		// ...with at least one pole *each*
		uint cart_count(1);
		for (const auto& json_cart : json_carts)
		{
			json json_poles(json_cart["poles"]);
			if (json_poles.size() == 0)
			{
				problems << "\t- No poles defined for cart " << cart_count << ".\n";
			}
			++cart_count;

			carts.emplace_back(cart_from_json(json_cart));
		}

		if (problems.str().size() > 0)
		{
			dlog() << "Setup failed due to the following problems:\n"
				   << problems.str();
			return false;
		}

		// Generalisation settings
		if (load_gen(_config["custom.gen.theta"], Gen::theta))
		{
			Gen::vars.push_back(Gen::Var::theta);
		}
		if (load_gen(_config["custom.gen.pos"], Gen::pos))
		{
			Gen::vars.push_back(Gen::Var::pos);
		}
		if (load_gen(_config["custom.gen.omega"], Gen::omega))
		{
			Gen::vars.push_back(Gen::Var::omega);
		}
		if (load_gen(_config["custom.gen.vel"], Gen::vel))
		{
			Gen::vars.push_back(Gen::Var::vel);
		}

		if (!Gen::vars.empty())
		{
			for (const Gen::Var v : Gen::vars)
			{
				switch (v)
				{
				case Gen::Var::theta: populate_gen(Gen::theta, Gen::Var::theta); break;
				case Gen::Var::pos: populate_gen(Gen::pos, Gen::Var::pos); break;
				case Gen::Var::omega: populate_gen(Gen::omega, Gen::Var::omega); break;
				case Gen::Var::vel: populate_gen(Gen::vel, Gen::Var::vel); break;
				}
			}
		}

		// Compute target fitness
//		_config.fit.tgt = Max::steps * (1 + Gen::envs.size());
		_config.fit.tgt = Max::steps * (Gen::tracks.size());

		dlog() << "Generalisation environments: " << Gen::tracks.size()
			   << "\nTarget fitness: " << _config.fit.tgt;

		return _config.validate();
	}


	Cart cart_from_json(const json& _j)
	{
		real pos(Cart::Def::pos);
		real max_pos(Max::pos);
		real vel(Cart::Def::vel);
		real mass(Cart::Def::mass);
		real acc(Cart::Def::acc);
		real k_l(Cart::Def::k_l);
		real k_r(Cart::Def::k_r);
		real tfc(Cart::Def::tfc);

		load(_j, "pos", pos);
		load(_j, "max_pos", max_pos);
		load(_j, "vel", vel);
		load(_j, "mass", mass);
		load(_j, "acc", acc);
		load(_j, "k_l", k_l);
		load(_j, "k_r", k_r);
		load(_j, "tfc", tfc);

		Cart cart(pos, max_pos, vel, mass, acc, k_l, k_r, tfc);

		json json_poles(_j["poles"]);
		for (const json& json_pole : json_poles)
		{
			cart.add_pole(pole_from_json(json_pole));
		}

		return cart;
	}

	Pole pole_from_json(const json& _j)
	{
		real theta(Pole::Def::theta);
		real max_theta(Max::theta);
		real omega(Pole::Def::omega);
		real mass(Pole::Def::mass);
		real len(Pole::Def::len);
		real alpha(Pole::Def::alpha);
		real hfc(Pole::Def::hfc);

		load(_j, "theta", theta);
		load(_j, "max_theta", max_theta);
		load(_j, "omega", omega);
		load(_j, "mass", mass);
		load(_j, "len", len);
		load(_j, "alpha", alpha);
		load(_j, "hfc", hfc);

		return Pole(theta, max_theta, omega, mass, len, alpha, hfc);
	}

	void test()
	{

		dlog d;

		Track env(def_env());

		d << "\nRandom env:\n" << env;

		std::ofstream csv("pos_theta_test.dat");

		std::vector<real> actions(env.cart_count(), 0.0);

//		if (csv.is_open())
//		{
//			for (uint step = 0; step < 10000; ++step)
//			{
//				env.update(actions);
//				csv << step << "," << env.as_csv(env.state()) << "\n";
//			}
//		}

		d << "\nGeneralisation envs:\n";
		for (const Track& env : Gen::tracks)
		{
			d << env;
		}
	}

	Track def_env()
	{
		// Create a new environment
		Track env;

		// Add the preloaded carts
		for (const Cart& cart : def_carts())
		{
			env.add_cart(cart);
		}

		env.init();

		return env;
	}

	void populate_gen(const std::vector<real>& _values, const Gen::Var _var)
	{
		if (Gen::tracks.empty())
		{
			Track env;
			for (Cart& cart : def_carts())
			{
				env.add_cart(cart);
			}
			Gen::tracks.emplace_back(env);
		}

		std::vector<Track> tmp_envs;

		for (Track& gen_env : Gen::tracks)
		{
			for (const real value : _values)
			{
				switch (_var)
				{
				case Gen::Var::pos:
					for (uint i = 0; i < gen_env.get_carts().size(); ++i)
					{
						Track env(gen_env);
						env.get_carts().at(i).set(Cart::Var::pos, value);
						tmp_envs.push_back(env);
					}
					break;
				case Gen::Var::vel:
					for (uint i = 0; i < gen_env.get_carts().size(); ++i)
					{
						Track env(gen_env);
						env.get_carts().at(i).set(Cart::Var::vel, value);
						tmp_envs.push_back(env);
					}
					break;
				case Gen::Var::omega:
					for (uint i = 0; i < gen_env.get_carts().size(); ++i)
					{
						for (uint j = 0; j < gen_env.get_carts().at(i).get_poles().size(); ++j)
						{
							Track env(gen_env);
							env.get_carts().at(i).get_poles().at(j).set(Pole::Var::omega, value);
							tmp_envs.push_back(env);
						}
					}
					break;
				case Gen::Var::theta:
					for (uint i = 0; i < gen_env.get_carts().size(); ++i)
					{
						for (uint j = 0; j < gen_env.get_carts().at(i).get_poles().size(); ++j)
						{
							Track env(gen_env);
							env.get_carts().at(i).get_poles().at(j).set(Pole::Var::theta, value);
							tmp_envs.push_back(env);
						}
					}
					break;
				}
			}
		}

		Gen::tracks = tmp_envs;
	}
}
