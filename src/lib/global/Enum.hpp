#ifndef ENUM_HPP
#define ENUM_HPP

#include <unordered_map>
#include <string>

namespace Cortex
{
	using uint = unsigned int;

	// Network types
	enum class NT : uint
	{
		Undef,
		Classical,
		Spiking
	};

	// Node roles
	enum class NR : uint
	{
		Undef,
		B, // Bias
		I, // Input
		O, // Output
		H  // Hidden
	};

	// Link types
	enum class LT : uint
	{
		Undef,
		F,
		R
	};

	// Function types
	enum class Fn : uint
	{
		Undef,
		Sum,
		Tanh,
		Logistic,
		ReLU,
		Gaussian,
		Sin,
		Cos,
		Abs,
		Min,
		Max,
		Avg,
		Const,
		Golden
	};

	// Mutation types
	enum class Mut : uint
	{
		Undef,
		AddNode,
		EraseNode,
		AddLink,
		EraseLink,
		Weight,
		Tau,
		Fn
	};

	// Optimisation types
	enum class Opt : uint
	{
		Undef,
		Trend,
		Anneal
	};

	// Synaptic plasticity types
	enum class STDP : uint
	{
		Undef,
		Heb,
		AntiHeb
	};

	// The following enums are used for values which can
	// increase, decrease or stay the same.
	// These are used to keep track of changes and decide
	// which changes have been beneficial and which haven't.
	enum class Act : uint
	{
		Undef,
		Inc,
		Dec
	};

	enum class Eff : uint
	{
		Undef,
		Inc,
		Dec
	};

	// Spike encoding method
	enum class Enc : uint
	{
		Undef,
		Time, // Relative times
		RankOrder // Rank order encoding
	};

	// Receptive field types
	enum class RF : uint
	{
		Undef,
		Direct, // No receptive fields
		ARF, // Adaptive
		GRF, // Gaussian
		ST // Spatiotemporal
	};

	// Search mode
	enum class Search : uint
	{
		Undef,
		Fitness,
//		Pareto, // Multiobjective
		Novelty
	};

	// Helper variable used in topological sorting
	enum class NodeMark : uint
	{
		Undef,
		Temp,
		Perm
	};

	// Used in variable initialisation
	enum class InitDist : uint
	{
		Undef,
		Fixed,
		Uniform,
		Normal
	};

	// Moving average type.
	enum class MAType : uint
	{
		Undef,
		Simple, // Exact
		WMA,	// Windowed MA
		EMA		// Exponential MA
	};

	enum class Topology : uint
	{
		Undef,
		Random,
		Layered,
		Conv
	};

	struct EnumHash
	{
		template<typename T>
		inline uint operator() (T _t) const
		{
			return static_cast<uint>(_t);
		}
	};

	// Unordered map and set capable of
	// taking enum class as a key.
	template<typename T1, typename T2> using emap = std::unordered_map<T1, T2, EnumHash>;

	template<typename E> using EnumMap = emap<E, std::string>;

	template<typename E>
	struct Enum
	{
		static E undef;
		static EnumMap<E> entries;
	};

	template<typename E, typename std::enable_if<std::is_enum<E>::value, E>::type ...>
	static std::string to_str( const E _enum )
	{
		if (Enum<E>::entries.find(_enum) != Enum<E>::entries.end())
		{
			return Enum<E>::entries.at(_enum);
		}
		return "undef";
	}

	template<typename E, typename std::enable_if<std::is_enum<E>::value, E>::type ...>
	static inline E to_enum( const std::string& _str )
	{
		for (const auto& entry : Enum<E>::entries)
		{
			if (entry.second == _str)
			{
				return entry.first;
			}
		}

		return Enum<E>::undef;
	}

	template<typename E, typename std::enable_if<std::is_enum<E>::value, E>::type ...>
	std::ostream& operator << (std::ostream& _strm, const E _e)
	{
		return _strm << to_str<E>(_e);
	}

	template<typename T> EnumMap<T> Enum<T>::entries;
	template<typename T> T Enum<T>::undef;

	template<> EnumMap<NT> Enum<NT>::entries;
	template<> NT Enum<NT>::undef;

	template<> EnumMap<NR> Enum<NR>::entries;
	template<> NR Enum<NR>::undef;

	template<> EnumMap<LT> Enum<LT>::entries;
	template<> LT Enum<LT>::undef;

	template<> EnumMap<Fn> Enum<Fn>::entries;
	template<> Fn Enum<Fn>::undef;

	template<> EnumMap<Mut> Enum<Mut>::entries;
	template<> Mut Enum<Mut>::undef;

	template<> EnumMap<Opt> Enum<Opt>::entries;
	template<> Opt Enum<Opt>::undef;

	template<> EnumMap<STDP> Enum<STDP>::entries;
	template<> STDP Enum<STDP>::undef;

	template<> EnumMap<Act> Enum<Act>::entries;
	template<> Act Enum<Act>::undef;

	template<> EnumMap<Eff> Enum<Eff>::entries;
	template<> Eff Enum<Eff>::undef;

	template<> EnumMap<Enc> Enum<Enc>::entries;
	template<> Enc Enum<Enc>::undef;

	template<> EnumMap<RF> Enum<RF>::entries;
	template<> RF Enum<RF>::undef;

	template<> EnumMap<Search> Enum<Search>::entries;
	template<> Search Enum<Search>::undef;

	template<> EnumMap<NodeMark> Enum<NodeMark>::entries;
	template<> NodeMark Enum<NodeMark>::undef;

	template<> EnumMap<InitDist> Enum<InitDist>::entries;
	template<> InitDist Enum<InitDist>::undef;

	template<> EnumMap<MAType> Enum<MAType>::entries;
	template<> MAType Enum<MAType>::undef;

	template<> EnumMap<Topology> Enum<Topology>::entries;
	template<> Topology Enum<Topology>::undef;
}

#endif // ENUM_HPP
