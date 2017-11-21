#include "Config.hpp"

namespace Cortex
{
	template<typename T>
	void from_json(const json& _j, std::vector<T>& _v)
	{
		_v = _j.get<std::vector<T>>();
	}

	template<typename E, typename V, typename ... Rest, typename std::enable_if< std::is_enum<E>::value, E>::type ...>
	void from_json(const json& _j, emap<E, V>& _map)
	{
		if (!_j.is_object())
		{
			dlog() << "JSON entity is not an object: " << _j.dump(4);
			exit(EXIT_FAILURE);
		}
		for (const auto& entry : _j.get<hmap<std::string, json>>() )
		{
			E e(as_enum<E>(entry.first));
			if (e == Enum<E>::undef)
			{
				dlog() << "Invalid enum entry '" << entry.first << "'";
				exit(EXIT_FAILURE);
			}
			_map[e] = entry.second.get<V>();
		}
	}

	template<typename E, typename std::enable_if< std::is_enum<E>::value, E>::type ...>
	void from_json( const json& _j, E _enum )
	{
		_enum = as_enum<E>(_j.get<std::string>());
		if (_enum == Enum<E>::undef)
		{
			dlog() << "Invalid enum entry '" << _enum << "'";
			exit(EXIT_FAILURE);
		}
	}

	template<typename Val, typename std::enable_if < !std::is_enum<Val>::value, Val >::type ...>
	void from_json(const json& _j, Val& _val)
	{
		_val = _j.get<Val>();
	}

	void Config::load()
	{
		try
		{
			parse_json();
		}
		catch (json::exception& e)
		{
			dlog() << "*** Error: Invalid JSON.\n"
				   << e.what();
			exit(EXIT_FAILURE);

		}

		/// Ecosystem
		load("ecosystem.search", ecosystem.search);
		load("ecosystem.init.size", ecosystem.init.size);
		load("ecosystem.max.size", ecosystem.max.size);
		load("ecosystem.max.age", ecosystem.max.age);

		/// Species
		load("species.init.count", species.init.count);
		load("species.max.count", species.max.count);

		/// Nets
		load("net.type", net.type);
		load("net.rf", net.rf);
		load("net.spiking.enc", net.spiking.enc);
		load("net.spiking.beta", net.spiking.beta);
		load("net.spiking.mod", net.spiking.mod);
		load("net.spiking.timestep", net.spiking.timestep);
		load("net.spiking.max.latency", net.spiking.max.latency);
		load("net.max.age", net.max.age);

		/// Nodes
		load("node.init.fn", node.init.fn);
		load("node.roles", node.roles);
		load("node.lif", node.lif);
		load("node.tau.val", node.tau.val);
		load("node.tau.mean", node.tau.mean);
		load("node.tau.max", node.tau.max);
		load("node.tau.sd", node.tau.sd);

		/// Links
		load("link.weight.val", link.weight.val);
		load("link.weight.mean", link.weight.mean);
		load("link.weight.max", link.weight.max);
		load("link.weight.sd", link.weight.sd);
		load("link.rec", link.rec);
		load("link.type", link.type);
		load("link.prob", link.prob);

		/// Mating
		load("mating.enabled", mating.enabled);
		load("mating.rate", mating.rate);

		/// Mutation
		load("mutation.enabled", mutation.enabled);
		load("mutation.adaptive", mutation.adaptive);
		load("mutation.elitism.enabled", mutation.elitism.enabled);
		load("mutation.elitism.prop", mutation.elitism.prop);
		load("mutation.attempts", mutation.attempts);
		load("mutation.opt", mutation.opt);
		load("mutation.scale", mutation.scale);
		load("mutation.prob", mutation.prob);
		load("mutation.rate", mutation.rate);
		load("mutation.node", mutation.node);
		load("mutation.fn", mutation.fn);

		/// Fitness
		load("fit.tgt", fit.tgt);
		load("fit.ema.coeff", fit.ema.coeff);

		/// STDP
		load("stdp.enabled", stdp.enabled);
		load("stdp.type", stdp.type);
		load("stdp.lr", stdp.lr);
		load("stdp.alpha", stdp.alpha);
		load("stdp.span", stdp.span);

		/// Novelty
		load("novelty.enabled", novelty.enabled);
		load("novelty.hist_size", novelty.hist_size);

		/// Others
		load("runs", runs);
		load("threads", threads);

		dlog() << "Configuration loaded successfully!\n" << config_json.dump(4);
	}

