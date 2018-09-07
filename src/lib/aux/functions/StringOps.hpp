#ifndef CORTEX_STRING_OPS_HPP
#define CORTEX_STRING_OPS_HPP

#include "Types.hpp"

namespace Cortex
{
	///=====================================
	///	String manipulation functions
	///=====================================

	/// @brief Converts a string to lowercase.
	inline void lcase(std::string& _str)
	{
		std::transform(_str.begin(), _str.end(), _str.begin(), ::tolower);
	}

	inline std::string lcase(const std::string& _str)
	{
		std::string s(_str);
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		return s;
	}

	/// @brief Converts a string to uppercase.
	inline void ucase(std::string& _str)
	{
		std::transform(_str.begin(), _str.end(), _str.begin(), ::toupper);
	}

	inline std::string ucase(const std::string& _str)
	{
		std::string s(_str);
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
		return s;
	}

	inline std::string alnum(const std::string& _str)
	{
		std::string s(lcase(_str));
		s.erase(std::remove_if(s.begin(),
							   s.end(),
							   [](uchar _l) { return !std::isalnum(_l); }),
				s.end());
		return s;
	}

	inline std::deque<std::string> split(const std::string& _key_seq, const char _sep = '.')
	{
		std::deque<std::string> keys;
		std::stringstream ss(_key_seq);
		std::string key;

		while ( std::getline( ss, key, _sep ) )
		{
			keys.push_back( key );
		}
		return keys;
	}

	inline void strip(std::string& _str, const char _ch = ' ')
	{
		_str.erase(std::remove(_str.begin(),
							   _str.end(),
							   _ch),
				   _str.end());
	}

	///=====================================
	///	Conditional reporting and exceptions
	///=====================================

	template<typename ... Args>
	inline void check(os& _os, const bool _condition, Args&& ... _args)
	{
		if (!_condition)
		{
			dlog(_os, std::forward<Args>(_args)...);
		}
	}

	template<typename ... Args>
	inline void check(const bool _condition, Args&& ... _args)
	{
		check(std::cout, _condition, std::forward<Args>(_args)...);
	}

	template<typename ... Args>
	inline void die(Args&& ... _args)
	{
		dlog("[Error] ", std::forward<Args>(_args)..., "\nExiting.");
		throw std::exception();
	}

	template<typename ... Args>
	inline void require(const bool _condition, Args&& ... _args)
	{
		if (!_condition)
		{
			die(std::forward<Args>(_args)...);
		}
	}
}

#endif // CORTEX_STRING_OPS_HPP
