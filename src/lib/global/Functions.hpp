#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "Globals.hpp"

namespace Cortex
{
	inline real Sum(const std::vector<real>& _input)
	{
		return std::accumulate(_input.begin(), _input.end(), 0.0);
	}

	inline real Tanh(const std::vector<real>& _input)
	{
		return std::tanh( Sum(_input) );
	}

	inline real Sigmoid(const std::vector<real>& _input)
	{
		return 0.5 * (std::tanh( 0.5 * Sum(_input) ) + 1.0);
	}

	inline std::vector<real> Softmax(const std::vector<real>& _input)
	{
		if (_input.size() > 0)
		{
			std::vector<real> ret(_input);
			real max(*std::max_element(_input.begin(), _input.end()));

			real denom(std::accumulate(_input.begin(), _input.end(), 0.0, [&](const real _sum, const real _x)
			{
				return _sum + std::exp(_x - max);
			}));
			for (uint i = 0; i < ret.size(); ++i)
			{
				ret.at(i) = std::exp(ret.at(i) - max) / denom;
			}
			return ret;
		}
		return _input;
	}

	/**
	 * @brief ReLU Differentiable ReLU passing through the origin.
	 * @param _input Vector of input values.
	 * @return Floating-point value.
	 */
	inline real ReLU(const std::vector<real>& _input)
	{
		real x( Sum(_input) );
		return (0.5 * std::sqrt(std::pow(x + 4.0, 2) + x) - 1.0);
	}

	/**
	 * @brief ReLU Differentiable ReLU passing through the origin.
	 * @param _val Input value.
	 * @return Floating-point value.
	 */
	inline real ReLU(const real _val)
	{
		return (0.5 * std::sqrt(std::pow(_val + 4.0, 2) + _val) - 1.0);
	}

	/**
	 * @brief Gaussian Non-normalised Gaussian (peaks at 1) with
	 * a mean of 0 and a standard deviation of 1.
	 * @param _input Vector of input values.
	 * @return Floating-point value.
	 * @todo Add mean and standard deviation as parameters.
	 */
	inline real Gaussian(const std::vector<real>& _input)
	{
		// Mean = 0, SD = 1, non-normalised
		return std::exp(-0.5 * std::pow(Sum(_input), 2));
	}

	/**
	 * @brief Sin Sine function.
	 * @param _input Vector of input values.
	 * @return Floating-point value.
	 */
	inline real Sin(const std::vector<real>& _input)
	{
		return std::sin( Sum(_input) );
	}

	/**
	 * @brief Cos Cosine function.
	 * @param _input Vector of input values.
	 * @return Floating-point value.
	 */
	inline real Cos(const std::vector<real>& _input)
	{
		return std::cos( Sum(_input) );
	}

	inline real Abs(const std::vector<real>& _input)
	{
		return std::fabs( Sum(_input) );
	}

	inline real Min(const std::vector<real>& _input)
	{
		if (_input.size() > 0)
		{
			return *std::min_element( _input.begin(), _input.end() );
		}

		return 0.0;
	}

	inline real Max(const std::vector<real>& _input)
	{
		if (_input.size() > 0)
		{
			return *std::max_element( _input.begin(), _input.end() );
		}

		return 0.0;
	}

	inline real Avg(const std::vector<real>& _input)
	{
		real mean(0.0);

		for (uint arg = 0; arg < _input.size(); ++arg)
		{
			mean += (_input[arg] + mean) / (arg + 1);
		}

		return mean;
	}

	inline real sigmoid(const real _val)
	{
		return 0.5 * (std::tanh(0.5 * _val) + 1.0);
	}

	inline real gaussian(const real _x, const real _mean, const real _sd)
	{
		return std::exp(-0.5 * std::pow((_x - _mean) / _sd, 2));
	}

	// Fast sgn(x)
	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type ... >
	inline real sgn(T val)
	{
		return ( T( 0 ) < val ) - ( val < T( 0 ) );
	}

	inline real rad(const real _deg)
	{
		return _deg * M_PI / 180.0;
	}

	inline real deg(const real _rad)
	{
		return _rad * 180.0 / M_PI;
	}

	template <typename T, typename std::enable_if<std::is_integral<T>::value>::type ... >
	inline real flt(T _val)
	{
		return static_cast<real>(_val);
	}
}

#endif // FUNCTIONS_HPP
