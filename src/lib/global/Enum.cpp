#include "Globals.hpp"

namespace Cortex
{
	template<> emap<OptMode> Enum<OptMode>::entries =
	{
		{OptMode::Evolution, "evolution"},
		{OptMode::GD, "gd"},
		{OptMode::Hybrid, "hybrid"}
	};
	template<> OptMode Enum<OptMode>::undef = OptMode::Undef;

	template<> emap<EnvType> Enum<EnvType>::entries =
	{
		{EnvType::Population, "population"},
		{EnvType::Single, "single"}
	};
	template<> EnvType Enum<EnvType>::undef = EnvType::Undef;

	template<> emap<NT> Enum<NT>::entries =
	{
		{NT::Classical, "classical"},
		{NT::Spiking, "spiking"}
	};
	template<> NT Enum<NT>::undef = NT::Undef;

	template<> emap<NR> Enum<NR>::entries =
	{
		{NR::B, "b"},
		{NR::I, "i"},
		{NR::O, "o"},
		{NR::H, "h"}
	};
	template<> NR Enum<NR>::undef = NR::Undef;

	template<> emap<LT> Enum<LT>::entries =
	{
		{LT::Fwd, "fwd"},
		{LT::Rec, "rec"}
	};
	template<> LT Enum<LT>::undef = LT::Undef;

	template<> emap<TF> Enum<TF>::entries =
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
	template<> TF Enum<TF>::undef = TF::Undef;

	template<> emap<Mut> Enum<Mut>::entries =
	{
		{Mut::AddLink, "add_link"},
		{Mut::AddNode, "add_node"},
		{Mut::EraseLink, "erase_link"},
		{Mut::EraseNode, "erase_node"},
		{Mut::TF, "tf"},
		{Mut::Tau, "tau"},
		{Mut::Weight, "weight"},
		{Mut::FT, "ft"}
	};
	template<> Mut Enum<Mut>::undef = Mut::Undef;

	template<> emap<ParamMut> Enum<ParamMut>::entries =
	{
		{ParamMut::TF, "tf"},
		{ParamMut::Tau, "tau"},
		{ParamMut::Weight, "weight"},
		{ParamMut::FT, "ft"},
	};
	template<> ParamMut Enum<ParamMut>::undef = ParamMut::Undef;

	template<> emap<StructMut> Enum<StructMut>::entries =
	{
		{StructMut::AddLink, "add_link"},
		{StructMut::AddNode, "add_node"},
		{StructMut::EraseLink, "erase_link"},
		{StructMut::EraseNode, "erase_node"}
	};
	template<> StructMut Enum<StructMut>::undef = StructMut::Undef;

	template<> emap<ParamOpt> Enum<ParamOpt>::entries =
	{
		{ParamOpt::ES, "trend"},
		{ParamOpt::Anneal, "anneal"}
	};
	template<> ParamOpt Enum<ParamOpt>::undef = ParamOpt::Undef;

	template<> emap<Act> Enum<Act>::entries =
	{
		{Act::Inc, "inc"},
		{Act::Dec, "dec"},
	};
	template<> Act Enum<Act>::undef = Act::Undef;

	template<> emap<Eff> Enum<Eff>::entries =
	{
		{Eff::Inc, "inc"},
		{Eff::Dec, "dec"},
	};
	template<> Eff Enum<Eff>::undef = Eff::Undef;

	template<> emap<Enc> Enum<Enc>::entries =
	{
		{Enc::Direct, "direct"},
		{Enc::RankOrder, "rank_order"}
	};
	template<> Enc Enum<Enc>::undef = Enc::Undef;

	template<> emap<RF> Enum<RF>::entries =
	{
		{RF::Direct, "direct"},
		{RF::ARF, "arf"},
		{RF::GRF, "grf"},
		{RF::ST, "st"}
	};
	template<> RF Enum<RF>::undef = RF::Undef;

	template<> emap<Search> Enum<Search>::entries =
	{
		{Search::Loss, "loss"},
		{Search::Fitness, "fitness"},
		{Search::Novelty, "novelty"},
		{Search::Pareto, "pareto"},
	};
	template<> Search Enum<Search>::undef = Search::Undef;

	template<> emap<NodeMark> Enum<NodeMark>::entries =
	{
		{NodeMark::Temp, "temp"},
		{NodeMark::Perm, "perm"}
	};
	template<> NodeMark Enum<NodeMark>::undef = NodeMark::Undef;

	template<> emap<Dist> Enum<Dist>::entries =
	{
		{Dist::Fixed, "fixed"},
		{Dist::Normal, "normal"},
		{Dist::Uniform, "uniform"}
	};
	template<> Dist Enum<Dist>::undef = Dist::Undef;

	template<> emap<MA> Enum<MA>::entries =
	{
		{MA::SMA, "sma"},
		{MA::EMA, "ema"}
	};
	template<> MA Enum<MA>::undef = MA::Undef;

	template<> emap<Set> Enum<Set>::entries =
	{
		{Set::Training, "training"},
		{Set::Validation, "validation"},
		{Set::Test, "test"}
	};
	template<> Set Enum<Set>::undef = Set::Undef;

	template<> emap<Layout> Enum<Layout>::entries =
	{
		{Layout::Random, "random"},
		{Layout::Layered, "layered"}
	};
	template<> Layout Enum<Layout>::undef = Layout::Undef;

	template<> emap<LayerType> Enum<LayerType>::entries =
	{
		{LayerType::Regular, "regular"},
		{LayerType::Conv, "conv"},
		{LayerType::Pool, "pool"},
		{LayerType::Softmax, "softmax"}
	};
	template<> LayerType Enum<LayerType>::undef = LayerType::Undef;

	template<> emap<DataType> Enum<DataType>::entries =
	{
		{DataType::Real, "real"},
		{DataType::TimeSeries, "time_series"}
	};
	template<> DataType Enum<DataType>::undef = DataType::Undef;

	template<> emap<Task> Enum<Task>::entries =
	{
		{Task::Classification, "classification"},
		{Task::Control, "control"},
		{Task::Prediction, "prediction"},
		{Task::Regression, "regression"}
	};
	template<> Task Enum<Task>::undef = Task::Undef;
}
