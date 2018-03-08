#ifndef CORTEX_LAYER_HPP
#define CORTEX_LAYER_HPP

#include "Globals.hpp"

namespace Cortex
{
	class Layer
	{
	private:

		std::vector<std::vector<NodePtr>> nodes;

		LayerType type;

	public:

		Layer(const std::vector<std::vector<NodePtr>>& _nodes,
			  const LayerType _type = LayerType::Regular);

		Layer(std::vector<std::vector<NodePtr>>&& _nodes,
			  const LayerType _type = LayerType::Regular);
	};
}

#endif // CORTEX_LAYER_HPP
