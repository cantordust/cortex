#ifndef CORTEX_TENSOR_HPP
#define CORTEX_TENSOR_HPP

#include "JSONOps.hpp"

namespace Cortex
{
	///=====================================
	/// Dimension struct contextually
	/// convertible to a size type.
	///=====================================

	template<sz>
	struct Dim
	{
		const sz dim{0};

		constexpr Dim(const sz _dim = 0)
			:
			  dim(_dim)
		{}

		inline constexpr operator sz ()
		{
			return dim;
		}
	};

	template<sz Size>
	using Array = std::array<sz, Size>;

	///=============================================================================
	///	Virtual tensor.
	/// This provides only the dimensions and the corresponding strides
	/// and cannot hold any elements.
	/// To store elements in a tensor, use the Tensor class defined after VTensor.
	///=============================================================================

	template<sz Rank, class = std::make_index_sequence<Rank>>
	class VTensor;

	template<sz Rank, sz ... Axes>
	class VTensor<Rank, std::index_sequence<Axes...>> : public Serialiser
	{
	protected:

		Array<Rank> dim{};

		Array<Rank + 1> strides{};

	public:

		constexpr VTensor(const Array<Rank>& _dim = {})
		{
			set_dim(_dim);
		}

		constexpr VTensor(const VTensor& _other)
			:
			  VTensor(_other.dim)
		{}

		constexpr VTensor(VTensor&& _other)
			:
			  VTensor(std::move(_other.dim))
		{}

		constexpr VTensor(Dim<Axes> ... _dim)
			:
			  VTensor(Array<Rank>{_dim...})
		{}

		/// @brief Get the dimension at index @p _dim_index.
		/// @note No range check (cf. at()).
		inline constexpr sz operator[] (const sz _dim_index) const
		{
			return dim[_dim_index];
		}

		/// @brief Get the dimension at offset @p _offset.
		/// @note Range-checked version of operator[].
		inline constexpr sz at(const sz _offset) const
		{
			if (_offset >= dim.back())
			{
				throw std::out_of_range("Invalid dimension.");
			}
			return dim[_offset];
		}

		/// @brief Range check for an offset.
		constexpr bool is_valid(const sz _offset) const
		{
			return _offset < strides.back();
		}

		/// @brief Explicit range check for an array of indices.
		constexpr bool is_valid(const Array<Rank>& _index) const
		{
			for (sz i = 0; i < _index.size(); ++i)
			{
				if (_index[i] >= dim[i])
				{
					return false;
				}
			}
			return true;
		}

		/// @brief Explicit range check for raw indices.
		constexpr bool is_valid(Dim<Axes> ... _dim) const
		{
			return is_valid(Array<Rank>{_dim...});
		}

		/// @brief Range check for another VTensor object.
		constexpr bool is_valid(const VTensor<Rank>& _other) const
		{
			return is_valid(_other.dim);
		}

		/// @brief Retrieve the total number of
		/// elements that the tensor can hold.
		constexpr sz get_size()
		{
			return strides.back();
		}

		/// @brief Get an index array from an offset.
		constexpr VTensor<Rank> get_index(sz _offset) const
		{
			VTensor<Rank> vt;
			for (sz d = Rank; d-- > 0;)
			{
				vt.dim[d] = _offset / strides[d];
				_offset %= strides[d];
			}
			return vt;
		}

		/// @brief Get offset from an array of dimensions.
		constexpr sz get_offset(const Array<Rank>& _dim) const
		{
			sz dim(0);
			return std::accumulate(_dim.cbegin(),
								   _dim.cend(),
								   0,
								   [&](const sz _sum, const sz _idx)
			{
				return _sum + _idx * strides[dim++];
			});
		}

		/// @brief Get offset from raw indices.
		constexpr sz get_offset(Dim<Axes> ... _dim) const
		{
			return get_offset(Array<Rank>{_dim...});
		}

		/// @brief Get offset from another VTensor object.
		constexpr sz get_offset(const VTensor& _vt) const
		{
			return get_offset(_vt.dim);
		}

		/// @brief Set the dimensions and compute the offsets.
		/// @note The last element of the strides array
		/// can be used to check the capacity of the tensor.
		constexpr void set_dim(const Array<Rank>& _dim)
		{
			dim = _dim;

			strides.back() = 1;
			for (sz d = 0; d < Rank; ++d)
			{
				if (dim[d] == 0)
				{
					dim[d] = 1;
				}
				strides[d] = strides.back();
				strides.back() *= dim[d];
			}
		}

		/// @brief Resize using raw dimensions.
		constexpr void set_dim(Dim<Axes> ... _dim)
		{
			set_dim(Array<Rank>{_dim...});
		}

		/// @brief Copy assignment operator.
		constexpr VTensor& operator = (const VTensor& _other)
		{
			set_dim(_other.dim);
			return *this;
		}

		/// @brief Move assignment operator.
		constexpr VTensor& operator = (VTensor&& _other)
		{
			set_dim(std::move(_other.dim));
			return *this;
		}

		/// @brief Deserialise from JSON.
		virtual void load(const json& _j) override
		{
			dim = _j;
		}

