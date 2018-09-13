#include "Task.hpp"
#include "Env.hpp"
#include "Conf.hpp"

namespace Cortex
{
	///=====================================
	/// Local functions
	///=====================================

	void greet()
	{
		dlog banner;
		banner + "\n" + title + "\n";
		banner + "\nLicence:\t" + licence;
		banner + "\nVersion:\t" + version;
//		greeter + "\nCopyright:\t", copyright;
		banner + "\n\n" + std::string(title.size(), '=') + "\n";
	}

	/// @brief Set some basic default arguments.
	void set_args()
	{
		Task::args.add_options()
				("c,config", "Specify a configuration file.", cxxopts::value<std::string>()->default_value("config.json"))
				("g,generate", "Generate a default configuration file and exit.", cxxopts::value<std::string>()->implicit_value("default_config.json"))
				;
	}

	///=====================================
	/// Setup and execution
	///=====================================

	void Task::setup(int _argc, char* _argv[], std::function<void(const NetPtr)> _evaluate)
	{
		/// Ensure that we have a single task
		static bool called(false);
		require(!called, "You can only create a single task at a time.");
		called = true;

		/// Set the default command line options.
		set_args();

		require(!conf, "Only a single configuration is allowed at a time. Exiting.");

		/// Create a new configuration
		conf.reset(new Conf());

		if (!conf)
		{
			die("Error creating configuration, exiting.");
		}

		/// Parse the options
		auto opts(args.parse(_argc, _argv));

		/// Check if there is a request to generate
		/// or load the configuration.
		Conf::file = opts.count("generate") ? opts["generate"].as<std::string>() : opts["config"].as<std::string>();

		if (opts.count("generate"))
		{
			conf->save();

			dlog("Configuration saved successfully to ", Conf::file);

			return;
		}

		/// Load the configuration
		conf->load();

		/// Store the evaluate function.
		evaluate = _evaluate;

		/// Seed the random number generator
		seed_rng();

		/// Create a task.
		make_task();

		/// Issue a greeting.
		greet();

		dlog("Task set up successfully.");
	}

	void Task::execute()
	{
		static bool called(false);
		require(!called, "You can only execute a task once.\nTry changing the number of runs in the configuration instead.");
		called = true;

		/// Execute the task
		task();
	}

	void Task::make_task()
	{
		task = [&]
		{
			/// Task run counter
			uint run(0);

			dlog report;

			while (++run <= conf->task.runs)
			{
				report << "\n==============[ Run" << run << "/" << conf->task.runs << "]==============\n";

				require(Env::initialise(), "Environment initialisation failed. Exiting.");

				/// Add a new run to the history.
				Task::history.new_run();

				/// Epoch counter.
				uint epoch(0);

				/// Store the number of evaluations from the last run.
				/// The number of evaluations for this run is computed
				/// by subtracting this stored number from the number
				/// of completed tasks in the threadpool.
				uint evals(threadpool.tasks_completed());

				while (++epoch <= conf->task.epochs)
				{
//					/// Switch to learning phase
					Env::phase = Phase::Learning;

					report << "==========[ Generation" << epoch << "|" << Env::phase << "phase ]==========\n";

//					/// Evaluate the population
//					for (const auto& net : Env::nets)
//					{
//						threadpool.enqueue(Task::evaluate, net);
//					}

					/// Wait for the threadpool to sync.
//					threadpool.sync();

//					/// Check if the task has been solved.
//					if (is_solved())
//					{
//						break;
//					}

					/// Switch to evolution phase
					Env::phase = Phase::Evolution;

					report << "==========[ Generation" << epoch << "|" << Env::phase << "phase ]==========\n";

					/// Evolve the environment
//					Env::evolve();

					/// Update the history for this run.
					history.add(Stat::Nets, Env::nets.size());
					history.add(Stat::Species, Env::species.size());

					/// These are average values over all nets for the current run.
					history.add(Stat::Layers, Env::count(ElemType::Layer) / flt(Env::nets.size()));
					history.add(Stat::Nodes, Env::count(ElemType::Node) / flt(Env::nets.size()));
					history.add(Stat::Links, Env::count(ElemType::Link) / flt(Env::nets.size()));
				}

				/// Update the history.
				history.add(Stat::SuccessRate, is_solved() ? 1 : 0);
				history.add(Stat::Generations, epoch - 1);
				history.add(Stat::Evaluations, threadpool.tasks_completed() - evals);
			}
		};

		/// Print statistics about the task.
		history.print();
	}

	bool Task::is_solved(const NetPtr _net, const bool _reset)
	{
		/// A switch indicating whether the task has been solved
		static bool solved(false);

		/// Mutex for manipulating the "solved" switch.
		static std::mutex mtx;

		glock lk(mtx);

		if (_net)
		{
			if (!solved)
			{
				solved = true;
				Env::champ = _net.get();
				threadpool.stop();
			}
		}
		else if (_reset)
		{
			solved = false;
		}

		return solved;
	}

//	void Task::evaluate(const NetPtr _net)
//	{

//	}
}
