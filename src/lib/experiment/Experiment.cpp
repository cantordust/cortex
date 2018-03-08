#include "Experiment.hpp"

namespace Cortex
{

	Experiment::Experiment()
	    :
	      running(false)
	{}

	void Experiment::run()
	{
		running.store(true);
		for (auto&& e : experiments)
		{
			e.join();
		}
	}

}
