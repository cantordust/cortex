#include "Conf.hpp"
#include "Node.hpp"
#include "Data.hpp"

namespace Cortex
{
	Conf::Conf(const std::string& _file)
		:
		  config_file(_file),
		  eco(*this),
		  spc(*this),
		  net(*this),
		  node(*this),
		  link(*this),
		  mating(*this),
		  mut(*this),
		  fit(*this),
		  stdp(*this),
		  novelty(*this),
		  dataset(*this),
		  sys(*this),
		  data(std::make_shared<Data>(*this))
	{
		dlog() << "\n===== Cortex neuroevolution platform v. " << version << " =====";

		rnd.engine.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

		/// @todo Doxygen documentation

		try
		{
			parse_json(config_file, json_root);
		}
		catch (json::exception& e)
		{
			dlog() << "### Error parsing JSON.\n" << e.what();
			exit(EXIT_FAILURE);
		}

		load();

		validate();

		data->set_defaults();

		data->validate();

		dlog() << "*** Configuration loaded successfully.";
	}

	void Conf::load()
	{
		load("eco", eco);
		load("spc", spc);
		load("net", net);
		load("node", node);
		load("link", link);
		load("mating", mating);
		load("mut", mut);
		load("fit", fit);
		load("stdp", stdp);
		load("novelty", novelty);
		load("dataset", dataset);
		load("sys", sys);
	}

	void Conf::validate()
	{
		std::stringstream problems;

		problems << eco.validate()
				 << spc.validate()
				 << net.validate()
				 << node.validate()
				 << link.validate()
				 << mating.validate()
				 << mut.validate()
				 << fit.validate()
				 << stdp.validate()
				 << novelty.validate()
				 << dataset.validate()
				 << sys.validate()
				 << data->validate();

		if (problems.str().size() > 0)
		{
			dlog() << "### Configuration errors:\n"
				   << problems.str();
			exit(EXIT_FAILURE);
		}
	}

	json Conf::operator[](const std::string& _key_seq)
	{
		json j(json_root);
		auto it(j.begin());
		if (traverse(j, it, _key_seq))
		{
			return *it;
		}
		return json();
	}

	const json& Conf::get_json() const
	{
		return json_root;
	}

	void Conf::reset_ids()
	{
		spc.reset_id();
		net.reset_id();
	}

	bool Conf::rnd_chance(const real _prob)
	{
		return rnd_real(0.0, 1.0) <= _prob;
	}

	real Conf::rnd_nd(const real _mean, const real _sd)
	{
		glock lk(rnd.semaphore);
		std::normal_distribution<real> dist(_mean, _sd);
		return dist(rnd.engine);
	}

	size_t Conf::w_dist(const std::vector<real>& _weights)
	{
		glock lk(rnd.semaphore);
		std::discrete_distribution<size_t> dist(_weights.begin(), _weights.end());
		return dist(rnd.engine);
	}

	std::ostream& operator << (std::ostream& _strm, const Conf& _conf)
	{
		return _strm << _conf.eco
					 << _conf.spc
					 << _conf.net
					 << _conf.node
					 << _conf.link
					 << _conf.mating
					 <<	_conf.mut
					 << _conf.fit
					 << _conf.stdp
					 << _conf.novelty
					 << _conf.dataset
					 << _conf.sys;
	}
}
