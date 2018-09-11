#ifndef CORTEX_MATH_OPS_HPP
#define CORTEX_MATH_OPS_HPP

#include "Types.hpp"
#include "Enum.hpp"

namespace Cortex
{
	///=====================================
	///	Math functions
	///=====================================

	namespace Const
	{
		/// @todo Other constants?
		inline const real phi((1.0 + std::sqrt(5.0)) / 2.0);
		inline const real pi(M_PI);
		inline const real two_pi(2.0 * M_PI);
		inline const real half_pi(0.5 * M_PI);
	}

	template<Func f>
	inline real function(const real _x);

	template<Func f>
	inline real function(const Mat& _m);

	///=====================================
	/// Explicit specialisations
	///=====================================

	template<>
	inline real function<Func::Abs>(const real _x)
	{
		return std::fabs(_x);
	}

	template<>
	inline real function<Func::Cos>(const real _x)
	{
		return std::cos(_x);
	}

	template<>
	inline real function<Func::Gaussian>(const real _x)
	{
		return std::exp(-0.5 * std::pow(_x, 2));
	}

	template<>
	inline real function<Func::Logistic>(const real _x)
	{
		return 0.5 * (std::tanh( 0.5 * _x ) + 1.0);
	}

	/// @brief Differentiable ReLU passing through the origin.
	template<>
	inline real function<Func::ReLU>(const real _x)
	{
		return (0.5 * (std::sqrt(std::pow(_x, 2) + 4) + _x) - 1.0);
	}

	template<>
	inline real function<Func::Sin>(const real _x)
	{
		return std::sin(_x);
	}

	template<>
	inline real function<Func::Tanh>(const real _x)
	{
		return std::tanh(_x);
	}

	/// @brief Logistic regression
	inline Mat Logistic(const Mat& _X)
	{
		return 0.5 * (arma::tanh( 0.5 * _X ) + 1.0);
	}

	/// @brief Non-normalised Gaussian (peaks at 1)
	inline real gaussian(const real _x, const real _mean = 0.0, const real _sd = 1.0)
	{
		return std::exp(-0.5 * std::pow((_x - _mean) / _sd, 2));
	}

	///=====================================
	/// Explicit specialisations for matrices
	///=====================================

	inline real Min(const Mat& _X)
	{
		return _X.empty() ? 0.0 : _X.min() ;
	}

	inline real Max(const Mat& _X)
	{
		return _X.empty() ? 0.0 : _X.max();
	}

	/// @brief Rescale a vector of elements to (0, 1);
	inline Vec Rescale(const Vec& _X)
	{
		real mean(arma::mean(_X));
		real sd(arma::stddev(_X));
		return Logistic((_X - mean) / (sd != 0.0 ? sd
												 : (mean != 0.0 ? mean
																: _X.max())));
	}

	/// @brief Numerically stable softmax
	inline Mat Softmax(const Mat& _X)
	{
		auto X(arma::exp(_X - _X.max()));
		return X / arma::sum(X);
	}

	template<typename T, OnlyNum<T> ...>
	inline T Sum(const std::vector<T>& _X)
	{
		return std::accumulate(_X.begin(), _X.end(), 0.0);
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
