#ifndef CORTEX_JSONOPS_HPP
#define CORTEX_JSONOPS_HPP

#include "Globals.hpp"

namespace Cortex
{
	bool traverse(json& _j, json::iterator& _it, const std::string& _key_seq);

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	void check_enum(const E& _e, const std::string& _str)
	{
		if (_e == Enum<E>::undef)
		{
			dlog() << "Invalid enum entry '" << _str << "'";
			exit(EXIT_FAILURE);
		}
	}
	void check_array(const json& _j);

	void check_object(const json& _j);

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	void from_json(const json& _j, E& _e)
	{
		_e = to_enum<E>(_j.get<std::string>());
	}

	template<typename T>
	void load (const json& _j, T& _var);

	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ...>
	void from_json(const json& _j, T& _val)
	{
		_val = _j.get<T>();
	}

	template<typename T, typename std::enable_if<!(std::is_enum<T>::value || std::is_arithmetic<T>::value)>::type ...>
	void from_json(const json& _j, T& _val)
	{
		load(_j, _val);
	}

	template<template<typename ... Args> class Cont,
			 typename E,
			 typename V,
			 typename ... Rest,
			 typename std::enable_if<std::is_enum<E>::value>::type ...>
	void from_json(const json& _j, Cont<E, V, Rest...>& _map)
	{
		check_object(_j);
		for (const auto& entry : _j.get<hmap<std::string, json>>() )
		{
			E e(to_enum<E>(entry.first));
			check_enum(e, entry.first);
			_map[e] = entry.second;
		}
	}

	template<template<typename ... Args> class Cont,
			 typename E,
			 typename ... Rest,
			 typename std::enable_if< std::is_enum<E>::value, E>::type ...>
	void from_json(const json& _j, Cont<E, Rest...>& _set)
	{
		check_array(_j);
		for (const auto& entry : _j.get<hset<std::string>>() )
		{
			E e(to_enum<E>(entry));
			check_enum(e, entry);
			_set.insert(e);
		}
	}

	template<template<typename ... Args> class Cont,
			 typename T,
			 typename V,
			 typename ... Rest,
			 typename std::enable_if<!std::is_enum<T>::value>::type ...>
	void from_json(const json& _j, Cont<T, V, Rest...>& _map)
	{
		check_object(_j);
		_map = _j.get<Cont<T, V, Rest...>>();
	}

	template<template<typename ... Args> class Cont,
			 typename T,
			 typename ... Rest,
			 typename std::enable_if<!std::is_enum<T>::value>::type ...>
	void from_json(const json& _j, Cont<T, Rest...>& _set)
	{
		check_array(_j);
		_set = _j.get<Cont<T, Rest...>>();
	}

	template<typename T>
	void load(const json& _j, const std::string& _key_seq, T& _var)
	{
		json j(_j);
		auto it(j.begin());

		if (traverse(j, it, _key_seq))
		{
			from_json(*it, _var);
		}
	}

	template<template<typename ... Args> class Cont, typename ... Ts>
	void load(const json& _j, const std::string& _key_seq, Cont<Ts...>& _cont)
	{
		json j(_j);
		auto it(j.begin());

		if (traverse(j, it, _key_seq))
		{
			from_json(*it, _cont);
		}
	}

	bool parse_json(const std::string& _file, json& _json_root);

}

#endif // CORTEX_JSONOPS_HPP
