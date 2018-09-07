#ifndef CORTEX_RANDOM_HPP
#define CORTEX_RANDOM_HPP

#include "Types.hpp"

namespace Cortex
{
	///=============================================================================
	///	Random numbers, elements, distributions, etc.
	///=============================================================================

	/// Semaphore for accesssing the RNG engine.
	inline std::mutex semaphore;

	/// Random number generator.
	inline std::mt19937_64 rng_engine;

	/// @brief Seed the random number generator.
	inline void seed_rng()
	{
		rng_engine.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	}

	/// @brief Returns a random number drawn from a normal distribution.
	/// @param _mean Mean of the distribution.
	/// @param _sd Standard deviation of the distribution.
	inline real rnd_nd(const real _mean, const real _sd)
	{
		glock lk(semaphore);
		return std::normal_distribution<real>(_mean, _sd)(rng_engine);
	}

	/// @brief Returns a random number drawn from
	/// the positve half of a normal distribution
	/// centred at 0.
	/// @param _sd Standard deviation of the distribution.
	inline real rnd_pos_nd(const real _sd)
	{
		return std::fabs(rnd_nd(0.0, _sd));
	}

	/// @brief Returns a random number drawn from
	/// the negative half of a normal distribution
	/// centred at 0.
	/// @param _sd Standard deviation of the distribution.
	inline real rnd_neg_nd(const real _sd)
	{
		return -std::fabs(rnd_nd(0.0, _sd));
	}

	/// @brief Returns a random floating-point value between @p _min and @p _max.
	/// @param _min Minimal value.
	/// @param _max Maximal value.
	inline real rnd_real(const real _min, const real _max)
	{
		glock lk(semaphore);
		return std::uniform_real_distribution<real> (_min, _max)(rng_engine);
	}

	/// @brief Returns the outcome of a condition check
	/// with probability @p _prob.
	/// @param _prob Probability of the desired outcome.
	/// @return Boolean value indicating whether the condition was met.
	inline bool rnd_chance(const real _prob)
	{
		return rnd_real(0.0, 1.0) <= _prob;
	}

	/// @brief Returns a number between \n 0 and the size of @p _weights.
	///	The number is selected with probability proportional to its weight.
	/// @param _weights
	inline uint roulette(const std::vector<real>& _weights)
	{
		glock lk(semaphore);
		return std::discrete_distribution<size_t>(_weights.begin(), _weights.end())(rng_engine);
	}

	/// @brief Returns a random integer value between @p _min and @p _max.
	/// @param _min Minimal value.
	/// @param _max Maximal value.
	template<typename T = uint, OnlyInt<T> ...>
	inline T rnd_int(const T _min, const T _max)
	{
		glock lk(semaphore);
		return std::uniform_int_distribution<T>(_min, _max)(rng_engine);
	}

	/// @brief Returns a random element from a non-associative container.
	template<template<typename ... Args> class Cont, typename T, typename ... Rest>
	inline const T& rnd_elem(const Cont<T, Rest...>& _cont)
	{
		auto it(std::cbegin(_cont));
		std::advance(it, rnd_int<uint>(0, _cont.size() - 1));
		return *it;
	}

	/// @brief Returns a const reference to a random key
	/// from an associative container.
	template<template <typename ... Args> class Cont, typename K, typename V, typename ... Rest>
	inline const K& rnd_key(const Cont<K, V, Rest ...>& _cont)
	{
		auto it(std::cbegin(_cont));
		std::advance(it, rnd_int<uint>(0, _cont.size() - 1));
		return it->first;
	}

	/// @brief Returns a const reference to a random value
	/// from an associative container.
	template<template <typename ... Args> class Cont,
			 typename K,
			 typename V,
			 typename ... Rest>
	inline const V& rnd_val(const Cont<K, V, Rest ...>& _cont)
	{
		auto it(std::cbegin(_cont));
		std::advance(it, rnd_int<uint>(0, _cont.size() - 1));
		return it->second;
	}
}

#endif // CORTEX_RANDOM_HPP
