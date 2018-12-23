#ifndef CORTEX_MATH_OPS_HPP
#define CORTEX_MATH_OPS_HPP

#include "Types.hpp"
#include "Enum.hpp"

namespace Cortex
{
	///=====================================
	///	std::vector<real>h functions
	///=====================================

	namespace Const
	{
		/// @todo Other constants?
		inline const real phi((1.0 + std::sqrt(5.0)) / 2.0);
		inline const real pi(M_PI);
		inline const real two_pi(2.0 * M_PI);
		inline const real half_pi(0.5 * M_PI);
	}

	template<FuncType f>
	inline real function(const real _x);

	template<FuncType f>
	inline real function(const std::vector<real>& _v);

	///=====================================
	/// Explicit specialisations
	///=====================================

	template<>
	inline real function<FuncType::Abs>(const real _x)
	{
		return std::fabs(_x);
	}

	template<>
	inline real function<FuncType::Cos>(const real _x)
	{
		return std::cos(_x);
	}

	template<>
	inline real function<FuncType::Gaussian>(const real _x)
	{
		return std::exp(-0.5 * std::pow(_x, 2));
	}

	template<>
	inline real function<FuncType::Logistic>(const real _x)
	{
		return 0.5 * (std::tanh( 0.5 * _x ) + 1.0);
	}

	/// @brief Differentiable ReLU passing through the origin.
	template<>
	inline real function<FuncType::ReLU>(const real _x)
	{
		return (0.5 * (std::sqrt(std::pow(_x, 2) + 4) + _x) - 1.0);
	}

	template<>
	inline real function<FuncType::Sin>(const real _x)
	{
		return std::sin(_x);
	}

	template<>
	inline real function<FuncType::Tanh>(const real _x)
	{
		return std::tanh(_x);
	}

	/// @brief Non-normalised Gaussian (peaks at 1)
	inline real gaussian(const real _x, const real _mean = 0.0, const real _sd = 1.0)
	{
		return std::exp(-0.5 * std::pow((_x - _mean) / _sd, 2));
	}

	///=====================================
	/// Operations for vectors of elements
	///=====================================

	inline opt<real> min(const std::vector<real>& _v)
	{
		if (_v.empty())
		{
			return nopt;
		}
		return *std::min_element(_v.cbegin(), _v.cend());
	}

	inline opt<real> max(const std::vector<real>& _v)
	{
		if (_v.empty())
		{
			return nopt;
		}
		return *std::max_element(_v.cbegin(), _v.cend());
	}

	template<typename T, OnlyNum<T> ...>
	inline T sum(const std::vector<T>& _v)
	{
		return std::accumulate(_v.cbegin(), _v.cend(), 0.0);
	}

	/// @brief Numerically stable softmax
	inline std::vector<real> softmax(const std::vector<real>& _v)
	{
		std::vector<real> v(0);
		auto max_elem(max(_v));
		auto sum_elem(sum(_v));
		if (!max_elem || !sum_elem)
		{
			return v;
		}
		v.reserve(_v.size());
		real sum_exp(0);
		for (const auto& elem : _v)
		{
			v.push_back(std::exp(elem - max_elem.value()));
			sum_exp += v.back();
		}

		for (auto& elem : v)
		{
			elem /= sum_exp;
		}

		return v;
	}

	///=====================================
	/// degree <-> radian conversion
	///=====================================

	inline real d2r(const real _deg)
	{
		return _deg * Const::pi / 180.0;
	}

	inline real r2d(const real _rad)
	{
		return _rad * 180.0 / Const::pi;
	}

	///=====================================
	/// Other useful functions
	///=====================================

	/// @brief Convert an integer to a floating point number
	template <typename T, OnlyInt<T> ...>
	inline auto flt(T _val)
	{
		return static_cast<real>(_val);
	}

	/// @brief Convert an integer to a floating point number
	template <typename To, typename From, OnlyIf<IsNum<From> && IsNum<To>> ...>
	inline auto as(From&& _val)
	{
		return static_cast<To>(std::forward<From>(_val));
	}

	/// @brief Return the sign of a number as a signed floating point number
	template <typename T, OnlyNum<T> ...>
	inline real sgn(T _val)
	{
		return ( T(0) < _val ) - ( _val < T(0) );
	}
}

#endif // CORTEX_MATH_OPS_HPP
