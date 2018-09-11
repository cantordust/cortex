#ifndef CORTEX_GLOBALS_HPP
#define CORTEX_GLOBALS_HPP

#include "Stat.hpp"
#include "Random.hpp"

namespace Cortex
{
	///=============================================================================
	///	Strings providing the title, description, version
	/// and other information about the library.
	///=============================================================================

	inline const std::string title{"=============[ Cortex neuroevolution platform ]============="};

	/// @todo This is clunky.
	/// Automate the version bump process.
	inline const std::string version{"0.5.8"};

	inline const std::string licence{"MIT (https://opensource.org/licence/MIT"};

	/// @todo Add author / contributor information.
	inline const std::string copyright{""};

	///=============================================================================
	///	Debugging
	///=============================================================================

	/// Debug level for debugging with "if constexpr" expressions.
	/// @todo Implementation.
	inline const uint debug(0);

	///=============================================================================
	/// Forward declarations
	///=============================================================================

	///=====================================
	/// Core classes
	///=====================================

	class Task;
	class Env;
	class Species;
	class Genome;
	class Net;
	template<ElemType> class Element;
	using Layer = Element<ElemType::Layer>;
	using Node = Element<ElemType::Node>;
	using Link = Element<ElemType::Link>;
	class Evaluator;
	class RField;
	class Parameter;
	class Fitness;
	class Sample;
	class Data;

	///=====================================
	/// Configuration classes
	///=====================================
	struct Conf;
	struct LayerConf;
	struct ParamConf;

	///=====================================
	/// Pointer and reference aliases
	///=====================================

	/// Reference wrappers
	using SampleRef = rw<Sample>;

	/// Smart pointers
	using LayerPtr = up<Layer>;
	using NodePtr = up<Node>;
	using ConfPtr = up<Conf>;
	using DataPtr = up<Data>;

	/// Networks and species are created in shared pointers.
	using SpeciesPtr = sp<Species>;
	using NetPtr = sp<Net>;

	///=====================================
	///	@struct Dimension information.
	///=====================================
	using Coordinates = hmap<Dim, uint>;

	///=============================================================================
	///	Helper classes.
	///=============================================================================

	///=====================================
	/// @class Optional class.
	/// For now, this is limited to numeric types.
	///=====================================
	inline struct nopt_t : std::false_type {} nopt;

	template<typename Num, OnlyNum<Num>...>
	class opt
	{
	private:

		inline static Num undef{std::numeric_limits<Num>::max()};

		Num value{undef};

	public:

		opt()
			:
			  value(undef)
		{}

		opt(Num _n)
			:
			  value(_n)
		{}

		opt(const nopt_t& _n)
			:
			  value(undef)
		{}

		const Num& operator () () const
		{
			if (value == undef)
			{
				throw std::out_of_range("Attempt to access the value of uninitialised optional value.");
			}
			return value;
		}

		Num operator () ()
		{
			if (value == undef)
			{
				throw std::out_of_range("Attempt to access the value of uninitialised optional value.");
			}
			return value;
		}

		Num operator () (const Num _alt) const noexcept
		{
			return (value != undef) ? value : _alt;
		}

		void reset() const noexcept
		{
			value = undef;
		}

		bool is_set () const noexcept
		{
			return value != undef;
		}

		void operator = (const Num& _num) noexcept
		{
			value = _num;
		}

		void operator = (const nopt_t& _nopt) noexcept
		{
			value = undef;
		}

		bool operator == (const opt<Num> _other) const noexcept
		{
			return value != undef && _other.value != undef && value == _other.value;
		}

		bool operator != (const opt<Num> _other) const noexcept
		{
			return !(value == _other);
		}

		friend os& operator << (os& _os, const opt<Num> _num)
		{
			return _os << (_num.is_set() ? std::to_string(_num.value) : "unset");
		}
	};

	/// @brief Load an optional value from JSON
	template<typename T>
	inline void from_json(const json& _j, opt<T>& _opt)
	{
		_opt = (_j.is_null() ? nopt : _j.get<T>());
	}

	/// @brief Store an optional value to JSON
	template<typename T>
	inline void to_json(json& _j, const opt<T>& _opt)
	{
		_j = (!_opt.is_set() ? json::value_t::null : _opt());
	}

