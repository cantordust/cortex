#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <random>
#include <chrono>

#include "Stat.hpp"
#include "json.hpp"

namespace Cortex
{
	using json = nlohmann::json;

	std::deque<std::string> get_keys(const std::string& _key_seq);

	bool traverse(json& _j, json::iterator& _it, const std::string& _key_seq);

	template<typename T>
	void load(const json& _j, const std::string& _param, T& _var, const T& _def = T())
	{
		json j(_j);
		auto it(j.begin());

		if (traverse(j, it, _param))
		{
			_var = it->get<T>();
		}
		else
		{
			_var = _def;
		}
	}

	template<template<typename ... Args> class Cont, typename ... Ts>
	void load(const json& _j, const std::string& _param, Cont<Ts...>& _cont)
	{
		json j(_j);
		auto it(j.begin());

		if (traverse(j, it, _param))
		{
			_cont = it->get<Cont<Ts...>>();
		}
	}

	struct ParamConf
	{
		real init = 0.0;
		real min = std::numeric_limits<real>::min();
		real max = std::numeric_limits<real>::max();
		real mean = 0.0;
		real sd = 1.0;
		real alpha = 0.25;
		bool fixed = false;
		InitDist dist = InitDist::Normal;
	};

	class Config
	{

	public:

		struct
		{
			// Search mode to use (fitness, novelty, etc.)
			Search search;

			struct
			{
				// Initial number of networks.
				uint size;
			} init;

			struct
			{
				// Maximal number of individuals in the ecosystem
				uint size;

				// Maximal number of generations
				uint age;
			} max;
		} ecosystem;

		struct
		{
			struct
			{
				// The initial number of species.
				uint count;
			} init;

			struct
			{
				// The maximal number of species.
				uint count;
			} max;

		} species;

		struct
		{
			// Network type
			NT type;

			// Network topology
//			Topology topology;

			// Input substrate
//			Substrate sub_in;

			// Output substrate
//			Substrate sub_out;

			struct
			{
				// The maximal number of generations that a
				// network is allowed to exist.
				// Set to 0 to disable.
				uint age;
			} max;

			struct
			{
				// The type of spike encoding determines
				// the efficiency of information processing.
				Enc enc;

				// Beta parameter for Gaussian receptive fields
				real beta;

				// Number of receptors per input node
				uint receptors;

				// Coefficient for rank order encoding
				real mod;

				// Maximal spiking latency
				struct
				{
					real latency;
				} max;
			} spiking;

		} net;

		struct
		{

			// Receptive field type
			RF rf;

			struct
			{
				// Default transfer function by node role
				emap<NR, Fn> fn;
			} init;

			// Node roles and corresponding counts
			emap<NR, uint> roles;

			// The activation level of LIF spiking nodes
			// decays over time, whereas non-LIF nodes
			// maintain their activation indefinitely.
			bool lif;

			// Parameter configuration for
			// membrane time constant
			ParamConf tau;

		} node;

		struct
		{
			// Parameter configuration for
			// link weights
			ParamConf weight;

			// Allow / disallow recurrent links
			bool rec;

			// Probabilities for forward and recurrent types
			emap<LT, real> type;

			emap<LT, emap<NR, emap<NR, real>>> prob;
		} link;

		struct
		{
			// Allow or disallow mating
			bool enabled;

			// Rate of producing offspring as a portion
			// of the current ecosystem size.
			real rate;

		} mating;

		struct
		{
			// Toggle mutation
			bool enabled;

			// Enables scaling the probability
			// of adding / deleting links and nodes
			// depending on the connectivity of the network
			bool adaptive;

			// Mutations per generation
			uint rate;

			// The number of times that the algorithm
			// should retry a mutation if it fails
			uint attempts;

			// Optimisation method
			Opt opt;

			// Scaling factor for mutations.
			// Defalut: 0.05 (5%)
			real scale;

			// Mutation probability weights for the network
			emap<Mut, real> prob;

			// Mutation probability by node role
			emap<NR, real> node;

			// Allowed transfer functions by node role
			emap<NR, emap<Fn, real>> fn;

			struct
			{
				// Toggle elitism
				bool enabled;

				// Proportion of the population to consider as elite
				real prop;

			} elitism;

		} mutation;

		struct
		{
			// The target fitness that all
			// networks are striving towards.
			real tgt;

			struct
			{
				// Coefficient for tracking the exponential
				// moving average of the fitness.
				// 0 < alpha < 0.5
				real coeff;
			} ema;

		} fit;

		struct
		{
			// Toggle STDP on/off
			bool enabled;

			// Type of Hebbian learning
			STDP type;

			// Learning rate
			real lr;

			// Ratio of depression to potentiation strength
			real alpha;

			// Time span for STDP
			struct
			{
				// Span for pre-spike STDP
				real pre;

				// Span for post-spike STDP
				real post;
			} span;
		} stdp;

		// This is not implemented yet!
		struct
		{
			// Enable novelty search.
			bool enabled;

			// Number of previous states to keep in the history
			uint hist_size;

		} novelty;