		/// @brief Serialise to JSON.
		virtual void save(json& _j)	const override
		{
			_j = dim;
		}

		friend constexpr bool operator == (const VTensor& _lhs, const VTensor& _rhs)
		{
			if (_lhs.dim.size() != _rhs.dim.size())
			{
				return false;
			}
			for (sz s = 0; s < _lhs.dim.size(); ++s)
			{
				if (_lhs.dim[s] != _rhs.dim[s])
				{
					return false;
				}
			}
			return true;
		}

		friend constexpr bool operator != (const VTensor& _lhs, const VTensor& _rhs)
		{
			return !(_lhs == _rhs);
		}

		/// @brief Print to stream.
		friend std::ostream& operator << (std::ostream& _os, const VTensor& _vt)
		{
			_os << "(";
			for (sz s = 0; s < _vt.dim.size(); ++s)
			{
				_os << _vt[s] << (s == _vt.dim.size() - 1 ? "" : ", ");
			}
			return _os << ")";
		}
	};

	///=============================================================================
	///	Main feature.
	/// Unlike VTensor, this class is an actual container that can hold elements.
	///=============================================================================

	template<typename T, sz Rank, class = std::make_index_sequence<Rank>>
	class Tensor;

	template<typename T, sz Rank, sz ... Axes>
	class Tensor<T, Rank, std::index_sequence<Axes...>> : public VTensor<Rank, std::index_sequence<Axes...>>
	{
	protected:

		std::vector<T> elements;

	public:

		constexpr Tensor(const Array<Rank>& _dim)
			:
			  VTensor<Rank, std::index_sequence<Axes...> > (_dim)
		{
			reserve();
		}

		constexpr Tensor(const VTensor<Rank>& _vt)
			:
			  Tensor(_vt.dim)
		{}

		constexpr Tensor(Dim<Axes> ... _dim)
			:
			  Tensor(Array<Rank>{_dim...})
		{}

		/// @brief Populate the tensor with elements
		/// by using a custom callable type.
		template<typename F, typename ... Args>
		constexpr void initialise(F&& _f, Args&& ... _args)
		{
			/// Reserve memory
			reserve();

			uint e(0);
			/// Replace existing elements
			while (e < elements.size())
			{
				elements[e] = _f(std::forward<Args>(_args)...);
				++e;
			}

			/// Fill up the remaining elements
			while (e < this->strides.back())
			{
				elements.emplace_back(_f(std::forward<Args>(_args)...));
				++e;
			}
		}

		constexpr void initialise(const std::vector<T>& _elements)
		{
			if (_elements.size() > this->strides.back())
			{
				throw std::out_of_range("[initialise] Initialisation failed: tensor capacity exceeded.");
			}

			/// Reserve memory
			reserve(_elements.size());

			uint e(0);
			/// Replace existing elements
			while (e < elements.size() &&
				   e < _elements.size())
			{
				elements[e] = _elements[e];
				++e;
			}

			/// Fill up the remaining elements
			while (e < _elements.size())
			{
				elements.emplace_back(_elements[e]);
				++e;
			}

			if (e < elements.size() - 1)
			{
				elements.erase(elements.begin() + e, elements.end());
			}
		}

		constexpr void initialise(std::initializer_list<T> _elements)
		{
			initialise(std::vector<T>(_elements));
		}

		template<typename ... Type>
		constexpr void initialise(Type&& ... _elements)
		{
			initialise({std::forward<Type>(_elements)...});
		}

		/// @brief Retrieve an element from an array of indices.
		/// @warning No range check.
		constexpr T& operator ()(const Array<Rank>& _dim)
		{
			return elements[this->get_offset(_dim)];
		}

		/// @brief Retrieve an element from an offset.
		/// @warning No range check.
		constexpr T& operator [](const sz _offset)
		{
			return elements[_offset];
		}

		/// @brief Retrieve an element from raw indices.
		/// @warning No range check.
		constexpr T& operator ()(Dim<Axes> ... _dim)
		{
			return operator()(Array<Rank>{_dim...});
		}

		/// @brief Retrieve an element from a VTensor object.
		/// @warning No range check.
		constexpr T& operator ()(const VTensor<Rank>& _vt)
		{
			return operator[](this->get_offset(_vt.dim));
		}

		/// @brief Retrieve an element from an array of indices.
		/// @note Range-checked version of operator().
		constexpr T& at(const Array<Rank>& _dim)
		{
			if (!is_valid(_dim))
			{
				throw std::out_of_range("Invalid dimensions.");
			}
			return elements[get_offset(_dim)];
		}

		/// @brief Retrieve an element from an offset.
		/// @note Range-checked version of operator[].
		constexpr T& at(const sz _offset)
		{
			return at(this->get_index(_offset).dim);
		}

		/// @brief Retrieve an element from raw indices.
		/// @note Range-checked version of operator().
		constexpr T& at(Dim<Axes> ... _dim)
		{
			return at(Array<Rank>{_dim...});
		}

		/// @brief Retrieve an element from a VTensor object.
		/// @note Range-checked version of operator().
		constexpr T& at(const VTensor<Rank>& _vt)
		{
			return at(_vt.dim);
		}

