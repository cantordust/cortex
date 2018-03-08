#ifndef CORTEX_ENUM_HPP
#define CORTEX_ENUM_HPP

#include "Types.hpp"

namespace Cortex
{
	///=========================================
	/// Enum helper structs
	///=========================================

	struct EHash
	{
		template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ... >
		auto operator() (const E _e) const
		{
			return static_cast<typename std::underlying_type<E>::type>(_e);
		}
	};

	/// Unordered map and set capable of
	/// taking enum class as a key.
	template<typename E, typename V, typename std::enable_if<std::is_enum<E>::value>::type ...>
	using emap = std::unordered_map<E, V, EHash>;

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	using eset = std::unordered_set<E, EHash>;

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	using EMap = std::vector<std::pair<E, std::string>>;

	template<typename E>
	struct Enum
	{
		static std::string name;
		static E undef;
		static EMap<E> entries;
	};

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	static std::string to_str( const E _enum )
	{
		for (auto it = Enum<E>::entries.begin(); it != Enum<E>::entries.end(); ++it)
		{
			if (it->first == _enum)
			{
				return it->second;
			}
		}
		return "undef";
	}

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	static E to_enum( const std::string& _str )
	{
		for (auto it = Enum<E>::entries.begin(); it != Enum<E>::entries.end(); ++it)
		{
			if (it->second == _str)
			{
				return it->first;
			}
		}
		return Enum<E>::undef;
	}

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	std::ostream& operator << (std::ostream& _strm, const E& _e)
	{
		return _strm << to_str<E>(_e);
	}

	template<typename E>
	EMap<E> Enum<E>::entries;

	template<typename E>
	E Enum<E>::undef;

	///=========================================
	/// Enum classes
	///=========================================

	/// Network type
	enum class NT : uchar
	{
		Undef,
		Classical,
		Spiking
	};

	template<> EMap<NT> Enum<NT>::entries;
	template<> NT Enum<NT>::undef;

	/// Node role
	enum class NR : uchar
	{
		Undef,
		B, // Bias
		I, // Input
		O, // Output
		H  // Hidden
	};

	template<> EMap<NR> Enum<NR>::entries;
	template<> NR Enum<NR>::undef;

	/// Link type
	enum class LT : uchar
	{
		Undef,
		Fwd,
		Rec
	};

	template<> EMap<LT> Enum<LT>::entries;
	template<> LT Enum<LT>::undef;

	/// Transfer function
	enum class TF : uchar
	{
		Undef,
		Abs,
		Avg,
		Const,
		Cos,
		Gaussian,
		Golden,
		Logistic,
		Max,
		Min,
		ReLU,
		Sin,
		Spiking,
		Sum,
		Tanh
	};

	template<> EMap<TF> Enum<TF>::entries;
	template<> TF Enum<TF>::undef;

	/// Mutation
	/// @todo Migrate to StructMut, ParamMut, etc.
	enum class Mut : uchar
	{
		Undef,
		AddNode,
		EraseNode,
		AddLink,
		EraseLink,
		Weight,
		Tau,
		TF
	};

	template<> EMap<Mut> Enum<Mut>::entries;
	template<> Mut Enum<Mut>::undef;

	/// Parameter Mutation
	enum class ParamMut : uchar
	{
		Undef,
		Weight,
		Tau,
		TF
	};

	template<> EMap<ParamMut> Enum<ParamMut>::entries;
	template<> ParamMut Enum<ParamMut>::undef;

	/// Structeter Mutation
	enum class StructMut : uchar
	{
		Undef,
		AddNode,
		EraseNode,
		AddLink,
		EraseLink
	};
	template<> EMap<StructMut> Enum<StructMut>::entries;
	template<> StructMut Enum<StructMut>::undef;

	/// Optimisation
	enum class Opt : uchar
	{
		Undef,
		Trend,
		Anneal
	};

	template<> EMap<Opt> Enum<Opt>::entries;
	template<> Opt Enum<Opt>::undef;

	/// The following enums are used for values which can
	/// increase, decrease or stay the same.
	/// These are used to keep track of changes and decide
	/// which changes have been beneficial and which haven't.

	/// Action
	enum class Act : uchar
	{
		Undef,
		Inc,
		Dec
	};

	template<> EMap<Act> Enum<Act>::entries;
	template<> Act Enum<Act>::undef;

	/// Effect
	enum class Eff : uchar
	{
		Undef,
		Inc,
		Dec
	};

	template<> EMap<Eff> Enum<Eff>::entries;
	template<> Eff Enum<Eff>::undef;

	/// Spike Encoding
	enum class Enc : uchar
	{
		Undef,
		Direct, // Relative times
		RankOrder // Rank order encoding
	};

	template<> EMap<Enc> Enum<Enc>::entries;
	template<> Enc Enum<Enc>::undef;

	/// Receptive Field
	enum class RF : uchar
	{
		Undef,
		None, // Raw spike times (no conversion).
		ARF, // Adaptive
		GRF, // Gaussian
		ST // Spatiotemporal
	};

	template<> EMap<RF> Enum<RF>::entries;
	template<> RF Enum<RF>::undef;

	/// Search mode
	enum class Search : uchar
	{
		Undef,
		Fitness,
//		Pareto, // Multiobjective
		Novelty
	};

	template<> EMap<Search> Enum<Search>::entries;
	template<> Search Enum<Search>::undef;

	/// Helper variable used in topological sorting
	/// @todo: Get rid of this and implement
	/// topological sorting with explicit stack.
	enum class NodeMark : uchar
	{
		Undef,
		Temp,
		Perm
	};

	template<> EMap<NodeMark> Enum<NodeMark>::entries;
	template<> NodeMark Enum<NodeMark>::undef;

	/// Distribution
	enum class Dist : uchar
	{
		Undef,
		Fixed,
		Uniform,
		Normal
	};

	template<> EMap<Dist> Enum<Dist>::entries;
	template<> Dist Enum<Dist>::undef;

	/// Moving average type.
	enum class MA : uchar
	{
		Undef,
		SMA, // Simple MA
		EMA	 // Exponential MA
	};

	template<> EMap<MA> Enum<MA>::entries;
	template<> MA Enum<MA>::undef;

	/// Data set type
	enum class Set : uchar
	{
		Undef,
		Training,
		Validation,
		Test
	};

	template<> EMap<Set> Enum<Set>::entries;
	template<> Set Enum<Set>::undef;

	/// Network layout
	enum class Layout : uchar
	{
		Undef,
		Random,
		Layered,
		Conv
	};

	template<> EMap<Layout> Enum<Layout>::entries;
	template<> Layout Enum<Layout>::undef;

	/// Task type
	enum class Task : uchar
	{
		Undef,
		Classification,
		Regression,
		Prediction,
		Control
	};

	template<> EMap<Task> Enum<Task>::entries;
	template<> Task Enum<Task>::undef;

	/// Data type
	enum class DataType : uchar
	{
		Undef,
		Real,
		TimeSeries
	};

	template<> EMap<DataType> Enum<DataType>::entries;
	template<> DataType Enum<DataType>::undef;
}

#endif // CORTEX_ENUM_HPP
