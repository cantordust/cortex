#include "Conf.hpp"
#include "Net.hpp"
#include "Evaluator.hpp"

namespace Cortex
{
	Evaluator::Evaluator(Net& _net)
		:
		  net(_net)
	{}

	void Evaluator::make_layers()
	{
		std::queue<Layer> lq;

		std::vector<NodePtr> l_out(net.get_nodes(NR::O));
		std::vector<NodePtr> l_in;
		for (const auto& nptr : l_out)
		{

		}


	}
}
