#include "Conf.hpp"
#include "LayerConf.hpp"
#include "Layer.hpp"

namespace Cortex
{
	Layer::Layer(LayerConf& _conf)
		:
		  conf(_conf)
	{
		init();
	}

	template<>
	void Layer::init<LayerType::Regular>()
	{
		output = vec(conf.out);
	}

	template<>
	void Layer::init<LayerType::Conv>()
	{

	}

	///=========================================
	/// Dispatch functions
	///=========================================

	void Layer::init()
	{
		switch (conf.type)
		{

		case LayerType::Conv:
			init<LayerType::Conv>();
			break;

		default:
			init<LayerType::Regular>();
			break;
		}
	}
}
