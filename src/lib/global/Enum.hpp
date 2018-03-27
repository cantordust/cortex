#ifndef CORTEX_ENUM_HPP
#define CORTEX_ENUM_HPP

#include "Types.hpp"

namespace Cortex
{
	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	using emap = std::unordered_map<E, std::string>;

	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
	struct Enum
	{
		static std::string name;
		static E undef;
		static emap<E> entries;
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
	std::ostream& operator << (std::ostream& _strm, const E _e)
	{
		return _strm << to_str<E>(_e);
	}

//	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
//	emap<E> Enum<E>::entries;

//	template<typename E, typename std::enable_if<std::is_enum<E>::value>::type ...>
//	E Enum<E>::undef;

	///=========================================
	/// Enum classes
	///=========================================

	/// @brief Optimisation mode
	enum class OptMode : uchar
	{
		Undef,
		Evolution, /// Evolution
		GD, /// Gradient descent
		Hybrid /// Evolution combined with gradient descent
	};

//	template<> emap<OptMode> Enum<OptMode>::entries;
//	template<> OptMode Enum<OptMode>::undef;

	/// @brief Environment type
	enum class EnvType : uchar
	{
		Undef,
		Population, /// Create a population of networks
		Single /// Create a single network
	};

	template<> emap<EnvType> Enum<EnvType>::entries;
	template<> EnvType Enum<EnvType>::undef;

	/// @brief Network type
	enum class NT : uchar
	{
		Undef,
		Classical,
		Spiking
	};

	template<> emap<NT> Enum<NT>::entries;
	template<> NT Enum<NT>::undef;

	/// Node role
	enum class NR : uchar
	{
		Undef,
		B, /// Bias
		I, /// Input
		O, /// Output
		H  /// Hidden
	};

	template<> emap<NR> Enum<NR>::entries;
	template<> NR Enum<NR>::undef;

	/// Link type
	enum class LT : uchar
	{
		Undef,
		Fwd,
		Rec
	};

	template<> emap<LT> Enum<LT>::entries;
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

	template<> emap<TF> Enum<TF>::entries;
	template<> TF Enum<TF>::undef;

	/// Mutation
	/// @todo Migrate to StructMut, ParamMut, etc.
	enum class Mut : uchar
	{
		Undef,
		AddNode, /// Add a node
		EraseNode, /// Erase a node
		AddLink, /// Add a link
		EraseLink, /// Erase a link
		Weight, /// Mutate weight
		Tau, /// Mutate membrane decay constant
		TF, /// Mutate transfer function
		FT /// Mutate firing threshold
	};

	template<> emap<Mut> Enum<Mut>::entries;
	template<> Mut Enum<Mut>::undef;

	/// Parameter mutations
	enum class ParamMut : uchar
	{
		Undef,
		Weight,
		Tau,
		TF,
		FT
	};

	template<> emap<ParamMut> Enum<ParamMut>::entries;
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
	template<> emap<StructMut> Enum<StructMut>::entries;
	template<> StructMut Enum<StructMut>::undef;

	/// Optimisation
	enum class ParamOpt : uchar
	{
		Undef,
		ES, /// Evolutionary strategies (track actions and their effects)
		Anneal /// Simulated annealing
	};

	template<> emap<ParamOpt> Enum<ParamOpt>::entries;
	template<> ParamOpt Enum<ParamOpt>::undef;

	/// The following enums are used for values which can
	/// increase, decrease or stay the same.
	/// These are used to keep track of changes and decide
	/// which changes have been beneficial and which haven't.

	/// Action (used in parameter mutation)
	enum class Act : uchar
	{
		Undef,
		Inc,
		Dec
	};

	template<> emap<Act> Enum<Act>::entries;
	template<> Act Enum<Act>::undef;

	/// Effect of mutation
	enum class Eff : uchar
	{
		Undef,
		Inc,
		Dec
	};

	template<> emap<Eff> Enum<Eff>::entries;
	template<> Eff Enum<Eff>::undef;

	/// Spike encoding
	enum class Enc : uchar
	{
		Undef,
		Direct, /// Relative times
		RankOrder /// Rank order encoding
	};

	template<> emap<Enc> Enum<Enc>::entries;
	template<> Enc Enum<Enc>::undef;

	/// Receptive field
	enum class RF : uchar
	{
		Undef,
		Direct, /// Direct input
		ARF, /// Adaptive
		GRF, /// Gaussian
		ST /// Spatiotemporal
	};

	template<> emap<RF> Enum<RF>::entries;
	template<> RF Enum<RF>::undef;

	/// Search mode
	enum class Search : uchar
	{
		Undef,
		Loss, /// Search based on minimising a loss function
		Fitness, /// Fitness-based search
		Pareto, /// Multiobjective optimisation
		Novelty /// Novelty search
	};

	template<> emap<Search> Enum<Search>::entries;
	template<> Search Enum<Search>::undef;

	/// Helper variable used in topological sorting
	/// @todo: Get rid of this and implement
	/// topological sorting with explicit stack.
	enum class NodeMark : uchar
	{
		Undef,
		Temp, /// Temporary mark (node visited but still visiting children)
		Perm /// Permanent mark (node is a leaf or all children are visited)
	};

	template<> emap<NodeMark> Enum<NodeMark>::entries;
	template<> NodeMark Enum<NodeMark>::undef;

	/// Distribution
	enum class Dist : uchar
	{
		Undef,
		Fixed, /// Fixed value not drawn from a distribution
		Uniform, /// Uniform distribution
		Normal /// Normal distribution
	};

	template<> emap<Dist> Enum<Dist>::entries;
	template<> Dist Enum<Dist>::undef;

	/// Moving average type.
	enum class MA : uchar
	{
		Undef,
		SMA, /// Simple MA
		EMA	 /// Exponential MA
	};

	template<> emap<MA> Enum<MA>::entries;
	template<> MA Enum<MA>::undef;

	/// Data set type
	enum class Set : uchar
	{
		Undef,
		Training,
		Validation,
		Test
	};

	template<> emap<Set> Enum<Set>::entries;
	template<> Set Enum<Set>::undef;

	/// Network layout
	enum class Layout : uchar
	{
		Undef,
		Random,
		Layered
	};

	template<> emap<Layout> Enum<Layout>::entries;
	template<> Layout Enum<Layout>::undef;

	/// Layer type
	enum class LayerType : uchar
	{
		Undef,
		Regular,
		Conv,
		Pool,
		Softmax
	};

	template<> emap<LayerType> Enum<LayerType>::entries;
	template<> LayerType Enum<LayerType>::undef;

	/// Data type
	enum class DataType : uchar
	{
		Undef,
		Real,
		TimeSeries
	};

	template<> emap<DataType> Enum<DataType>::entries;
	template<> DataType Enum<DataType>::undef;

	/// Task type
	enum class Task : uchar
	{
		Undef,
		Classification,
		Regression,
		Prediction,
		Control
	};

	template<> emap<Task> Enum<Task>::entries;
	template<> Task Enum<Task>::undef;
}

#endif // CORTEX_ENUM_HPP
