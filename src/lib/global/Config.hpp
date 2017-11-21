#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <random>
#include <chrono>

#include "Stat.hpp"
#include "json.hpp"

namespace Cortex
{
	using json = nlohmann::json;

	std::deque<std::string> get_keys(const std::string& _param);

	bool traverse(json& _j, json::iterator& _it, const std::string& _param);

	template<typename T>
	void load(const json& _j, const std::string& _param, T& _var, const T& _def = T())
	{
		json j(_j);
		auto it(j.begin());

		if (traverse(j, it, _param))
		{
			_var = it->get<T>();
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

	class Config
	{

	private:

		std::string config_file;

		json config_json;

		uint species_id;
		uint net_id;

		void load();

		bool parse_json();

		struct
		{
			/// \brief 64-bit Mersenne twister RNG.
			std::mt19937_64 engine;

			/// RNG mutex
			std::mutex mtx;
		} rng;

	public:

		template<typename T>
		inline void load( const std::string& _param, T& _var, const T& _def = T())
		{
			Cortex::load(config_json, _param, _var);
		}

		template<template<typename ... Args> class Cont, typename ... Ts>
		inline void load(const std::string& _param, Cont<Ts...>& _cont)
		{
			Cortex::load(config_json, _param, _cont);
		}

		json operator[] (const std::string& _param)
		{
			json j(config_json);
			auto it(j.begin());
			if (traverse(j, it, _param))
			{
				return *it;
			}
			return json();
		}

		struct ParamCfg
		{
			real val;
			real mean;
			real max;
			real sd;
		};

		struct
		{
			/// Search mode to use (fitness, novelty, etc.)
			Search search;

			struct
			{
				/// Initial number of networks.
				uint size;
			} init;

			struct
			{
				/// Maximal number of individuals in the ecosystem
				uint size;

				/// Maximal number of generations
				uint age;
			} max;
		} ecosystem;

		struct
		{
			struct
			{
				/// The initial number of species.
				uint count;
			} init;

			struct
			{
				/// The maximal number of species.
				uint count;
			} max;

		} species;

		struct
		{
			/// Network type
			NT type;

			/// Receptive field type
			RF rf;

			struct
			{
				/// The type of spike encoding determines
				/// the efficiency of information processing.
				Enc enc;

				/// Beta parameter for Gaussian receptive fields
				real beta;

				/// Parameter for computing node
				/// activation in the case of rank order encoding
				real mod;

				/// The default time step for evaluation.
				real timestep;

				/// Maximal spiking latency
				struct
				{
					real latency;
				} max;
			} spiking;

			struct
			{
				/// The maximal number of generations that a
				/// network is allowed to exist.
				/// Set to 0 to disable.
				uint age;
			} max;
		} net;

		struct
		{
			struct
			{
				/// Default transfer function by node role
				emap<NR, Fn> fn;
			} init;

			/// Node roles and corresponding counts
			emap<NR, uint> roles;

			/// The activation level of LIF spiking nodes
			/// decays over time, whereas non-LIF nodes
			/// maintain their activation indefinitely.
			bool lif;

			/// Parameter configuration for
			/// membrane time constant
			ParamCfg tau;

		} node;

		struct
		{
			/// Parameter configuration for
			/// link weights
			ParamCfg weight;

			/// Allow / disallow recurrent links
			bool rec;

			/// Probabilities for forward and recurrent types
			emap<LT, real> type;

			emap<LT, emap<NR, emap<NR, real>>> prob;
		} link;

		struct
		{
			/// Allow or disallow mating
			bool enabled;

			/// Rate of producing offspring as a portion
			/// of the current ecosystem size.
			real rate;

		} mating;

		struct
		{
			/// Toggle mutation
			bool enabled;

			/// Enables scaling the probability
			/// of adding / deleting links and nodes
			/// depending on the connectivity of the network
			bool adaptive;

			/// Mutations per generation
			uint rate;

			/// The number of times that the algorithm
			/// should retry a mutation if it fails
			uint attempts;

			/// Optimisation method
			Opt opt;

			/// Scaling factor for mutations.
			/// Defalut: 0.05 (5%)
			real scale;

			/// Mutation probability weights for the network
			emap<Mut, real> prob;

			/// Mutation probability by node role
			emap<NR, real> node;

			/// Allowed transfer functions by node role
			emap<NR, emap<Fn, real>> fn;

			struct
			{
				/// Toggle elitism
				bool enabled;

				/// Proportion of the population to consider as elite
				real prop;

			} elitism;

		} mutation;

		struct
		{
			/// The target fitness that all
			/// networks are striving towards.
			real tgt;

			struct
			{
				/// Coefficient for tracking the exponential
				/// moving average of the fitness.
				/// 0 < alpha < 0.5
				real coeff;
			} ema;

		} fit;

		struct
		{
			/// Toggle STDP on/off
			bool enabled;

			/// Type of Hebbian learning
			STDP type;

			/// Learning rate
			real lr;

			/// Ratio of depression to potentiation strength
			real alpha;

			/// Time span to consider for plasticity
			real span;
		} stdp;

		/// This is not implemented yet!
		struct
		{
			/// Enable novelty search.
			bool enabled;

			/// Number of previous states to keep in the history
			uint hist_size;

		} novelty;

		/// Number of experiments
		uint runs;

		/// Number of threads in the threadpool
		uint threads;

		Config(const std::string& _config_file);

		bool validate();

		template<typename T>
		inline T get_custom_val( const std::string& _keys )
		{
			T retval;
			load( _keys, retval );
			return retval;
		}

		inline json get_json() const
		{
			return config_json;
		}

		inline uint new_spc_id()
		{
			return ++species_id;
		}

		inline uint new_net_id()
		{
			return ++net_id;
		}

		inline void reset_ids()
		{
			species_id = 0;
			net_id = 0;
		}

		/// Functions returning random numbers, elements, etc.

		/// Random integer drawn from a uniform distribution
		template<typename T = uint, typename std::enable_if<std::is_integral<T>::value>::type ...>
		inline T rnd_int(const T _min, const T _max)
		{
			glock lk(rng.mtx);
			std::uniform_int_distribution<T> dist(_min, _max);
			return dist(rng.engine);
		}

		/// Random real number drawn from a uniform distribution
		template<typename T = real, typename std::enable_if<std::is_floating_point<T>::value>::type ...>
		inline real rnd_real(const real _min, const real _max)
		{
			glock lk(rng.mtx);
			std::uniform_real_distribution<real> dist(_min, _max);
			return dist(rng.engine);
		}

		/// Chance
		inline bool rnd_chance(const real _prob)
		{
			return rnd_real(0.0, 1.0) <= _prob;
		}

		/// Random number drawn from a normal distribution
		inline real rnd_nd(const real _mean, const real _sd)
		{
			glock lk(rng.mtx);
			std::normal_distribution<real> dist(_mean, _sd);
			return dist(rng.engine);
		}

		inline size_t w_dist(const std::vector<real>& _weights)
		{
			glock lk(rng.mtx);
			std::discrete_distribution<size_t> dist(_weights.begin(), _weights.end());
			return dist(rng.engine);
		}

		/// Generic random functions

		/// Variadic templates (require C++11).
		/// Should work with anything that has a size() method
		/// and is compatible with std::begin().
		template<template<typename ...> class Cont, typename T>
		inline T& rnd_elem(const Cont<T>& _cont)
		{
			auto it( std::begin( _cont ) );
			std::advance(it, rnd_int<size_t>(0, _cont.size() - 1));
			return *it;
		}

		template<template <typename ...> class Cont, typename K, typename V, typename ... Rest>
		inline K rnd_key(const Cont<K, V, Rest ...>& _cont)
		{
			auto it(std::begin(_cont));
			std::advance(it, rnd_int<size_t>(0, _cont.size() - 1));
			return it->first;
		}

		template<template <typename ...> class Cont, typename K, typename V, typename ... Rest>
		inline V& rnd_val(const Cont<K, V, Rest ...>& _cont)
		{
			auto it( std::begin( _cont ) );
			std::advance( it, rnd_int<size_t>(0, _cont.size() - 1));
			return it->second;
		}

		template<template <typename ...> class Cont, typename K, typename ... Hash>
		inline K w_dist(const Cont<K, real, Hash...>& _cont)
		{
			std::vector<K> values;
			std::vector<real> weights;

			for (const auto& key : _cont)
			{
				values.push_back(key.first);
				weights.push_back(key.second);
			}

			size_t u(w_dist(weights));
			return values[u];
		}

	};
}


#endif // CONFIG_HPP