	bool Config::parse_json()
	{
		/// Open, parse and load the JSON file
		std::ifstream is( config_file );
		std::stringstream ss_json;
		ss_json << is.rdbuf();
		std::string content(ss_json.str());

		if ( content.size() > 0 )
		{
			std::transform( content.begin(), content.end(), content.begin(), ::tolower );
		}
		else
		{
			dlog() << "Empty configuration file!\n";
			return false;
		}

		config_json = json::parse(content);

		return true;
	}

	std::deque<std::string> get_keys(const std::string& _keys)
	{
		std::deque<std::string> keys;
		std::stringstream ss(_keys);
		std::string key;

		while ( std::getline( ss, key, '.' ) )
		{
			keys.emplace_back( key );
		}
		return keys;
	}

	Config::Config(const std::string& _config_file)
		:
		  config_file(_config_file),
		  species_id(0),
		  net_id(0)
	{
		dlog() << "\n##### Cortex neuroevolution platform v. " << version << " #####\n";

		rng.engine.seed( static_cast<ulong>( std::chrono::high_resolution_clock::now().time_since_epoch().count() ) );

		ecosystem.search = Search::Fitness;
		ecosystem.init.size = 50;
		ecosystem.max.size = 200;
		ecosystem.max.age = 1000;

		species.init.count = 3;
		species.max.count = 15;

		net.type = NT::Classical;
		net.rf = RF::Undef;
		net.spiking.enc = Enc::Rank;
		net.spiking.beta = 1.5;
		net.spiking.mod = 0.9;
		net.spiking.timestep = 10.0;
		net.spiking.max.latency = 70.0;
		net.max.age = 0;

		node.init.fn[NR::B] = Fn::Const;
		node.init.fn[NR::I] = Fn::Sum;
		node.init.fn[NR::O] = Fn::Tanh;
		node.init.fn[NR::H] = Fn::Tanh;

		node.roles[NR::B] = 0;
		node.roles[NR::I] = 0;
		node.roles[NR::O] = 0;
		node.roles[NR::H] = 0;

		node.lif = true;

		/// Initial value | mean | max | SD
		node.tau = {0.0, 10.0, 20.0, 0.1};

		/// Initial value | mean | max | SD
		link.weight = {0.0, 0.0, 5.0, 1.0};
		link.rec = true;

		link.type[LT::F] = 0.8;
		link.type[LT::R] = 0.2;

		/// Forward link probabilities
		link.prob[LT::F][NR::B][NR::H] = 1.0;
		link.prob[LT::F][NR::B][NR::O] = 1.0;
		link.prob[LT::F][NR::I][NR::H] = 100.0;
		link.prob[LT::F][NR::I][NR::O] = 5.0;
		link.prob[LT::F][NR::H][NR::H] = 50.0;
		link.prob[LT::F][NR::H][NR::O] = 100.0;

		/// Recurrent link probabilities
		link.prob[LT::R][NR::I][NR::I] = 1.0;
		link.prob[LT::R][NR::H][NR::I] = 10.0;
		link.prob[LT::R][NR::H][NR::H] = 10.0;
		link.prob[LT::R][NR::O][NR::I] = 50.0;
		link.prob[LT::R][NR::O][NR::H] = 100.0;
		link.prob[LT::R][NR::O][NR::O] = 10.0;

		mating.enabled = true;
		mating.rate = 0.1;

		mutation.enabled = true;
		mutation.adaptive = false;
		mutation.rate = 10;
		mutation.attempts = 5;
		mutation.opt = Opt::Trend;
		mutation.scale = 0.05;

		mutation.prob[Mut::AddNode] = 5.0;
		mutation.prob[Mut::EraseNode] = 5.0;
		mutation.prob[Mut::AddLink] = 30.0;
		mutation.prob[Mut::EraseLink] = 30.0;
		mutation.prob[Mut::Tau] = 10.0;
		mutation.prob[Mut::Fn] = 1.0;
		mutation.prob[Mut::Weight] = 1000.0;

		mutation.node[NR::B] = 1.0;
		mutation.node[NR::I] = 1.0;
		mutation.node[NR::O] = 5.0;
		mutation.node[NR::H] = 10.0;

		/// Default transfer functions
		mutation.fn[NR::B][node.init.fn.at(NR::B)] = 1.0;
		mutation.fn[NR::I][node.init.fn.at(NR::I)] = 1.0;
		mutation.fn[NR::O][node.init.fn.at(NR::O)] = 1.0;
		mutation.fn[NR::H][node.init.fn.at(NR::H)] = 1.0;

		mutation.elitism.enabled = true;
		mutation.elitism.prop = 0.05;

		fit.tgt = 0.0;
		fit.ema.coeff = 0.25;

		stdp.enabled = false;
		stdp.type = STDP::Heb;
		stdp.lr = 0.01;
		stdp.alpha = 1.05;
		stdp.span = 10.0;

		novelty.enabled = false;
		novelty.hist_size = 100;

		runs = 1;
		threads = std::thread::hardware_concurrency();

		load();
	}

