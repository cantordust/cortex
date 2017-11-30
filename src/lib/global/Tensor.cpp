#include "Tensor.hpp"

namespace Cortex
{


	template<typename T>
	std::ostream& Tensor<T>::operator <<(std::ostream& _strm, const Tensor<T>& _t)
	{
		_strm << "Tensor of size " << _t.idx.x << " x " << _t.idx.y << " x " << _t.idx.z
			  << "\nbuffer size: " << _t.buf.size() << "\n";

		for (size_t z = 0; z < _t.idx.z; ++z)
		{
			_strm << "\nz = " << z << ":\n";
			for (size_t y = 0; y < _t.idx.y; ++y)
			{
				for (size_t x = 0; x < _t.idx.x; ++x)
				{
					_strm << " " << _t(x, y, z);
				}
				_strm << "\n";
			}
		}
		return _strm;
	}

	template<typename T>
	Slice<T> Tensor<T>::xslice(size_t _x)
	{
		Slice<T> s(idx.y, idx.z);
		for (uint z = 0; z < idx.z; ++z)
		{
			for (uint y = 0; y < idx.y; ++y)
			{
				s(y, z) = this->operator()(_x, y, z);
			}
		}

		return s;
	}

	template<typename T>
	Slice<T> Tensor<T>::yslice(size_t _y)
	{
		Slice<T> s(idx.x, idx.z);
		for (uint z = 0; z < idx.z; ++z)
		{
			for (uint x = 0; x < idx.x; ++x)
			{
				s(x, z) = this->operator()(x, _y, z);
			}
		}

		return s;
	}

	template<typename T>
	inline Slice<T> Tensor<T>::zslice(size_t _z)
	{
		Slice<T> s(idx.x, idx.y);
		for (uint y = 0; y < idx.y; ++y)
		{
			for (uint x = 0; x < idx.x; ++x)
			{
				s(x, y) = this->operator()(x, y, _z);
			}
		}

		return s;
	}

}
