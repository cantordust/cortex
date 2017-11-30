#include "Enum.hpp"

namespace Cortex
{

	template<> EnumMap<NT> Enum<NT>::entries =
	{
		{NT::Classical, "classical"},
		{NT::Spiking, "spiking"}
	};
	template<> NT Enum<NT>::undef = NT::Undef;

	template<> EnumMap<NR> Enum<NR>::entries =
	{
		{NR::B, "b"},
		{NR::I, "i"},
		{NR::O, "o"},
		{NR::H, "h"}
	};
	template<> NR Enum<NR>::undef = NR::Undef;

	template<> EnumMap<LT> Enum<LT>::entries =
	{
		{LT::F, "fwd"},
		{LT::R, "rec"}
	};
	template<> LT Enum<LT>::undef = LT::Undef;

	template<> EnumMap<Fn> Enum<Fn>::entries =
	{
		{Fn::Tanh, "tanh"},
		{Fn::Logistic, "logistic"},
		{Fn::ReLU, "relu"},
		{Fn::Gaussian, "gaussian"},
		{Fn::Sin, "sin"},
		{Fn::Cos, "cos"},
		{Fn::Abs, "abs"},
		{Fn::Min, "min"},
		{Fn::Max, "max"},
		{Fn::Sum, "sum"},
		{Fn::Avg, "avg"},
		{Fn::Const, "const"},
		{Fn::Golden, "golden"}
	};
	template<> Fn Enum<Fn>::undef = Fn::Undef;

	template<> EnumMap<Mut> Enum<Mut>::entries =
	{
		{Mut::AddNode, "add_node"},
		{Mut::EraseNode, "erase_node"},
		{Mut::AddLink, "add_link"},
		{Mut::EraseLink, "erase_link"},
		{Mut::Weight, "weight"},
		{Mut::Tau, "tau"},
		{Mut::Fn, "fn"}
	};
	template<> Mut Enum<Mut>::undef = Mut::Undef;

	template<> EnumMap<Opt> Enum<Opt>::entries =
	{
		{Opt::Trend, "trend"},
		{Opt::Anneal, "anneal"}
	};
	template<> Opt Enum<Opt>::undef = Opt::Undef;

	template<> EnumMap<STDP> Enum<STDP>::entries =
	{
		{STDP::Heb, "heb"},
		{STDP::AntiHeb, "antiheb"}
	};
	template<> STDP Enum<STDP>::undef = STDP::Undef;

	template<> EnumMap<Act> Enum<Act>::entries =
	{
		{Act::Inc, "inc"},
		{Act::Dec, "dec"},
	};
	template<> Act Enum<Act>::undef = Act::Undef;

	template<> EnumMap<Eff> Enum<Eff>::entries =
	{
		{Eff::Inc, "inc"},
		{Eff::Dec, "dec"},
	};
	template<> Eff Enum<Eff>::undef = Eff::Undef;

	template<> EnumMap<Enc> Enum<Enc>::entries =
	{
		{Enc::Time, "time"},
		{Enc::RankOrder, "rank_order"}
	};
	template<> Enc Enum<Enc>::undef = Enc::Undef;

	template<> EnumMap<RF> Enum<RF>::entries =
	{
		{RF::Direct, "direct"},
		{RF::ARF, "arf"},
		{RF::GRF, "grf"},
		{RF::ST, "st"}
	};
	template<> RF Enum<RF>::undef = RF::Undef;

	template<> EnumMap<Search> Enum<Search>::entries =
	{
		{Search::Fitness, "fitness"},
		{Search::Novelty, "novelty"}
	};
	template<> Search Enum<Search>::undef = Search::Undef;

	template<> EnumMap<NodeMark> Enum<NodeMark>::entries =
	{
		{NodeMark::Temp, "temp"},
		{NodeMark::Perm, "perm"}
	};
	template<> NodeMark Enum<NodeMark>::undef = NodeMark::Undef;

	template<> EnumMap<InitDist> Enum<InitDist>::entries =
	{
		{InitDist::Fixed, "fixed"},
		{InitDist::Normal, "normal"},
		{InitDist::Uniform, "uniform"}
	};
	template<> InitDist Enum<InitDist>::undef = InitDist::Undef;

	template<> EnumMap<MAType> Enum<MAType>::entries =
	{
		{MAType::Simple, "simple"},
		{MAType::WMA, "wma"},
		{MAType::EMA, "ema"}
	};
	template<> MAType Enum<MAType>::undef = MAType::Undef;

	template<> EnumMap<Topology> Enum<Topology>::entries =
	{
		{Topology::Random, "random"},
		{Topology::Layered, "layered"},
		{Topology::Conv, "conv"}
	};
	template<> Topology Enum<Topology>::undef = Topology::Undef;
}
