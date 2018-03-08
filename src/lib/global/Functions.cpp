#include "Functions.hpp"

namespace Cortex
{
	void lcase(std::string& _str)
	{
		std::transform(_str.begin(), _str.end(), _str.begin(), ::tolower);
	}

	void ucase(std::string& _str)
	{
		std::transform(_str.begin(), _str.end(), _str.begin(), ::toupper);
	}

	real Logistic(const real _x)
	{
		return 0.5 * (std::tanh( 0.5 * _x ) + 1.0);
	}

	real ReLU(const real _x)
	{
		return (0.5 * std::sqrt(std::pow(_x + 4.0, 2) + _x) - 1.0);
	}

	real Gaussian(const real _x)
	{
		/// Mean = 0, SD = 1, non-normalised
		return std::exp(-0.5 * std::pow(_x, 2));
	}

	real gaussian(const real _x, const real _mean, const real _sd)
	{
		return std::exp(-0.5 * std::pow((_x - _mean) / _sd, 2));
	}

	real rad(const real _deg)
	{
		return _deg * pi / 180.0;
	}

	real deg(const real _rad)
	{
		return _rad * 180.0 / pi;
	}
}
