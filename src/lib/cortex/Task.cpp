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
	/// Bookkeeping
	///=====================================

	void Task::print_stats()
	{
		/// Stats report
		dlog report;

		std::string header("---------------[ Task statistics ]---------------");
		std::vector<std::string> fields{"Variable", "Mean", "SD"};
		std::vector<uint> width;
		uint total(std::accumulate(fields.cbegin(),
								   fields.cend(),
								   0,
								   [](const uint _sum, const std::string& _field)
		{
			return _sum + _field.size();
		}));

		/// Compute the field sizes.
		for (const auto& f : fields)
		{
			width.emplace_back((f.size() / flt(total)) * header.size() - 2);
		}

		/// Header
		report + "\n" + header + "\n";
		report.left().setfill(' ');
		for (uint f = 0; f < fields.size(); ++f)
		{
			report.add(" ").format(fields[f], width[f]).add((f < fields.size() - 1 ? "|" : ""));
		}
		report + "\n" + std::string(header.size(), '-');

		for (const auto& stat : stats)
		{
			uint idx(0);
			report.add("\n ").format(pretty(stat.first), width[idx]);
			report.add("| ").format(stat.second.mean, width[++idx]);
			report.add("| ").format(stat.second.sd(), width[++idx]);
		}

		/// Bottom line
		report + "\n" + std::string(header.size(), '-') + "\n";
	}

	///=====================================
	/// Setup and execution
	///=====================================

	void Task::setup(int _argc, char* _argv[])
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
		require(!called, "You can only execute a task once.", "\nTry changing the number of runs in the configuration instead.");
		called = true;

		/// Execute the task
		task();
	}

	void Task::make_task()
	{
		task = [&]
		{
			uint run(0);

			while (++run <= conf->task.runs)
			{
				require(Env::initialise(), "Environment initialisation failed. Exiting.");

				dlog("\n==============[ Run", run, "/", conf->task.runs, "]==============\n");

				while (Env::stats[Stat::Generations].value < conf->task.generations)
				{
//					/// Evaluate the population
//					Env::evaluate();

//					/// Check if the task has been solved.
//					if (is_solved())
//					{
//						break;
//					}

					/// Evolve the environment
//					Env::evolve();

					/// Update the statistics.
					Env::stats[Stat::Generations].add(1);
				}

				/// Update the global statistics
				for (const auto& stat : Env::stats)
				{
					stats[stat.first].update(stat.second.value);
				}
			}

			threadpool.sync();

			print_stats();
		};
	}

	bool Task::is_solved(const NetPtr& _net, const bool _reset)
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
				Env::stats[Stat::SuccessRate].add(1);
				threadpool.stop();
			}
		}
		else if (_reset)
		{
			solved = false;
		}

		return solved;
	}
}
