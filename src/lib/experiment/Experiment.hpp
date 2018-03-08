#ifndef CORTEX_EXPERIMENT_HPP
#define CORTEX_EXPERIMENT_HPP

#include "Ecosystem.hpp"
#include "Net.hpp"

namespace Cortex
{
	class Experiment
	{
	private:

		flag running;

		/// Threads containing experiments.
		std::vector<std::thread> experiments;

	public:

		Experiment();

		template<typename F, typename ... Args>
		void setup(Conf& _conf, F&& _eval_func, Args&& ... _args)
		{
			experiments.push_back(std::thread([&]
			{
				if (running)
				{
					dlog() << "Experiment created in thread " <<
							  std::this_thread::get_id() << "\n";

					uint runs(_conf.sys.runs);
					uint run(1);
					uint successes(0);
					std::vector<uint> evals;
					std::vector<uint> gens;
					std::vector<uint> hidden_nodes;

					while (run <= runs)
					{
						Ecosystem es(_conf);

						if (!es.init())
						{
							dlog() << "Error initialising the ecosystem, exiting...";
							exit(EXIT_FAILURE);
						}

						dlog() << "\n################\n"
							   << "Run " << run << " / " << runs
							   << "\n################\n\n";

						while (es.age <= _conf.eco.max.age)
						{
							es.eval(std::forward<F>(_eval_func), std::forward<Args>(_args)...);
							if ( es.is_solved() )
							{
								++successes;
								evals.push_back(es.get_evals());
								gens.push_back(es.age);
								hidden_nodes.push_back(es.get_champ()->get_node_count(NR::H));

								break;
							}
						}

						++run;
					}

					dlog stats;
					stats << "Experiment statistics:\n"
						  << "\tSuccess rate: " << successes << " / " << runs << "\n";

					if (successes > 0)
					{
						real denom(static_cast<real>(successes));
						real mean_evals(Avg(evals));
						real mean_gens(std::accumulate(gens.begin(), gens.end(), 0.0) / denom);
						real mean_hidden_nodes(std::accumulate(hidden_nodes.begin(), hidden_nodes.end(), 0.0) / denom);

						stats << "\tAverage number of evaluations: " << mean_evals << "\n"
							  << "\tAverage number of generations: " << mean_gens << "\n"
							  << "\tAverage number of hidden nodes: " << mean_hidden_nodes;
					}
				}
			}));
		}

		void run();
	};
}

#endif // CORTEX_EXPERIMENT_HPP
