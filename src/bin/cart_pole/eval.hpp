#ifndef EVAL_HPP
#define EVAL_HPP

#include "Cortex.hpp"
#include "physics.hpp"

namespace CartPole
{
	static std::mutex dump_mtx;

	bool setup(Config& _config);

	void eval(Net& _net);

	uint eval_env(Net& _net, const Env& _env, std::vector<std::string>& _histograms, bool _add_hist = false);

	void to_csv(const std::vector<std::string>& _histograms);

	void populate_gen(const std::vector<real>& _values, const Gen::Var _var);

	bool load_gen(const json& _gen, std::vector<real>& _vec);

	std::vector<Cart>& def_carts();

	Cart cart_from_json(const json& _j);

	Pole pole_from_json(const json& _j);

	void test();

	/// Random track based on the default track
	Env def_env();
}

#endif // EVAL_HPP
