#ifndef CORTEX_TYPES_HPP
#define CORTEX_TYPES_HPP

/// @todo Automatic update of dependencies
#include <torch/torch.h>

/// @todo Update to the latest version.
#include "cxxopts.hpp"
#include "clara.hpp"

/// @todo Update to the latest version.
#include "json.hpp"

/// @todo Update to the latest version.
#include "threadpool.hpp"

/// @todo Update to the latest version.
#include "dlog.hpp"

/// @todo Update to the latest version.

#include "abacus_hash.hpp"

#ifdef GENERATE_PYTHON_BINDINGS

/// @todo Update to the latest version.
#include "pybind11.h"

#endif

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
#include <optional>

#include <random>
#include <chrono>
#include <cctype>

namespace Cortex
{
	///=====================================
	/// POD types
	///=====================================

	using real = float;
	using sz = std::size_t;
	using u8 = std::uint_fast8_t;
	using u16 = std::uint_fast16_t;
	using u32 = std::uint_fast32_t;
	using u64 = std::uint_fast64_t;
	using uint = u32;
	using uchar = unsigned char;
	using ulong = unsigned long;
	using ullong = unsigned long long;

	///=====================================
	/// Optional
	///=====================================

	template<typename T>
	using opt = std::optional<T>;

	inline constexpr std::nullopt_t nopt{std::nullopt};

	///=====================================
	///	Foreign namespaces
	///=====================================

	using ThreadPool = Async::ThreadPool;
	using toggle = Async::toggle;
	using glock = Async::glock;
	using ulock = Async::ulock;
	using dlog = Async::dlog;
	using json = nlohmann::json;

#ifdef GENERATE_PYTHON_BINDINGS
	namespace py = pybind11;
#endif

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
	/// Helper aliases for restricting
	/// templates to certain types.
	///=====================================

	template<typename ... Conds>
	struct And : std::true_type {};

	template<typename Cond, typename... Conds>
	struct And<Cond, Conds...>
			:
			std::conditional<Cond::value, And<Conds...>,
			std::false_type>::type
	{};

	template<typename T1, typename T2>
	using Is = std::is_same<T1, T2>;

	template<typename T1, typename T2>
	inline bool const is = Is<T1, T2>::value;

	template<typename E>
	inline bool const IsEnum = std::is_enum<E>::value;

	template<typename N>
	inline bool const IsNum = std::is_arithmetic<N>::value;

	template<typename F>
	inline bool const IsFlt = std::is_floating_point<F>::value;

	template<typename I>
	inline bool const IsInt = std::is_integral<I>::value;

	template<typename N>
	inline bool const IsSig = std::is_signed<N>::value;

	template<bool cond, typename Type = std::true_type>
	using OnlyIf = typename std::enable_if<cond, Type>::type;

	template<typename E>
	using OnlyEnum = OnlyIf<IsEnum<E>>;

	template<typename N>
	using OnlyNum = OnlyIf<IsNum<N>>;

	template<typename F>
	using OnlyFlt = OnlyIf<IsFlt<F>>;

	template<typename I>
	using OnlyInt = OnlyIf<IsInt<I>>;

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

	///=============================================================================
	/// Forward declarations
	///=============================================================================

	///=====================================
	/// Core classes
	///=====================================

	class Net;
	class Layer;
	class Node;
	class Link;
	class Model;
	class Genome;
	class Mutation;
	class Parameter;
	class Fitness;
	class Sample;
	class Data;

	///=====================================
	/// Definition and configuration classes
	///=====================================
	struct InitDef;
	struct NodeDef;
	struct LayerDef;
	struct Conf;

	///=====================================
	/// Pointer and reference aliases
	///=====================================

	/// Reference wrappers
	using SampleRef = rw<Sample>;
	using ParamRef = rw<Parameter>;

	/// Smart pointers
	using NetPtr = sp<Net>;
	using LayerPtr = sp<Layer>;
	using NodePtr = sp<Node>;
	using ModelPtr = sp<Model>;
	using GenomePtr = sp<Genome>;
	using MutationPtr = sp<Mutation>;

	/// Other convenience containers
	using DNA = std::vector<LayerPtr>;

	/// Iterators
	using GenomeIt = std::vector<LayerDef>::iterator;
	using DNAIt = DNA::iterator;
	using GeneIt = std::vector<NodePtr>::iterator;
}
#endif // CORTEX_TYPES_HPP
