#ifndef CORTEX_WEIGHT_MAP_HPP
#define CORTEX_WEIGHT_MAP_HPP

#include "JSONOps.hpp"
#include "Stat.hpp"
#include "Random.hpp"

namespace Cortex
{
	///=====================================
	///	@struct A container for elements and their corresponding weights.
	/// Used in roulette wheel selection.
	///=====================================
	template<typename T, FuncType f = FuncType::Logistic>
	struct Roulette
	{
		std::vector<T> elements;

	private:

		struct Weights
		{
			bool normalised{false};

			std::vector<real> raw;
			std::vector<real> normal;
			std::vector<real> inverse;

			void normalise()
			{
				normal.clear();
				inverse.clear();

				SMAStat stat;
				for (const auto& entry : raw)
				{
					stat.update(entry);
				}
				for (uint i = 0; i < raw.size(); ++i)
				{
					normal.emplace_back(stat.get_offset<f>(raw[i]));
					inverse.emplace_back(1.0 - normal.back());
				}

				normalised = true;
			}

			void reset()
			{
				raw.clear();
				normal.clear();
				inverse.clear();

				normalised = false;
			}

		} weights;

	public:

		Roulette() {};

		Roulette(const hmap<T, real>& _hmap)
		{
			for (const auto& elem : _hmap)
			{
				elements.emplace_back(elem.first);
				weights.raw.emplace_back(elem.second);
			}
			weights.normalise();
		}

		Roulette(std::initializer_list<std::pair<T, real>> _list)
			:
			  Roulette(hmap<T, real>(_list.begin(), _list.end()))
		{}

		real& operator [] (const T _t)
		{
			weights.normalised = false;

			for (uint pos = 0; pos < elements.size(); ++pos)
			{
				if (elements[pos] == _t)
				{
					return weights.raw[pos];
				}
			}
			elements.emplace_back(_t);
			weights.raw.emplace_back(0.0);

			return weights.raw.back();
		}

		hmap<T, real> to_hmap() const
		{
			hmap<T, real> map;
			for (uint e = 0; e < elements.size(); ++e)
			{
				map[elements[e]] = weights.raw[e];
			}
			return map;
		}

		Roulette& operator = (const hmap<T, real>& _hmap)
		{
			return (*this = Roulette(_hmap));
		}

		Roulette& operator = (std::initializer_list<std::pair<T, real>> _list)
		{
			return (*this = Roulette(_list));
		}

		const Weights& get_weights()
		{
			return weights;
		}

		T spin()
		{
			if (!weights.normalised)
			{
				weights.normalise();
			}
			return elements[roulette(weights.normal)];
		}

		T raw_spin()
		{
			return elements[roulette(weights.raw)];
		}

		T inverse_spin()
		{
			if (!weights.normalised)
			{
				weights.normalise();
			}
			return elements[roulette(weights.inverse)];
		}

		bool is_empty() const
		{
			return elements.empty();
		}

		void reset()
		{
			elements.clear();
			weights.reset();
		}

		friend os& operator << (os& _os, const Roulette& _wmap)
		{
			return _os << _wmap.to_hmap();
		}
	};

} // namespace Cortex

#endif // CORTEX_WEIGHT_MAP_HPP
