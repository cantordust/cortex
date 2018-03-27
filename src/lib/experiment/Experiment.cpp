#include "Experiment.hpp"

namespace Cortex
{
	Experiment::Experiment(Conf& _conf)
		:
		  conf(_conf),
		  running(false),
		  evals(MA::SMA),
		  gens(MA::SMA),
		  hidden_nodes(MA::SMA)
	{
		dlog() << "Experiment created in thread " <<
				  std::this_thread::get_id() << "\n";
	}

	void Experiment::setup(const EvalFunc& _ef)
	{
		experiment = std::thread([&]
		{
			if (running)
			{
				uint run(1);

				while (run <= conf.sys.runs)
				{
					Env env(conf, _ef);

					if (!env.init())
					{
						dlog() << "Error initialising environment, exiting...";
						exit(EXIT_FAILURE);
					}

					dlog() << "\n### Run " << run << " / " << conf.sys.runs << " ###\n\n";

					while (env.age <= conf.env.max.age)
					{
						env.eval();
						evals.update(env.get_evals());
						gens.update(env.age);
						hidden_nodes.update(env.get_champ()->get_node_count(NR::H));

						if ( env.is_solved() )
						{
							break;
						}
					}

					++run;
				}
			}
		});
	}

	void Experiment::print_summary()
	{
		dlog() << "*** Experiment statistics ***\n"
			   << "\nSuccess rate: " << evals.count << " / " << conf.sys.runs
			   << "\nEvaluations:\n" << evals
			   << "\nGenerations:\n" << gens
			   << "\nHidden nodes:\n" << hidden_nodes;
	}

	void Experiment::run()
	{
		running.store(true);
		experiment.join();
		running.store(false);
	}
}
