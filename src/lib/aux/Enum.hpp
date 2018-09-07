#ifndef CORTEX_ENUM_HPP
#define CORTEX_ENUM_HPP

#include "StringOps.hpp"

namespace Cortex
{
	///=============================================================================
	///	Helpers for storing, checking and printing enum types.
	///=============================================================================

	template<typename E, OnlyEnum<E>...>
	struct EnumNames
	{
		const std::string name;
		const std::string pretty_name;
		const std::string scoped_name;
	};

	template<typename E, OnlyEnum<E>...>
	using emap = vmap<E, EnumNames<E>>;

	/// Name of enum E
	template<typename E, OnlyEnum<E> ...>
	inline const std::string name;

	/// Description of enum E
	template<typename E, OnlyEnum<E> ...>
	inline const std::string desc;

	/// Default undefined value for enum E
	template<typename E, OnlyEnum<E> ...>
	inline const E undef;

	/// Dictionary of enum values and
	/// corresponding names for enum E
	template<typename E, OnlyEnum<E> ...>
	inline const emap<E> values;

	template<typename E, OnlyEnum<E>...>
	inline std::string prettify(std::string _str)
	{
		std::string ret;
		for (uint c = 0; c < _str.size(); ++c)
		{
			if (c == 0)
			{
				ret += std::toupper(_str[c]);
			}
			else if (!std::isalpha(_str[c]))
			{
				ret += ' ';
			}
			else if (std::isupper(_str[c]))
			{
				ret += ' ';
				ret += std::tolower(_str[c]);
			}
			else
			{
				ret += _str[c];
			}
		}

		return ret;
	}

	template <typename E, OnlyEnum<E>...>
	inline vmap<E, EnumNames<E>> enum_gen(std::string _enum_name, std::string _tokens)
	{
		strip(_tokens, ' ');

		auto tokens(split(_tokens, ','));

		auto pretty_tokens(tokens);

		for (auto& token : pretty_tokens)
		{
			token = prettify<E>(token);
		}

		vmap<E, EnumNames<E>> ret;

		uchar idx(0);
		for (uint t = 0; t < tokens.size(); ++t)
		{
			ret.emplace_back(static_cast<E>(++idx), EnumNames<E>{tokens[t], pretty_tokens[t], _enum_name + "::" + tokens[t]});
		}

		return ret;
	}

	/// @brief Enum to string conversion
	template<typename E, OnlyEnum<E> ...>
	inline std::string to_str(const E _enum)
	{
		for (const auto& m : values<E>)
		{
			if (m.first == _enum)
			{
				return m.second.name;
			}
		}
		return "Undef";
	}

	/// @brief String to enum conversion
	template<typename E, OnlyEnum<E> ...>
	inline E to_enum(const std::string& _str)
	{
		for (const auto& m : values<E>)
		{
			if (alnum(m.second.name) == alnum(_str))
			{
				return m.first;
			}
		}
		return undef<E>;
	}

	/// @brief Enum to string conversion
	template<typename E, OnlyEnum<E> ...>
	inline std::string pretty(const E _enum)
	{
		for (const auto& m : values<E>)
		{
			if (m.first == _enum)
			{
				return m.second.pretty_name;
			}
		}
		return "Undefined";
	}

	/// @brief Enum to string conversion
	template<typename E, OnlyEnum<E> ...>
	inline std::string scoped(const E _enum)
	{
		for (const auto& m : values<E>)
		{
			if (m.first == _enum)
			{
				return m.second.scoped_name;
			}
		}
		return name<E> + "::Undef";
	}