		// Number of times to run the experiment
		uint runs;

		// Number of threads in the threadpool
		uint threads;

	private:

		std::string config_file;

		json config_json;

		uint species_id;
		uint net_id;

		struct
		{
			// \brief 64-bit Mersenne twister RNG.
			std::mt19937_64 engine;

			// RNG mutex
			std::mutex mtx;
		} rng;

	public:

		Config(const std::string& _config_file);

		bool validate();

		json operator[] (const std::string& _param);

		json get_json() const;

		uint new_spc_id();

		uint new_net_id();

		void reset_ids();

		// Functions returning random numbers, elements, etc.

		// Random outcome with given probability
		bool rnd_chance(const real _prob);

		// Random number drawn from a normal distribution
		real rnd_nd(const real _mean, const real _sd);

		size_t w_dist(const std::vector<real>& _weights);

		/**
		 * @brief load Load a value into a variable from a JSON configuraiton.
		 * @param _key_seq  Key sequence in the form of "key1.key2.key3...".
		 * @param _var Variable to be populated.
		 * @param _def Default value (optional).
		 */
		template<typename T>
		inline void load( const std::string& _key_seq, T& _var, const T& _def = T())
		{
			Cortex::load(config_json, _key_seq, _var);
		}

		/**
		 * @brief load Load an associative container from a JSON configuration.
		 * @param _key_seq Key sequence in the form of "key1.key2.key3...".
		 * @param _cont Associative container.
		 */
		template<template<typename ... Args> class Cont, typename ... Ts>
		inline void load(const std::string& _key_seq, Cont<Ts...>& _cont)
		{
			Cortex::load(config_json, _key_seq, _cont);
		}

		/**
		 * @brief rnd_int Returns a random integer value between \p _min and \p _max.
		 * @param _min Minimal value.
		 * @param _max Maximal value
		 * @return Integer value.
		 */
		template<typename T = uint, typename std::enable_if<std::is_integral<T>::value>::type ...>
		inline T rnd_int(const T _min, const T _max)
		{
			glock lk(rng.mtx);
			std::uniform_int_distribution<T> dist(_min, _max);
			return dist(rng.engine);
		}

		/**
		 * @brief rnd_real Returns a random floating-point value between \p _min and \p _max.
		 * @param _min Minimal value.
		 * @param _max Maximal value.
		 * @return Floating-point value.
		 */
		template<typename T = real, typename std::enable_if<std::is_floating_point<T>::value>::type ...>
		inline real rnd_real(const real _min, const real _max)
		{
			glock lk(rng.mtx);
			std::uniform_real_distribution<real> dist(_min, _max);
			return dist(rng.engine);
		}

		/**
		 * @brief rnd_elem Returns a random element from a non-associative
		 * container (such as std::vector).
		 * @param _cont A non-associative container	compatible with std::begin().
		 * @return Reference to a random element.
		 */
		template<template<typename ...> class Cont, typename T>
		inline T rnd_elem(const Cont<T>& _cont)
		{
			auto it( std::begin( _cont ) );
			std::advance(it, rnd_int<size_t>(0, _cont.size() - 1));
			return *it;
		}

		/**
		 * @brief rnd_key Returns a random key from
		 * an associative container (such as std::map).
		 * @param _cont An associative container compatible with std::begin().
		 * @return Reference to a random key.
		 */
		template<template <typename ...> class Cont, typename K, typename V, typename ... Rest>
		inline K rnd_key(const Cont<K, V, Rest ...>& _cont)
		{
			auto it(std::begin(_cont));
			std::advance(it, rnd_int<size_t>(0, _cont.size() - 1));
			return it->first;
		}

		/**
		 * @brief rnd_val Returns a reference to a random value of
		 * an associative container (such as std::map).
		 * @param _cont An associative container compatible with std::begin().
		 * @return Reference to a random value.
		 */
		template<template <typename ...> class Cont, typename K, typename V, typename ... Rest>
		inline V rnd_val(const Cont<K, V, Rest ...>& _cont)
		{
			auto it( std::begin( _cont ) );
			std::advance( it, rnd_int<size_t>(0, _cont.size() - 1));
			return it->second;
		}

		/**
		 * @brief w_dist Returns a random key from an associative container
		 * containing elements as keys and their respective weights as values.
		 * The weights must be floating-point values (float or double).
		 * @param _cont Associative container with weights as values.
		 * @return Random key drawn from a weighted distribution.
		 */
		template<template <typename ...> class Cont, typename K, typename ... Hash>
		inline K w_dist(const Cont<K, real, Hash...>& _cont)
		{
			std::vector<K> values;
			std::vector<real> weights;
			values.reserve(_cont.size());
			weights.reserve(_cont.size());

			for (auto&& key : _cont)
			{
				values.push_back(key.first);
				weights.push_back(key.second);
			}

			size_t u(w_dist(weights));
			return values[u];
		}

	private:

		void load();

		bool parse_json();

	};
}


#endif // CONFIG_HPP
