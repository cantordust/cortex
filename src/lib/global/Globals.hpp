#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <vector>
#include <memory>
#include <queue>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>

#include "Enum.hpp"
#include "threadpool.hpp"
#include "dlog.hpp"

namespace Cortex
{
	using namespace Async;

	const std::string version("0.5.0");

	/// This affects the entire library.
	using real = double;
	using uint = unsigned int;
	using ulong = unsigned long;

	using ulock = std::unique_lock<std::mutex>;
	using glock = std::lock_guard<std::mutex>;

	/// Key: spike time
	/// Value: source - target pair
	using event = std::pair<real, uint>;
	using event_pair = std::pair<real, std::pair<uint, uint>>;
	using Scheduler = std::priority_queue<event_pair, std::vector<event_pair>, std::greater<event_pair>>;

//	/// The substrate can have any dimensionality.
//	/// Key: Point (a point in space)
//	/// Value: node ID
//	typedef std::vector<real> Point;
//	typedef std::vector<std::pair<Point, uint>> Substrate;

	template<typename T1, typename T2> using hmap = std::unordered_map<T1, T2>;
	template<typename T> using hset = std::unordered_set<T>;

	class Ecosystem;
	class Species;
	class Net;
	class Node;
	class Link;
	class Param;

	using EcosystemRef = std::reference_wrapper<Ecosystem>;
	using SpeciesRef = std::reference_wrapper<Species>;
	using NetRef = std::reference_wrapper<Net>;
	using NodeRef = std::reference_wrapper<Node>;
	using NodePtr = std::unique_ptr<Node>;
	using LinkRef = std::reference_wrapper<Link>;
	using LinkPtr = std::unique_ptr<Link>;
	using ParamRef = std::reference_wrapper<Param>;

	/// Some constants.
	/// Golden ratio
	/// phi = (1 + sqrt(5))/2 = 1.618...
	/// Can be used for pretty Fibonacci fractals!?
	static constexpr real phi = static_cast<real>(1.6180339887498948482);
	static constexpr real pi = static_cast<real>(M_PI);
	static constexpr real two_pi = static_cast<real>(2.0 * M_PI);
}

#endif // GLOBALS_HPP