		/// @brief Retrieve the index of an element.
		/// It returns an invalid index if the
		/// element is not found.
		/// The optional comparator is useful, for example,
		/// when the stored elements are smart pointers
		/// but @p _elem is a raw pointer.
		template<typename Type, typename F>
		constexpr opt<VTensor<Rank>> index_of(Type&& _elem,
											  F&& _comp = [](const T& _e1, const Type& _e2) { return _e1 == _e2; }) const
		{
			for (auto it = elements.begin(); it != elements.end(); ++it)
			{
				if (_comp(*it, _elem))
				{
					return VTensor{this->get_index(it - elements.begin())};
				}
			}

			return nopt;
		}

		/// @brief Retrieve the offset of an element.
		/// It returns an invalid offset if the
		/// element is not found.
		/// The optional comparator is useful, for example,
		/// when the stored elements are smart pointers
		/// but @p _elem is a raw pointer.
		template<typename Type, typename F>
		constexpr opt<sz> offset_of(Type&& _elem,
									F&& _comp = [](const T& _e1, const Type& _e2) { return _e1 == _e2; }) const
		{
			opt<sz> offset;

			for (sz i = 0; i < elements.size(); ++i)
			{
				if (_comp(elements[i], _elem))
				{
					offset = i;
					break;
				}
			}

			return offset;
		}

		/// @brief Resizes the tensor using an array of dimensions.
		/// @warning Destroys the stored elements and resizes
		/// the storage based on the new dimensions.
		constexpr void resize(const Array<Rank>& _dim)
		{
			this->set_dim(_dim);

			elements.clear();
			elements.resize(this->strides.back());
		}

		/// @brief Resize using raw dimensions.
		/// @warning Destroys the stored elements.
		constexpr void resize(Dim<Axes> ... _dim)
		{
			resize(Array<Rank>{_dim...});
		}

		/// @brief Resize using a TensorDim object.
		/// @warning Destroys the stored elements.
		constexpr void resize(const VTensor<Rank>& _vt)
		{
			resize(_vt.dim);
		}

		/// @brief Apply a function to all elements
		template<typename Func, typename ... Args>
		void apply(Func&& _f, Args&& ... _args)
		{
			for (auto& elem : elements)
			{
				std::forward<Func>(_f)(elem, std::forward<Args>(_args)...);
			}
		}

		/// @brief Copy assignment operator.
		constexpr Tensor<T, Rank>& operator = (const Tensor<T, Rank>& _other)
		{
			resize(_other.dim);
			return *this;
		}

		/// @brief Move assignment operator.
		constexpr Tensor<T, Rank>& operator = (Tensor<T, Rank>&& _other)
		{
			resize(std::move(_other.dim));
			return *this;
		}

	private:

		constexpr void reserve(const opt<sz> _size = nopt)
		{
			if (_size &&
				elements.capacity() < _size.value())
			{
				elements.reserve(_size.value());
			}

			if (elements.capacity() < this->strides.back())
			{
				elements.reserve(this->strides.back());
			}
		}
	};

	///=============================================================================
	/// Specialisations for low-rank tensors
	/// rank 0: Scalar
	/// rank 2: Matrix
	/// rank 3: Cube
	/// rank 4: Field
	///=============================================================================

	///=====================================
	/// Specialisation for tensors
	/// of rank 0 (scalars).
	/// @todo Implementation.
	///=====================================
	template<typename T>
	class Tensor<T, 0>
	{
		T _t;
	};

	template<typename T>
	using Scalar = Tensor<T, 0>;

	///=====================================
	/// Matrix class (tensor of rank 2).
	///=====================================

	struct VMatrix : public VTensor<2>
	{
		using VTensor<2>::VTensor;
		using VTensor<2>::operator=;
		using VTensor<2>::operator[];

		const sz rows()
		{
			return dim[0];
		}

		const sz cols()
		{
			return dim[1];
		}
	};

	template<typename T>
	using Matrix = Tensor<T, 2>;

	///=====================================
	/// Cube class (tensor of rank 3).
	///=====================================

	struct VCube : public VTensor<3>
	{
		using VTensor<3>::VTensor;
		using VTensor<3>::operator=;
		using VTensor<3>::operator[];

		const sz depth() const
		{
			return dim[0];
		}

		const sz height() const
		{
			return dim[1];
		}

		const sz width() const
		{
			return dim[2];
		}
	};

	template<typename T>
	using Cube = Tensor<T, 3>;

	///=====================================
	/// Field class (tensor of rank 4).
	///=====================================

	struct VField : public VTensor<4>
	{
		using VTensor<4>::VTensor;
		using VTensor<4>::operator=;
		using VTensor<4>::operator[];

		const sz span() const
		{
			return dim[0];
		}

		const sz depth() const
		{
			return dim[1];
		}

		const sz height() const
		{
			return dim[2];
		}

		const sz width() const
		{
			return dim[3];
		}
	};

	template<typename T>
	using Field = Tensor<T, 4>;
}

#endif // CORTEX_TENSOR_HPP
