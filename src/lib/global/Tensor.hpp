#ifndef TENSOR_HPP
#define TENSOR_HPP

#include "Config.hpp"

namespace Cortex
{
	template<typename T> class Row;
	template<typename T> class Slice;

	template<typename T = real>
	class Tensor
	{
	private:

		struct
		{
			size_t x = 0;
			size_t y = 0;
			size_t z = 0;
		} idx;

		size_t xy;
		std::vector<T> buf;
	public:

		Tensor(const size_t _x,
			   const size_t _y = 0,
			   const size_t _z = 0)
			:
			  idx({_x, (_y == 0 ? 1 : _y), (_z == 0 ? 1 : _z)}),
			  xy(_x * (_y == 0 ? 1 : _y)),
			  buf(_x * (_y == 0 ? 1 : _y) * (_z == 0 ? 1 : _z), 0.0)
		{
			if (_x == 0)
			{
				std::cout << "Invalid tensor size (0)\n";
				exit(EXIT_FAILURE);
			}
		}

		T& operator()(std::size_t _x, std::size_t _y, std::size_t _z)
		{
			return buf.at(_z * xy + _y * idx.x + _x);
		}

		const T& operator()(std::size_t _x, std::size_t _y, std::size_t _z) const
		{
			return buf.at(_z * xy + _y * idx.x + _x);
		}

		inline const std::vector<T>& buffer() const
		{
			return buf;
		}

		inline size_t x() const
		{
			return idx.x;
		}

		inline size_t y() const
		{
			return idx.y;
		}

		inline size_t z() const
		{
			return idx.z;
		}

		Slice<T> xslice(size_t _x);

		Slice<T> yslice(size_t _y);

		Slice<T> zslice(size_t _z);

		Row<T> xyrow(size_t _x, size_t _y);

		Row<T> xzrow(size_t _x, size_t _z);

		Row<T> yzrow(size_t _y, size_t _z);

		friend std::ostream& operator << (std::ostream& _strm, const Tensor& _t);
	};

	template<typename T = real>
	class Slice : public Tensor<T>
	{
	public:

		Slice(const size_t _x,
			  const size_t _y = 0)
			:
			  Tensor<T>(_x, _y, 0)
		{}
	};

	template<typename T = real>
	class Row : public Tensor<T>
	{
	public:
		Row(const size_t _x)
			:
			  Tensor<T>(_x, 0, 0)
		{}
	};
}

#endif // TENSOR_HPP
