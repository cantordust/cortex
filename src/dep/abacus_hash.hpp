#ifndef ABACUS_HASH
#define ABACUS_HASH

/// @brief Uncomment if printing hashes and bitsets.
//#include <iostream>

#include <tuple>
#include <bitset>

namespace AbacusHash
{
	namespace Internal
	{
		/// @brief Returns the underlying type
		/// for enum class and the type itself
		/// for integral types.
		template<typename T, typename std::enable_if<std::is_integral<T>::value, void>::type ... >
		inline constexpr auto utype (const T _t) noexcept
		{
			return _t;
		}

		template<typename E, typename std::enable_if<std::is_enum<E>::value, void>::type ... >
		inline constexpr auto utype (const E _e) noexcept
		{
			return static_cast<typename std::underlying_type<E>::type>(_e);
		}

		/// @brief Sum of the sizes (in bytes) of all elements
		/// of a tuple (as returned by sizeof(Type)).
		template<typename T, typename ... Rest>
		inline constexpr size_t tuple_size()
		{
			if constexpr (sizeof ... (Rest) == 0)
			{
				return sizeof(T);
			}
			else
			{
				return sizeof (T) + tuple_size<Rest...>();
			}
		}

		/// @brief Create a temporary bitset
		/// initialised with element @p _t,
		/// shift it n bits (the sum of the sizes
		/// of all types hashed so far) and then
		/// OR the result with the main bitset @p _h.
		template<size_t bs, size_t shift, typename T>
		inline constexpr void hashme(std::bitset<bs>& _h, T _t)
		{
			std::bitset<bs> h_aux(_t);
			//	std::cout << "(hashme) h_aux:  " << h_aux.to_string() << "\n";
			h_aux <<= 8 * shift;
			//	std::cout << "(hashme) h_aux:  " << h_aux.to_string() << "\n";
			_h |= h_aux;
		}

		/// @brief Returns the hash of a bitset of size @p bs.
		template<size_t bs, size_t elem, size_t shift, typename T, typename ... Rest>
		inline constexpr size_t bitset_hash(std::bitset<bs>&& _h, const std::tuple<T, Rest...>& _tpl)
		{
			hashme<bs, shift>(_h, utype(std::get<elem>(_tpl)));
			//	std::cout << "(bitset_hash) h: " << _h.to_string() << "\n";
			if constexpr (elem == sizeof ... (Rest))
			{
				return std::hash<std::bitset<bs>>()(_h);
			}
			else
			{
				return bitset_hash<bs, elem + 1, shift + sizeof(decltype (std::get<elem>(_tpl))), T, Rest...>(std::move(_h), _tpl);
			}
		}

		template<typename Array, std::size_t... Is>
		auto constexpr array_to_tuple(const Array& _array, std::index_sequence<Is...>) -> decltype(std::make_tuple(_array[Is]...))
		{
			return std::make_tuple(_array[Is]...);
		}

		template<typename T, std::size_t Size, typename Indices = std::make_index_sequence<Size>>
		auto constexpr array_to_tuple(const std::array<T, Size>& _array) -> decltype(array_to_tuple(_array, Indices()))
		{
			return array_to_tuple(_array, Indices());
		}
	}

	/// @brief Returns the hash of a tuple.
	/// The tuple elements must be integral or enum class.
	template<typename T, typename ... Rest>
	inline constexpr size_t tuple_hash(const std::tuple<T, Rest ...>& _tpl)
	{
		std::bitset<8 * Internal::tuple_size<T, Rest...>()> h(Internal::utype(std::get<0>(_tpl)));
	//	std::cout << "(tuple_hash) h:  " << h.to_string() << "\n";
		return Internal::bitset_hash<8 * Internal::tuple_size<T, Rest...>(), 1, sizeof (T), T, Rest...>(std::move(h), _tpl);
	}

	template<typename T, typename ... Rest>
	inline constexpr size_t hash(T&& _t, Rest&& ... _rest)
	{
		return tuple_hash(std::make_tuple(std::forward<T>(_t), std::forward<Rest>(_rest)...));
	}

	/// @brief Returns the hash of a tuple.
	/// The tuple elements must be integral or enum class.
	template<typename T, std::size_t Size>
	inline constexpr size_t array_hash(const std::array<T, Size>& _array)
	{
		return tuple_hash(Internal::array_to_tuple(_array));
	}
}

#endif // ABACUS_HASH
