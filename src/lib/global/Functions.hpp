#ifndef CORTEX_FUNCTIONS_HPP
#define CORTEX_FUNCTIONS_HPP

#include "Types.hpp"

namespace Cortex
{
	/// @brief Converts a string to lowercase.
	void lcase(std::string& _str);

	/// @brief Converts a string to uppercase.
	void ucase(std::string& _str);

	real Logistic(const real _x);

	/// @brief ReLU Differentiable ReLU passing through the origin.
	/// @param _val Input value.
	/// @return Floating-point value.
	real ReLU(const real _x);

	/// @brief Gaussian Non-normalised Gaussian (peaks at 1) with
	/// a mean of 0 and a standard deviation of 1.
	/// @param _x Vector of input values.
	/// @return Floating-point value.
	/// @todo Add mean and standard deviation as parameters.
	real Gaussian(const real _x);

	real gaussian(const real _x, const real _mean, const real _sd);

	real rad(const real _deg);

	real deg(const real _rad);

	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ... >
	T Sum(const std::vector<T>& _X)
	{
		return std::accumulate(_X.begin(), _X.end(), 0.0);
	}

	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ... >
	T Min(const std::vector<T>& _X)
	{
		return ((_X.size() > 0) ?  *std::min_element( _X.begin(), _X.end() ) : 0.0);
	}

	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ... >
	T Max(const std::vector<T>& _X)
	{
		return ((_X.size() > 0) ? *std::max_element( _X.begin(), _X.end() ) : 0.0);
	}

	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ... >
	std::vector<real> Softmax(const std::vector<T>& _X)
	{
		/// Numerically stable softmax
		std::vector<real> ret(_X);

		real max(*std::max_element(_X.begin(), _X.end()));

		real denom(0.0);
		for (const auto& x : _X)
		{
			denom += std::exp(x - max);
		}

		for (uint i = 0; i < ret.size(); ++i)
		{
			ret.at(i) = std::exp(ret.at(i) - max) / denom;
		}
		return ret;
	}


	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ... >
	real Avg(const std::vector<T>& _x)
	{
		real mean(0.0);

		for (uint arg = 0; arg < _x.size(); ++arg)
		{
			mean += (_x[arg] + mean) / (arg + 1);
		}

		return mean;
	}

	/// @brief Convert an integer to a floating point number
	template <typename T, typename std::enable_if<std::is_integral<T>::value>::type ... >
	real flt(T _val)
	{
		return static_cast<real>(_val);
	}

	/// @brief Return the sign of a number as a signed floating point number
	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ... >
	real sgn(T _val)
	{
		return ( T(0) < _val ) - ( _val < T( 0 ) );
	}
}

#endif // CORTEX_FUNCTIONS_HPP
