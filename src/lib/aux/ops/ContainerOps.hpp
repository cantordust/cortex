#ifndef CORTEX_CONTAINER_OPS_HPP
#define CORTEX_CONTAINER_OPS_HPP

#include "Types.hpp"

namespace Cortex
{
	///=====================================
	/// Shortcuts for make_unique and make_shared
	///=====================================

	template<typename T, typename ... Args>
	inline constexpr sp<T> mksp(Args&& ... _args)
	{
		return std::make_shared<T>(std::forward<Args>(_args)...);
	}

	template<typename T, typename ... Args>
	inline constexpr up<T> mkup(Args&& ... _args)
	{
		return std::make_unique<T>(std::forward<Args>(_args)...);
	}

	///=====================================
	///	Shortcut functions to check if a key
	/// exists in a hashmap or a hashset
	///=====================================

	template<typename T1, typename T2>
	inline bool is_key(const hmap<T1, T2>& _map, const T1& _key)
	{
		return _map.find(_key) != _map.end();
	}

	template<typename T>
	inline bool is_key(const hset<T>& _set, const T& _key)
	{
		return _set.find(_key) != _set.end();
	}

	///=====================================
	/// Batch removal of entries from
	/// hashmaps and hashsets
	///=====================================

	template<typename K,
			 typename V,
			 typename ... Ks,
			 OnlyIf< And< std::is_same<K, Ks>... >::value > ... >
	inline void disable(hmap<K, V>& _map, Ks&& ... _keys)
	{
		std::array<int, sizeof...(_keys)> status{(_map.erase(std::forward<Ks>(_keys)), 0) ...};
	}

	template<typename K,
			 typename ... Ks,
			 OnlyIf< And< std::is_same<K, Ks>... >::value > ... >
	inline void disable(hset<K>& _set, Ks&& ... _keys)
	{
		std::array<int, sizeof...(_keys)> status{(_set.erase(std::forward<Ks>(_keys)), 0) ...};
	}

	///=====================================
	/// Pretty-printing for containers
	///=====================================

	template<typename K, typename V>
	inline os& operator << (os& _os, const hmap<K, V>& _map)
	{
		if (!_map.empty())
		{
			for (const auto& elem : _map)
			{
				_os  << "\n\t" << elem.first << ": " << elem.second;
			}
		}
		return _os;
	}

	template<template<typename...> class Cont, typename T,
			 OnlyIf<is<Cont<T>, std::vector<T>> || is<Cont<T>, hset<T>> > ... >
	inline os& operator << (os& _os, const Cont<T>& _cont)
	{
		if (!_cont.empty())
		{
			uint i(0);
			for (const auto& _elem : _cont)
			{
				_os << _elem << (i++ < _cont.size() - 1 ? ", " : "");
			}
		}
		return _os;
	}

	template<template<typename...> class Container, typename Lambda, typename ... Ts>
	inline uint count(const Container<Ts...>& _cont, Lambda&& _lambda)
	{
		return std::accumulate(std::cbegin(_cont),
							   std::cend(_cont),
							   0,
							   _lambda);
	}

	template<template<typename...> class Container, typename Lambda, typename ... Ts>
	inline void apply(const Container<Ts...>& _cont, Lambda&& _lambda)
	{
		for (const auto& elem : _cont)
		{
			_lambda(elem);
		}
	}

	template<template<typename, typename...> class Container, typename T, typename ... Ts>
	inline T min_element(const Container<T, Ts...>& _cont)
	{
		return *std::min_element(std::cbegin(_cont), std::cend(_cont));
	}
}

#endif // CORTEX_CONTAINER_OPS_HPP
