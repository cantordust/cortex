#ifndef CORTEX_TYPES_HPP
#define CORTEX_TYPES_HPP

#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <bitset>

#include <string>

#include <memory>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <type_traits>
#include <functional>

#include <random>
#include <chrono>
#include <atomic>

namespace Cortex
{

	///=========================================
	/// Forward declarations and aliases
	///=========================================

	/// These affect the entire library.
	using real = double;
	using uchar = unsigned char;
	using uint = unsigned int;
	using flag = std::atomic<bool>;
	using areal = std::atomic<real>;
	using auint = std::atomic<uint>;

	template<typename T1, typename T2> using hmap = std::unordered_map<T1, T2>;
	template<typename T> using hset = std::unordered_set<T>;
	template<typename T> using uptr = std::unique_ptr<T>;
	template<typename T> using sptr = std::shared_ptr<T>;
	template<typename T> using rfw = std::reference_wrapper<T>;

	/// @todo Switch to tensors (cf. Tensor.hpp)
//	using Point = std::vector<real>;
//	using Substrate = std::vector<std::pair<Point, uint>>;

	/// Forward class declarations
	class Conf;
	class Ecosystem;
	class Species;
	class Genotype;
	class Net;
	class Topology;
	class Node;
	class Link;
	class Param;
	class Stat;
	class Fitness;
	class Data;
	class Sample;

	struct NodeID;
	struct Soma;
	struct Axon;

	/// Reference wrappers
	using ConfRef = rfw<Conf>;
	using EcoRef = rfw<Ecosystem>;
	using SpcRef = rfw<Species>;
	using NetRef = rfw<Net>;
	using NodeRef = rfw<Node>;
	using LinkRef = rfw<Link>;
	using ParamRef = rfw<Param>;
	using SampleRef = rfw<Sample>;

	/// Pointers
	using ConfPtr = sptr<Conf>;
	using NetPtr = sptr<Net>;
	using SpcPtr = sptr<Species>;
	using NodePtr = sptr<Node>;
	using LinkPtr = sptr<Link>;
	using DataPtr = sptr<Data>;
	using ParamPtr = sptr<Param>;

	///=========================================
	/// Constants
	///=========================================

	/// @brief Golden ratio
	/// phi = (1 + sqrt(5))/2 = 1.618...
	/// Can be used for pretty Fibonacci fractals!?
	static constexpr real phi = static_cast<real>(1.6180339887498948482);

	/// @brief Self-explanatory
	static constexpr real pi = static_cast<real>(M_PI);
	static constexpr real two_pi = static_cast<real>(2.0 * M_PI);
}

#endif // CORTEX_TYPES_HPP