	/// This macro expands to define an enum class and
	/// the some associated helpers, such as class name,
	/// a value - string LUT, a description, an automatic
	/// undefined value (Undef) and JSON loaders.
	///
	/// @todo Replace with reflexpr.
#define ENUM(Desc, EnumName, ...) \
	enum class EnumName : uchar { Undef = 0, __VA_ARGS__ }; \
	template<> inline const std::string desc<EnumName> = Desc; \
	template<> inline const std::string name<EnumName> = #EnumName; \
	template<> inline const EnumName undef<EnumName> = EnumName::Undef; \
	template<> inline const emap<EnumName> values<EnumName> = enum_gen<EnumName>(#EnumName, #__VA_ARGS__); \
	inline void from_json(const json& _j, EnumName& _e) { _e = to_enum<EnumName>(_j.get<std::string>()); } \
	inline void to_json(json& _j, const EnumName& _e) { _j = to_str<EnumName>(_e); } \
	inline os& operator << (os& _os, const EnumName _e) noexcept { return _os << to_str<EnumName>(_e); } \
	inline dlog& operator << (dlog& _d, const EnumName _e) noexcept { return _d << to_str<EnumName>(_e); }

	///=====================================
	/// Enum classes.
	/// These are used as template arguments
	///	as well as in dispatch functions.
	///=====================================

	///=====================================
	/// Task modes.
	///=====================================

	ENUM("Task mode", TaskMode,
		 Autonomous,
		 Interactive
		 );

	///=====================================
	/// Task types.
	///=====================================

	ENUM("Task type", TaskType,
		 Classification,
		 Regression,
		 Prediction,
		 Control
		 );

	///=====================================
	/// Network types.
	///=====================================

	ENUM("Network type", NetType,
		 Classical,
		 Spiking,
		 Convolutional
		 );

	///=====================================
	/// Link types.
	///=====================================

	ENUM("Link type", LinkType,
		 Forward,
		 Recurrent,
		 Lateral,
		 Skip
		 );

	///=====================================
	/// Transfer functions.
	/// These also work as standalone functions.
	/// @todo More functions.
	///=====================================

	ENUM("Function type", Func,
		 Abs,
		 Cos,
		 Gaussian,
		 Logistic,
		 ReLU,
		 Sin,
		 Tanh
		 );

	///=====================================
	/// Optimisaiton phases.
	///=====================================

	ENUM("Optimisation phase", Phase,
		 Learning,
		 Evolution
		 );

	///=====================================
	/// Mutation types.
	///=====================================

	ENUM("Learning mode", LearningMode,
//		 Backprop,
		 Mutation,
		 STDP
		 );

	///=====================================
	/// Network element types.
	///=====================================

	ENUM("Network element type", ElemType,
		 Node,
		 Link,
		 Layer
		 );

	///=====================================
	/// Parameter types.
	///=====================================

	ENUM("Parameter type", Param,
		 Weight,
		 TimeConstant
		 );

	///=====================================
	/// The following two types are used
	/// for values which can increase,
	/// decrease or stay the same.
	/// These are used to keep track of
	/// changes and decide which mutations
	/// have been beneficial.
	///=====================================

	///=====================================
	/// Action (used in parameter mutation).
	///=====================================

	ENUM("Action", Action,
		 Inc,
		 Dec
		 );

	///=====================================
	/// Effect of an action.
	///=====================================

	ENUM("Effect", Effect,
		 Inc,
		 Dec
		 );

	///=====================================
	/// Spike encoding types.
	///=====================================

	ENUM("Spike encoding", SpikeEnc,
		 Time,
		 RankOrder
		 );

	///=====================================
	/// Receptive field types.
	///=====================================

	ENUM("Receptive field type", RFType,
		 Direct,
		 Adaptive,
		 Gaussian,
		 Spatiotemporal
		 );

	///=====================================
	/// Number distribution types.
	///=====================================

	ENUM("Number distribution", Dist,
		 Fixed,
		 Uniform,
		 Normal,
		 PosNormal,
		 NegNormal
		 );

	///=====================================
	/// Evaluation stage.
	///=====================================

	ENUM("Evaluation stage", Stage,
		 Train,
		 Dev,
		 Test
		 );

	///=====================================
	/// Layer types.
	///=====================================

	ENUM("Layer type", LayerType,
		 Regular,
		 Convolutional
		 );

	///=====================================
	/// Data types.
	///=====================================

	ENUM("Data type", DataType,
		 RealValued,
		 TimeSeries,
		 Image
		 );

	///=====================================
	/// Variables for which statistics
	/// should be collected.
	///=====================================

	ENUM("Statistics", Stat,
		 SuccessRate,
		 Generations,
		 Evaluations,
		 Species,
		 Nets,
		 Layers,
		 Nodes,
		 Links
		 );

	///=====================================
	/// Moving average types.
	///=====================================

	ENUM("Moving average type", MAType,
		 Exponential,
		 Simple
		 );

	///=====================================
	/// Order of dimensions for image input.
	/// These are used to map the input order
	/// into a canonical order (DHW)
	///=====================================

	ENUM("Dimension order", DimOrder,
		 DHW,
		 DWH,
		 WHD,
		 WDH,
		 HWD,
		 HDW
		 );

	///=============================================================================
	///	Error codes. Used for exception handling.
	///=============================================================================

	ENUM("Error type", Error,
		 OutOfRange,
		 InvalidValue
		 );
}

#endif // CORTEX_ENUM_HPP