	///=====================================
	///	@struct A container for elements and their corresponding weights.
	/// Used in roulette wheel selection.
	///=====================================
	template<typename T, Func f = Func::Logistic>
	struct wmap
	{
		std::vector<T> elements;

	private:

		struct Weights
		{
			bool normalised{false};

			std::vector<real> raw;
			std::vector<real> normal;
			std::vector<real> flipped;

			void normalise()
			{
				normal.clear();
				flipped.clear();

				SMAStat stat;
				for (const auto& entry : raw)
				{
					stat.update(entry);
				}
				for (uint i = 0; i < raw.size(); ++i)
				{
					normal.emplace_back(stat.get_offset<f>(raw[i]));
					flipped.emplace_back(1.0 - normal.back());
				}

				normalised = true;
			}

			void reset()
			{
				raw.clear();
				normal.clear();
				flipped.clear();

				normalised = false;
			}

		} weights;

	public:

		wmap() {};

		wmap(const hmap<T, real>& _hmap)
		{
			for (const auto& elem : _hmap)
			{
				elements.emplace_back(elem.first);
				weights.raw.emplace_back(elem.second);
			}
			weights.normalise();
		}

		wmap(std::initializer_list<std::pair<T, real>> _list)
			:
			  wmap(hmap<T, real>(_list.begin(), _list.end()))
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

		wmap& operator = (const hmap<T, real>& _hmap)
		{
			return (*this = wmap(_hmap));
		}

		wmap& operator = (std::initializer_list<std::pair<T, real>> _list)
		{
			return (*this = wmap(_list));
		}

		T spin()
		{
			if (!weights.normalised)
			{
				weights.normalise();
			}
			return elements[roulette(weights.normal)];
		}

		T flip_spin()
		{
			if (!weights.normalised)
			{
				weights.normalise();
			}
			return elements[roulette(weights.flipped)];
		}

		T raw_spin()
		{
			return elements[roulette(weights.raw)];
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

		friend os& operator << (os& _os, const wmap& _wmap)
		{
			return _os << _wmap.to_hmap();
		}
	};

	/// @brief Load a weighted map from JSON
	template<typename T>
	inline void from_json(const json& _j, wmap<T>& _wmap)
	{
		_wmap = _j.get<hmap<T, real>>();
	}

	/// @brief Store a weighted map to JSON
	template<typename T>
	inline void to_json(json& _j, const wmap<T>& _wmap)
	{
		_j = _wmap.to_hmap();
	}

	///=====================================
	///	@struct A mutation event.
	///=====================================
	struct Mutation
	{
		ElemType element{ElemType::Undef};
		Action action{Action::Undef};
		struct
		{
			LayerType type{LayerType::Undef};
			opt<uint> index{nopt};
		} layer;

		explicit operator bool() noexcept
		{
			return (element != ElemType::Undef &&
					action != Action::Undef &&
					layer.type != LayerType::Undef &&
					layer.index.is_set());
		}

		friend os& operator << (os& _os, const Mutation& _mut)
		{
			return _os << "\nElement: " << _mut.element
					   << "\nAction: " << _mut.action
					   << "\nLayer type: " << _mut.layer.type
					   << "\nLayer index: " << _mut.layer.index
					   << "\n";
		}
	};

	///=====================================
	///	@struct A spike event.
	///=====================================
	struct Spike
	{
		/// Alias for a queue of spike events
		using Scheduler = std::priority_queue<Spike, std::vector<Spike>>;

		Node* node{nullptr};

		Spike(Node* _node)
			:
			  node(_node)
		{}

		friend bool operator < (const Spike& _lhs, const Spike& _rhs) noexcept;
	};

	///=====================================
	/// @struct Node ID
	/// (layer index + node index).
	///=====================================
	struct NodeID
	{
		opt<uint> layer{nopt};
		opt<uint> node{nopt};

		NodeID(const opt<uint> _layer = nopt, const opt<uint> _node = nopt)
			:
			  layer(_layer),
			  node(_node)
		{}

		bool operator == (const NodeID& _other) const noexcept
		{
			return (layer == _other.layer &&
					node == _other.node);
		}

