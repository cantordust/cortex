#include "Conf.hpp"
#include "Layer.hpp"

namespace Cortex
{
	Layer::Layer(const std::vector<std::vector<NodePtr>>& _nodes,
				 const LayerType _type)
		:
		  nodes(_nodes),
		  type(_type)
	{}

	Layer::Layer(std::vector<std::vector<NodePtr>>&& _nodes,
				 const LayerType _type)
		:
		  nodes(std::move(_nodes)),
		  type(_type)
	{}
}
