#ifndef CORTEX_TYPES_HPP
#define CORTEX_TYPES_HPP

/// @todo Make this optional or bundle Armadillo code.
#include "cxxopts.hpp"
#include "json.hpp"
#include "armadillo"

/// @todo Update threadpool to the latest version.
/// @todo Update cxxopts to the latest version.
/// @todo Update dlog with printing functions
/// currently defined in Globals.hpp
/// @todo Automatic update.
#include "threadpool.hpp"
#include "dlog.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>

#include <vector>
#include <array>

#include <unordered_map>
#include <unordered_set>

#include <memory>
#include <cmath>
#include <algorithm>
#include <functional>

#include <random>
#include <chrono>
#include <cctype>

namespace Cortex
{
	///=====================================
	/// POD types
	///=====================================

	using real = float;
	using uint = unsigned int;
	using uchar = unsigned char;
	using ulong = unsigned long;
	using ullong = unsigned long long;

	///=====================================
	///	Foreign namespaces
	///=====================================

	using ThreadPool = Async::ThreadPool;
	using glock = Async::glock;
	using ulock = Async::ulock;
	using dlog = Async::dlog;
	using json = nlohmann::json;

	///=====================================
	/// Smart pointer and reference aliases.
	///=====================================

	/// Unique pointer
	template<typename T>
	using up = std::unique_ptr<T>;

	/// Shared pointer
	template<typename T>
	using sp = std::shared_ptr<T>;

	/// Reseatable / assignable reference
	template<typename T>
	using rw = std::reference_wrapper<T>;

	///=====================================
	/// Vector, matrix and cube aliases
	///=====================================

	using Vec = arma::Col<real>;
	using RVec = arma::Row<real>;
	using Mat = arma::Mat<real>;
	using Cube = arma::Cube<real>;

	///=====================================
	/// Helper aliases for restricting
	/// templates to certain types.
	///=====================================

	template<typename E>
	inline bool const IsEnum = std::is_enum<E>::value;

	template<typename N>
	inline bool const IsNum = std::is_arithmetic<N>::value;

	template<typename F>
	inline bool const IsFlt = std::is_floating_point<F>::value;

	template<typename I>
	inline bool const IsInt = std::is_integral<I>::value;

	template<typename T1, typename T2>
	inline bool const is = std::is_same<T1, T2>::value;

	template<typename E>
	using OnlyEnum = typename std::enable_if<IsEnum<E>>::type;

	template<typename N>
	using OnlyNum = typename std::enable_if<IsNum<N>>::type;

	template<typename F>
	using OnlyFlt = typename std::enable_if<IsFlt<F>>::type;

	template<typename I>
	using OnlyInt = typename std::enable_if<IsInt<I>>::type;

	template<bool cond>
	using OnlyIf = typename std::enable_if<cond>::type;

	///=====================================
	/// Minimal and maximal values
	///=====================================

	template<typename T, OnlyNum<T>...>
	inline constexpr T min = std::numeric_limits<T>::min();

	template<typename T, OnlyNum<T>...>
	inline constexpr T max = std::numeric_limits<T>::max();

	///=====================================
	/// Container aliases
	///=====================================

	/// Hashmap
	template<typename K, typename V>
	using hmap = std::unordered_map<K, V>;

	/// Hashset
	template<typename T>
	using hset = std::unordered_set<T>;

	/// Vector map (vector of pairs)
	template<typename T1, typename T2>
	using vmap = std::vector<std::pair<T1, T2>>;

	///=====================================
	/// Stream aliases
	///=====================================

	using os = std::ostream;
}

#endif // CORTEX_TYPES_HPP
