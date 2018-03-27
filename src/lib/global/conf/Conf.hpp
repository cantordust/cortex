#ifndef CORTEX_CONF_HPP
#define CORTEX_CONF_HPP

#include "Globals.hpp"
#include "Stat.hpp"
#include "ParamConf.hpp"
#include "JSONOps.hpp"

#include "ConfBase.hpp"
#include "EnvConf.hpp"
#include "SpcConf.hpp"
#include "NetConf.hpp"
#include "NodeConf.hpp"
#include "LinkConf.hpp"
#include "MatingConf.hpp"
#include "MutConf.hpp"
#include "FitConf.hpp"
#include "STDPConf.hpp"
#include "NoveltyConf.hpp"
#include "DataConf.hpp"
#include "SysConf.hpp"
#include "LayerConf.hpp"
#include "ConvConf.hpp"
#include "StatConf.hpp"
#include "ParamConf.hpp"

namespace Cortex
{
	class Conf final
	{

	public:

		EnvConf env;

		SpcConf spc;

		NetConf net;

		NodeConf node;

		LinkConf link;

		MatingConf mating;

		MutConf mut;

		FitConf fit;

		STDPConf stdp;

		NoveltyConf novelty;

		SysConf sys;

		DataConf data;

	private:

		/// @brief Path to the configuration file.
		std::string config_file;

		/// @brief JSON object holding the parsed configuration.
		json json_root;

		/// @brief Report containing problems
		/// different subconfigurations
		std::stringstream report;

		struct
		{
			/// @brief Shmaphore for accesssing the RNG engine.
			std::mutex shmaphore;

			/// @brief Global RNG.
			std::mt19937_64 engine;

		} rnd;

		friend class ConfBase;

	public:

		/// @brief Constructor taking a path to a JSON configuration
		/// file as a parameter.
		/// @param _config_file Path to the JSON configuration file.
		Conf(const std::string& _file);

		/// @brief Reset the species and net IDs to 0.
		void reset_ids();

		///=========================================
		/// JSON-related methods
		///=========================================

		/// @brief Retrieve a JSON object at the provided key sequence.
		/// @param _key_seq Key sequence in the form of "key1.key2.key3...".
		/// @return JSON object.
		json operator[] (const std::string& _key_seq);

		/// @brief Retrieve the stored root JSON object.
		/// @return JSON object.
		const json& get_json() const;

		/// @brief Check if the configuration is sane.
		void validate();

		/// @brief Load a value into a variable from a JSON configuraiton.
		/// @param _key_seq  Key sequence in the form of "key1.key2.key3...".
		/// @param _var Variable to be populated.
		/// @param _def Default value (optional).
		template<typename T>
		void load( const std::string& _key_seq, T& _var )
		{
			Cortex::load(json_root, _key_seq, _var);
		}

		/// @brief Load an associative container from a JSON configuration.
		/// @param _key_seq Key sequence in the form of "key1.key2.key3...".
		/// @param _cont Associative container.
		template<template<typename ... Args> class Cont, typename ... Ts>
		void load( const std::string& _key_seq, Cont<Ts...>& _cont )
		{
			Cortex::load(json_root, _key_seq, _cont);
		}

		///=========================================
		/// Functions returning random numbers,
		/// container elements, etc.
		///=========================================

		/// @brief Returns the outcome of a condition check
		/// with probability @p _prob.
		/// @param _prob Probability of the desired outcome.
		/// @return Boolean value indicating whether the condition was met.
		bool rnd_chance(const real _prob);

		/// @brief Returns a random number drawn from a normal distribution.
		/// @param _mean Mean of the distribution.
		/// @param _sd Standard deviation of the distribution.
		real rnd_nd(const real _mean, const real _sd);

		/// @brief Returns a number between \n 0 and the size of @p _weights
		/// @param _weights
		size_t w_dist(const std::vector<real>& _weights);

		/// @brief Returns a random integer value between @p _min and @p _max.
		/// @param _min Minimal value.
		/// @param _max Maximal value.
		template<typename T = uint, typename std::enable_if<std::is_integral<T>::value>::type ...>
		T rnd_int(const T _min, const T _max)
		{
			glock lk(rnd.shmaphore);
			std::uniform_int_distribution<T> dist(_min, _max);
			return dist(rnd.engine);
		}

		/// @brief Returns a random floating-point value between @p _min and @p _max.
		/// @param _min Minimal value.
		/// @param _max Maximal value.
		real rnd_real(const real _min, const real _max)
		{
			glock lk(rnd.shmaphore);
			std::uniform_real_distribution<real> dist(_min, _max);
			return dist(rnd.engine);
		}

		/// @brief Returns a random element from a non-associative container
		/// (such as std::vector).
		/// @param _cont A non-associative container	compatible with std::begin().
		/// @return Reference to a random element.
		template<template<typename ... Args> class Cont, typename T, typename ... Rest>
		const T& rnd_elem(const Cont<T, Rest...>& _cont)
		{
			auto it( std::begin( _cont ) );
			std::advance(it, rnd_int<size_t>(0, _cont.size() - 1));
			return *it;
		}

		/// @brief Returns a random key from an associative container
		/// (such as std::map).
		/// @param _cont An associative container compatible with std::begin().
		/// @return Reference to a random key.
		template<template <typename ... Args> class Cont, typename K, typename V, typename ... Rest>
		const K& rnd_key(const Cont<K, V, Rest ...>& _cont)
		{
			auto it(std::begin(_cont));
			std::advance(it, rnd_int<size_t>(0, _cont.size() - 1));
			return it->first;
		}

		/// @brief Returns a reference to a random value of an
		/// associative container (such as std::map).
		/// @param _cont An associative container compatible with std::begin().
		/// @return Reference to a random value.
		template<template <typename ... Args> class Cont,
				 typename K,
				 typename V,
				 typename ... Rest>
		const V& rnd_val(const Cont<K, V, Rest ...>& _cont)
		{
			auto it( std::begin( _cont ) );
			std::advance( it, rnd_int<size_t>(0, _cont.size() - 1));
			return it->second;
		}

		/// @brief Returns a random key from an associative container
		/// containing elements as keys and their respective weights as values.
		/// The weights must be floating-point values (float or double).
		/// @param _cont Associative container with weights as values.
		/// @return Random key drawn from a weighted distribution.
		template<template <typename ... Args> class Cont,
				 typename K,
				 typename V,
				 typename ... Rest,
				 typename std::enable_if<std::is_arithmetic<V>::value, V>::type ...>
		K w_dist(const Cont<K, V, Rest...>& _cont)
		{
			std::vector<K> values;
			std::vector<real> weights;
			values.reserve(_cont.size());
			weights.reserve(_cont.size());

			if (_cont.size() == 0)
			{
				dlog() << "### Error: Empty weighted distribution.\n";
				exit(EXIT_FAILURE);
			}
			for (const auto& key : _cont)
			{
				values.push_back(key.first);
				weights.push_back(static_cast<real>(key.second));
			}

			size_t u(w_dist(weights));
			return values.at(u);
		}

		friend std::ostream& operator << (std::ostream& _strm, const Conf& _conf);

	private:

		/// @brief Initialise the default configuration options.
		void set_defaults();

		/// @brief Load a value into a variable from a JSON configuraiton.
		/// @param _key_seq  Key sequence in the form of "key1.key2.key3...".
		/// @param _var Variable to be populated.
		/// @param _def Default value (optional).
		void load();
	};
}

#endif // CORTEX_CONF_HPP