		bool operator != (const NodeID& _other) const noexcept
		{
			return !(*this == _other);
		}

		friend os& operator << (os& _os, const NodeID& _id)
		{
			return _os << "[L " << _id.layer << ", N " << _id.node << "]";
		}
	};

	///=====================================
	/// @class Archive of values and
	/// statistics for a number of runs.
	///=====================================
	class History
	{
	private:

		bool closed{false};

		/// A record for a single run.
		struct Archive
		{
			/// Statistics for a single run.
			SMAStat stats;

			/// Archive of individual values
			/// obtained throughout the run.
			std::vector<uint> archive;

			void add(const uint _val)
			{
				archive.emplace_back(_val);
				stats.update(_val);
			}
		};

		/// Global statistics
		hmap<Stat, SMAStat> stats;

		hmap<uint, hmap<Stat, Archive>> runs;

	public:

		~History()
		{
			if (!closed)
			{
				close();
			}
			print();
		}

		/// @brief Insert an empty record for a new run.
		void new_run()
		{
			if (!closed)
			{
				runs[runs.size() + 1] = {};
			}
		}

		/// @brief Add a new entry to the record for @p _stat
		void add(const Stat _stat, const uint _val)
		{
			if (!closed)
			{
				runs[runs.size()][_stat].add(_val);
			}
		}

		/// @brief Compute the global statistics and finalise.
		void close()
		{
			for(const auto& run : runs)
			{
				for (const auto& stat : run.second)
				{
					stats[stat.first].update(stat.second.archive.back());
				}
			}
			closed = true;
		}

		/// @brief Get a reference to the statistics.
		const auto& get()
		{
			return runs;
		}

		/// @brief Tabulate statistics.
		void print()
		{
			/// Stats report
			dlog report;

			std::string header("--------------------[ Task statistics ("
							   + std::to_string(runs.size())
							   + " run"
							   + (runs.size() == 1 ? "" : "s")
							   + ") ]--------------------");
			std::vector<std::string> fields{"Variable", "Mean", "SD"};
			std::vector<uint> width;
			uint total(std::accumulate(fields.cbegin(),
									   fields.cend(),
									   0,
									   [](const uint _sum, const std::string& _field)
			{
				return _sum + _field.size();
			}));

			/// Compute the field sizes.
			for (const auto& f : fields)
			{
				width.emplace_back((f.size() / flt(total)) * header.size() - 2);
			}

			/// Header
			report + "\n" + header + "\n";

			/// Justify left and set the filler to ' '.
			report.left().setfill(' ');

			/// Print stat fields
			for (uint f = 0; f < fields.size(); ++f)
			{
				report.add(" ").format(fields[f], width[f]).add((f < fields.size() - 1 ? "|" : ""));
			}
			report + "\n" + std::string(header.size(), '-');

			for (const auto& stat : stats)
			{
				uint idx(0);
				report.add("\n ").format(pretty(stat.first), width[idx]);
				report.add("| ").format(stat.second.mean, width[++idx]);
				report.add("| ").format(stat.second.sd(), width[++idx]);
			}

			/// Bottom line
			report + "\n" + std::string(header.size(), '-') + "\n";
		}
	};

	///=============================================================================
	///	Exception handling.
	///=============================================================================

	/// @todo Destroy global instances after an unhandled exception.
	inline void destroy()
	{
//		conf.reset(nullptr);
//		data.reset(nullptr);
	}

	/// All exceptions should be funnelled through this function.
	inline void pitch(const Error _error, std::string&& _msg)
	{
		/// Destroy static variables
		destroy();
	}
}

namespace std
{
	template<typename T>
	class numeric_limits<Cortex::opt<T>>
	{
	public:
		inline T min() const noexcept
		{
			return std::is_unsigned<T>::value ? std::numeric_limits<T>::min()
											  : std::numeric_limits<T>::min() + 1;
		};

		inline T max() const noexcept
		{
			return (std::is_arithmetic<T>::value ? std::numeric_limits<T>::max() - 1
												 : std::numeric_limits<T>::max());
		};
	};
}

#endif // CORTEX_GLOBALS_HPP
