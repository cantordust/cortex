#include "Globals.hpp"

namespace Cortex
{
	template<>
	EMap<NT> Enum<NT>::entries =
	{
		{NT::Classical, "classical"},
		{NT::Spiking, "spiking"}
	};
	template<>
	NT Enum<NT>::undef = NT::Undef;

	template<>
	EMap<NR> Enum<NR>::entries =
	{
		{NR::B, "b"},
		{NR::I, "i"},
		{NR::O, "o"},
		{NR::H, "h"}
	};
	template<>
	NR Enum<NR>::undef = NR::Undef;

	template<>
	EMap<LT> Enum<LT>::entries =
	{
		{LT::Fwd, "fwd"},
		{LT::Rec, "rec"}
	};
	template<>
	LT Enum<LT>::undef = LT::Undef;

	template<>
	EMap<TF> Enum<TF>::entries =
	{
		{TF::Abs, "abs"},
		{TF::Avg, "avg"},
		{TF::Const, "const"},
		{TF::Cos, "cos"},
		{TF::Gaussian, "gaussian"},
		{TF::Golden, "golden"},
		{TF::Logistic, "logistic"},
		{TF::Max, "max"},
		{TF::Min, "min"},
		{TF::ReLU, "relu"},
		{TF::Sin, "sin"},
		{TF::Spiking, "spiking"},
		{TF::Sum, "sum"},
		{TF::Tanh, "tanh"}

	};
	template<>
	TF Enum<TF>::undef = TF::Undef;

	template<>
	EMap<Mut> Enum<Mut>::entries =
	{
		{Mut::AddLink, "add_link"},
		{Mut::AddNode, "add_node"},
		{Mut::EraseLink, "erase_link"},
		{Mut::EraseNode, "erase_node"},
		{Mut::TF, "tf"},
		{Mut::Tau, "tau"},
		{Mut::Weight, "weight"}
	};
	template<>
	Mut Enum<Mut>::undef = Mut::Undef;

	template<>
	EMap<ParamMut> Enum<ParamMut>::entries =
	{
		{ParamMut::TF, "tf"},
		{ParamMut::Tau, "tau"},
		{ParamMut::Weight, "weight"}
	};
	template<>
	ParamMut Enum<ParamMut>::undef = ParamMut::Undef;

	template<>
	EMap<StructMut> Enum<StructMut>::entries =
	{
		{StructMut::AddLink, "add_link"},
		{StructMut::AddNode, "add_node"},
		{StructMut::EraseLink, "erase_link"},
		{StructMut::EraseNode, "erase_node"}
	};
	template<>
	StructMut Enum<StructMut>::undef = StructMut::Undef;

	template<>
	EMap<Opt> Enum<Opt>::entries =
	{
		{Opt::Trend, "trend"},
		{Opt::Anneal, "anneal"}
	};
	template<>
	Opt Enum<Opt>::undef = Opt::Undef;

	template<>
	EMap<Act> Enum<Act>::entries =
	{
		{Act::Inc, "inc"},
		{Act::Dec, "dec"},
	};
	template<>
	Act Enum<Act>::undef = Act::Undef;

	template<>
	EMap<Eff> Enum<Eff>::entries =
	{
		{Eff::Inc, "inc"},
		{Eff::Dec, "dec"},
	};
	template<>
	Eff Enum<Eff>::undef = Eff::Undef;

	template<>
	EMap<Enc> Enum<Enc>::entries =
	{
		{Enc::Direct, "direct"},
		{Enc::RankOrder, "rank_order"}
	};
	template<>
	Enc Enum<Enc>::undef = Enc::Undef;

	template<>
	EMap<RF> Enum<RF>::entries =
	{
		{RF::None, "none"},
		{RF::ARF, "arf"},
		{RF::GRF, "grf"},
		{RF::ST, "st"}
	};
	template<>
	RF Enum<RF>::undef = RF::Undef;

	template<>
	EMap<Search> Enum<Search>::entries =
	{
		{Search::Fitness, "fitness"},
		{Search::Novelty, "novelty"}
	};
	template<>
	Search Enum<Search>::undef = Search::Undef;

	template<>
	EMap<NodeMark> Enum<NodeMark>::entries =
	{
		{NodeMark::Temp, "temp"},
		{NodeMark::Perm, "perm"}
	};
	template<>
	NodeMark Enum<NodeMark>::undef = NodeMark::Undef;

	template<>
	EMap<Dist> Enum<Dist>::entries =
	{
		{Dist::Fixed, "fixed"},
		{Dist::Normal, "normal"},
		{Dist::Uniform, "uniform"}
	};
	template<>
	Dist Enum<Dist>::undef = Dist::Undef;

	template<>
	EMap<MA> Enum<MA>::entries =
	{
		{MA::SMA, "sma"},
		{MA::EMA, "ema"}
	};
	template<>
	MA Enum<MA>::undef = MA::Undef;

	template<>
	EMap<Set> Enum<Set>::entries =
	{
		{Set::Training, "training"},
		{Set::Test, "test"},
		{Set::Validation, "validation"}
	};
	template<>
	Set Enum<Set>::undef = Set::Undef;

	template<>
	EMap<Layout> Enum<Layout>::entries =
	{
		{Layout::Random, "random"},
		{Layout::Layered, "layered"},
		{Layout::Conv, "conv"}
	};
	template<>
	Layout Enum<Layout>::undef = Layout::Undef;

	template<>
	EMap<Task> Enum<Task>::entries =
	{
		{Task::Classification, "classification"},
		{Task::Control, "control"},
		{Task::Prediction, "prediction"},
		{Task::Regression, "regression"}
	};
	template<>
	Task Enum<Task>::undef = Task::Undef;

	template<>
	EMap<DataType> Enum<DataType>::entries =
	{
		{DataType::Real, "real"},
		{DataType::TimeSeries, "time_series"}
	};
	template<>
	DataType Enum<DataType>::undef = DataType::Undef;
}
