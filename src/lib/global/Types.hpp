#ifndef CORTEX_TYPES_HPP
#define CORTEX_TYPES_HPP

#include <iostream>
#include <sstream>
#include <fstream>

#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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

/// @todo Make this optional OR
/// bundle Armadillo code
#include <armadillo>

namespace Cortex
{
	///=========================================
	/// Fundamental types
	///=========================================

	/// These affect the entire library.
	using real = double;
	using uchar = unsigned char;
	using uint = unsigned int;
	using flag = std::atomic<bool>;
	using areal = std::atomic<real>;
	using auint = std::atomic<uint>;

	///=========================================
	/// Vectors and matrices
	///=========================================

	using mat = arma::sp_mat;
	using vec = arma::vec;

	///=========================================
	/// Containers
	///=========================================

	/// Hashmap
	template<typename K, typename V>
	using hmap = std::unordered_map<K, V>;

	/// Hashset
	template<typename T>
	using hset = std::unordered_set<T>;

	///=========================================
	/// Smart pointers and references
	///=========================================

	/// Unique pointer
	template<typename T> using uptr = std::unique_ptr<T>;

	/// Shared pointer
	template<typename T> using sptr = std::shared_ptr<T>;

	/// Reseatable / assignable reference
	template<typename T> using rfw = std::reference_wrapper<T>;

	///=========================================
	/// Forward declarations
	///=========================================

	/// @brief Core classes
	class Experiment;
	class Env;
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
	class RField;
	class Evaluator;

	/// @brief Structs
	struct NodeID;
	struct Soma;
	struct Axon;

	/// @brief Configuration classes
	class Conf;
	class ConfBase;
	class EnvConf;
	class SpcConf;
	class NetConf;
	class NodeConf;
	class LinkConf;
	class MatingConf;
	class MutConf;
	class FitConf;
	class STDPConf;
	class NoveltyConf;
	class DataConf;
	class SysConf;
	class ParamConf;
	class StatConf;
	class LayerConf;
	class ConvConf;

	/// @brief Reference wrappers
	using NetRef = rfw<Net>;
	using ParamRef = rfw<Param>;
	using SampleRef = rfw<Sample>;

	/// @brief Pointers
	using NetPtr = sptr<Net>;
	using SpcPtr = sptr<Species>;
	using NodePtr = sptr<Node>;
	using LinkPtr = sptr<Link>;
	using DataPtr = sptr<Data>;

	/// @brief Network evaluation function
	using EvalFunc = std::function<void(Net&)>;

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