	bool Config::validate()
	{
		/// Make sure that there are input and output nodes
		std::stringstream problems;

		if (ecosystem.init.size == 0)
		{
			problems << "\t - Initial ecosystem size set to 0.\n";
		}

		if (species.init.count == 0)
		{
			problems << "\t - Initial species count set to 0.\n";
		}

		if (ecosystem.init.size < species.init.count )
		{
			problems << "\t - Number of species > number of networks.\n";
		}

		if (ecosystem.max.age == 0 )
		{
			problems << "\t - Max ecosystem age set to 0.\n";
		}

		if (node.roles.at(NR::I) == 0)
		{
			problems << "\t - Missing input nodes.\n";
		}

		if (node.roles.at(NR::O) == 0)
		{
			problems << "\t - Missing output nodes.\n";
		}

		/// Make sure that the network type is consistent
		/// with the activation function
		if (net.type == NT::Undef)
		{
			problems << "\t - Missing network type.\n";
		}
		else if (net.type == NT::Spiking)
		{
			/// Disable transfer function mutation.
			mutation.prob.erase(Mut::Fn);
		}
		else
		{
			/// Disable membrane time constant mutation.
			mutation.prob.erase(Mut::Tau);
		}

		if (ecosystem.search == Search::Fitness &&
			fit.tgt <= 0.0 )
		{
			problems << "\t - Fitness <= 0.\n";
		}

		if (fit.ema.coeff > 0.5 ||
			fit.ema.coeff <= 0.0)
		{
			problems << "\t - EMA coefficient must be between 0 and 0.5.\n";
		}

		if (!link.rec)
		{
			link.type.at(LT::F) = 1.0;
			link.type.at(LT::R) = 0.0;
		}

		if (mutation.enabled)
		{
			if (mutation.attempts == 0)
			{
				problems << "\t - Mutation attempts set to 0.\n";
			}

			if (mutation.elitism.prop <= 0.0)
			{
				problems << "\t - Elite proportion set to 0.\n";
			}

			if (mutation.rate == 0)
			{
				problems << "\t - Mutation rate set to 0.\n";
			}
		}
		else
		{
			mutation.rate = 1;
		}

		if (mating.enabled)
		{
			if (mating.rate == 0.0)
			{
				problems << "\t - Mating rate set to 0.\n";
			}
		}

		if (runs == 0)
		{
			problems << "\t - Number of runs set to 0.\n";
		}

		if (problems.str().size() > 0)
		{
			dlog() << "Invalid configuration:\n"
				   << problems.str();

			return false;
		}

		return true;
	}

	bool traverse(json& _j, json::iterator& _it, const std::string& _param)
	{
		std::deque<std::string> keys(get_keys(_param));
		std::string key(keys.front());
		keys.pop_front();

		while (true)
		{
			if ((_it = _j.find(key)) == _j.end())
			{
				break;
			}
			if (keys.empty())
			{
				/// Key found
				return true;
			}
			else if (_it->is_object())
			{
				key = keys.front();
				keys.pop_front();
				_j = _it.value();
				_it = _j.begin();
			}
		}
		return false;
	}
}
