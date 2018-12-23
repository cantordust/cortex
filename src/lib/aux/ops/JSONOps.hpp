#ifndef CORTEX_JSON_OPS_HPP
#define CORTEX_JSON_OPS_HPP

#include "StringOps.hpp"
#include "Types.hpp"

/// Load parameter values from JSON.
#define JLOAD(json_node, keys) \
	embed(const_cast<json&>(json_node), #keys); \
	cortex::load(const_cast<json&>(json_node), #keys, keys);

/// Save parameters to JSON nodes.
#define JSAVE(json_node, keys) \
	cortex::save(json_node, #keys, keys);

namespace Cortex
{
	///=============================================================================
	///	JSON object structure manipulators and iterators
	///=============================================================================

	/// @brief Parse a root JSON object
	inline void parse(const std::string& _file, json& _json_root)
	{
		// Open, parse and load the JSON file
		std::ifstream ifs(_file);

		require(ifs.is_open(), "Error opening file ", _file);

		std::stringstream ss;
		ss << ifs.rdbuf();
		std::string content(ss.str());

		_json_root = content.empty() ? json() : json::parse(content);
	}

	/// @brief Traverse the JSON tree by following the key
	/// order in @p _keys
	inline json walk(const json& _j, std::deque<std::string>&& _keys)
	{
		if (_keys.empty())
		{
			return _j;
		}
		std::string key(_keys.front());
		_keys.pop_front();
		return walk(_j[key], std::move(_keys));
	}

	/// @brief Traverse the JSON tree by following the key
	/// order in @p _keys
	inline json walk(const json& _j, const std::string& _keys)
	{
		json copy(_j);
		return walk(copy, split(_keys));
	}

	/// @brief Traverse the JSON tree by following the key
	/// order in @p _keys and embed the keys if missing
	inline json& embed(json& _j, std::deque<std::string>&& _keys)
	{
		if (_keys.empty())
		{
			if (_j.is_null() ||
				_j.empty())
			{
				_j = json();
			}
			return _j;
		}

		std::string key(_keys.front());
		_keys.pop_front();
		return embed(_j[key], std::move(_keys));
	}

	/// @brief Traverse the JSON tree by following the key
	/// order in @p _keys and embed the keys if missing.
	inline json& embed(json& _j, const std::string& _keys)
	{
		return embed(_j, split(_keys));
	}

	///=====================================
	/// @struct Serialisation base
	///=====================================
	struct Serialiser
	{
		virtual void load(const json& _j) {};
		virtual void save(json& _j) const {};
	};

	template<class Class, OnlyIf<std::is_base_of<Serialiser, Class>::value>...>
	void from_json(const json& _j, Class& _class)
	{
		_class.load(_j);
	}

	template<class Class, OnlyIf<std::is_base_of<Serialiser, Class>::value>...>
	void to_json(json& _j, const Class& _class)
	{
		_class.save(_j);
	}

	///=============================================================================
	///	JSON serialisers for various custom types
	///=============================================================================

//	/// @brief Load a hashset.
//	template<typename T>
//	inline void from_json(const json& _j, hset<T>& _set)
//	{
//		_set.clear();
//		for (const auto& j : _j.get<hset<json>>())
//		{
//			_set.emplace(j.get<T>());
//		}
//	}

	/// @brief Load a hashmap.
	template<typename T1, typename T2>
	inline void from_json(const json& _j, hmap<T1, T2>& _map)
	{
		if (!(_j.empty() || _j.is_null()))
		{
			_map.clear();
			for (const auto& j : _j.get<hmap<json, json>>())
			{
				_map[j.first.get<T1>()] = j.second.get<T2>();
			}
		}
	}

	/// @brief Save a hashmap.
	template<typename T1, typename T2>
	inline void to_json(json& _j, const hmap<T1, T2>& _map)
	{
		for (const auto& entry : _map)
		{
			std::stringstream ss;
			ss << entry.first;
			_j[ss.str()] = entry.second;
		}
	}

//	/// @brief Load a vector.
//	template<typename T>
//	inline void from_json(const json& _j, std::vector<T>& _vec)
//	{
//		_vec.clear();
//		for (const auto& j : _j.get<std::vector<json>>())
//		{
//			_vec.emplace_back(j.get<T>());
//		}
//	}

	/// @brief Load a generic parameter by traversing the JSON tree.
	/// The keys are assumed to be hierarchical and stored in @p _keys
	/// in dot-separated format (key1.key2.param ...).
	template<typename VarType>
	inline void load(json& _j, const std::string& _keys, VarType& _var)
	{
		json j(walk(_j, _keys));

		if (!(j.empty() || j.is_null()))
		{
			_var = j.get<VarType>();

//			dlog("Loaded ", _keys, ":\t", _var);
		}
	}

	/// @brief Save a parameter to a JSON node.
	/// The keys are assumed to be hierarchical and stored in @p _keys
	/// in dot-separated format (key1.key2.param ...).
	template<typename VarType>
	inline void save(json& _j, const std::string& _keys, VarType& _var)
	{
//		dlog("Saving ", _keys, ":\t", _var);

		embed(_j, _keys) = _var;
	}
}

#endif // CORTEX_JSON_OPS_HPP
