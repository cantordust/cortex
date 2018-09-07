#ifndef CORTEX_TRANSFER_OPS_HPP
#define CORTEX_TRANSFER_OPS_HPP

#include "MathOps.hpp"
#include "Enum.hpp"

namespace Cortex
{
	///=============================================================================
	/// Transfer and backprop functions
	///=============================================================================

	using TransferFunc = std::function<Mat(const Mat&)>;
	using BackpropFunc = std::function<Mat(const Mat&, const Mat&)>;

	inline hmap<Func, TransferFunc> transfer;
	inline hmap<Func, BackpropFunc> backprop;

	namespace Transfer
	{
		template<Func f = Func::Undef>
		inline Mat function(const Mat& _in);

		template<Func f = Func::Undef>
		inline Mat derivative(const Mat& _in, const Mat& _out);

		///=====================================
		/// Abs
		///=====================================

		template<>
		inline Mat function<Func::Abs>(const Mat& _in)
		{
			return arma::abs(_in);
		};

		template<>
		inline Mat derivative<Func::Abs>(const Mat& _in, const Mat& _out)
		{
			Mat grad(_out.n_rows, _out.n_cols, arma::fill::ones);
			grad.elem( arma::find(_in) < 0.0 ).fill(-1);
			return grad;
		}

		///=====================================
		/// Cos
		///=====================================

		template<>
		inline Mat function<Func::Cos>(const Mat& _in)
		{
			return arma::cos(_in);
		};

		template<>
		inline Mat derivative<Func::Cos>(const Mat& _in, const Mat& _out)
		{
			return -arma::sin(_in);
		}

		///=====================================
		/// Gaussian
		///=====================================

		template<>
		inline Mat function<Func::Gaussian>(const Mat& _in)
		{
			Mat out(_in);
			out.for_each([&](Mat::elem_type& _elem)
			{
				_elem = gaussian(_elem);
			});
			return out;
		};

		template<>
		inline Mat derivative<Func::Gaussian>(const Mat& _in, const Mat& _out)
		{
			return (- 2 * _in % _out);
		}

		///=====================================
		/// Logistic
		///=====================================

		template<>
		inline Mat function<Func::Logistic>(const Mat& _in)
		{
			return 0.5 * (arma::tanh(0.5 * _in) + 1.0);
		};

		template<>
		inline Mat derivative<Func::Logistic>(const Mat& _in, const Mat& _out)
		{
			return _out % (1.0 - _out);
		}

		///=====================================
		/// ReLU
		///=====================================

		template<>
		inline Mat function<Func::ReLU>(const Mat& _in)
		{
			Mat out(_in);
			out.for_each([&](Mat::elem_type& _elem)
			{
				_elem = Cortex::function<Func::ReLU>(_elem);
			});
			return out;
		};

		template<>
		inline Mat derivative<Func::ReLU>(const Mat& _in, const Mat& _out)
		{
			return (_out + 1.0) / (2 * (_out + 1.0) - _in);
		}

		///=====================================
		/// Sin
		///=====================================

		template<>
		inline Mat function<Func::Sin>(const Mat& _in)
		{
			return arma::sin(_in);
		};

		template<>
		inline Mat derivative<Func::Sin>(const Mat& _in, const Mat& _out)
		{
			return arma::cos(_in);
		}

		///=====================================
		/// Tanh
		///=====================================

		template<>
		inline Mat function<Func::Tanh>(const Mat& _in)
		{
			return arma::tanh(_in);
		};

		template<>
		inline Mat derivative<Func::Tanh>(const Mat& _in, const Mat& _out)
		{
			return (1.0 - arma::square(_out));
		}

		inline void init()
		{
			/// Populate the lookup tables for functions
			/// and their derivatives
			transfer[Func::Abs] = function<Func::Abs>;
			backprop[Func::Abs] = derivative<Func::Abs>;

			transfer[Func::Cos] = function<Func::Cos>;
			backprop[Func::Cos] = derivative<Func::Cos>;

			transfer[Func::Gaussian] = function<Func::Gaussian>;
			backprop[Func::Gaussian] = derivative<Func::Gaussian>;

			transfer[Func::Logistic] = function<Func::Logistic>;
			backprop[Func::Logistic] = derivative<Func::Logistic>;

			transfer[Func::ReLU] = function<Func::ReLU>;
			backprop[Func::ReLU] = derivative<Func::ReLU>;

			transfer[Func::Sin] = function<Func::Sin>;
			backprop[Func::Sin] = derivative<Func::Sin>;

			transfer[Func::Tanh] = function<Func::Tanh>;
			backprop[Func::Tanh] = derivative<Func::Tanh>;
		}
	}
}

#endif // CORTEX_TRANSFER_OPS_HPP
