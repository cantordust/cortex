#include "Cortex.hpp"
#include "Genome.hpp"
#include "Mutation.hpp"

namespace Cortex
{
	namespace
	{
		///=============================================================================
		///	Strings providing the title, description, version
		/// and other information about the library.
		///=============================================================================

		const std::string title{"=============[ Cortex neuroevolution platform ]============="};

		/// @todo This is clunky.
		/// Automate the version bump process.
		const std::string version{"0.6.0.2"};

		const std::string licence{"MIT (https://opensource.org/licence/MIT)"};

		/// @todo Add author / contributor information.
		const std::string copyright{""};

		///=============================================================================
		///	Task-related variables variables
		///=============================================================================

		/// Threadpool for evaluating networks in parallel
		up<ThreadPool> threadpool{nullptr};

		/// Statistics and individual records about the task -
		/// number of runs, network and genomes count,
		/// success rate, etc.
		up<History> history{nullptr};

		/// An executable task
		up<std::function<void()>> task{nullptr};

		/// @brief Evaluation function
		std::function<void(const NetPtr)> evaluator;

		/// The phase that the environment is in.
		/// During the learning phase,
		/// networks optimise their parameters
		/// or add new connections,
		/// while in the evolution phase networks
		/// add new genes and participate in crossover.
		Phase phase{Phase::Undef};

		/// Atomic toggle to indicate whether the task has been solved.
		toggle solved{false};
	}

	void info()
	{
		dlog banner;
		banner + "\n" + title + "\n";
		banner + "\nLicence:\t" + licence;
		banner + "\nVersion:\t" + version;
//		greeter + "\nCopyright:\t", copyright;
		banner + "\n\n" + std::string(title.size(), '=') + "\n";
	}

	///=====================================
	/// Setters, getters and counters
	///=====================================

	template<>
	uint count<ElemType>(const ElemType _et)
	{
		switch (_et)
		{
		case ElemType::Genome:
			return Genome::genomes.size();

		case ElemType::Net:
			return Net::nets.size();

		default:
			return Cortex::count(Net::nets, [&](const uint _sum, const auto& _net)
			{
				return _sum + _net->count(_et);
			});
		}
	}

	template<>
	uint count<LayerType>(const LayerType _lt)
	{
		return Cortex::count(Net::nets, [&](const uint _sum, const auto& _net)
		{
			return _sum + _net->count(_lt);
		});
	}

	template<>
	uint count<LinkType>(const LinkType _lt)
	{
		return Cortex::count(Net::nets, [&](const uint _sum, const auto& _net)
		{
			return _sum + _net->count(_lt);
		});
	}

	///=====================================
	/// Setup
	///=====================================

	///=============================================================================
	///	Local functions
	///=============================================================================

//	GenomePtr find(const std::vector<LayerConf>& _layers)
//	{
//		if (!conf.genome.enabled)
//		{
//			return nullptr;
//		}

//		/// First, check if the genome exists.
//		for (const auto& [genome, nets] : Net::genomes)
//		{
//			if (*genome == _genome)
//			{
//				return genome;
//			}
//		}

//		return nullptr;
//	}

//	GenomePtr create(const Genome& _genome)
//	{
//		/// If speciation is disabled, there will be only one genome.
//		/// Just return that.
//		if (!conf.genome.enabled &&
//			!Net::genomes.empty())
//		{
//			return Net::genomes.begin()->first;
//		}

//		/// Check if the genome exists.
//		if (auto genome = find(_genome))
//		{
//			return genome;
//		}

//		/// If the genome doesn't exist, check if we can add it.
//		if (conf.genome.max.count > 0 &&
//			Net::genomes.size() >= conf.genome.max.count)
//		{
//			/// The genome doesn't exist and we can create it.
//			/// Insert the new genome into the environment.
//			return nullptr;
//		}

//		GenomePtr genome(mksp<Genome>(_genome));
//		Net::genomes[genome] = {};
//		return genome;
//	}

//	NetPtr add_net(const GenomePtr& _genome)
//	{
//		NetPtr net(mksp<Net>(_genome));
//		if (_genome)
//		{
//			_genome->nets.emplace(net);
//		}
//		return net;
//	}

//	NetPtr add_net(const NetPtr& _p1, const NetPtr& _p2)
//	{
//		NetPtr net(mksp<Net>(_p1, _p2));
//		if (_p1->genome)
//		{

//		}
//		Net::nets.emplace(net);
//		return net;
//	}

	real get_offset(const ElemType _elem, const uint _count)
	{
		SMAStat stat;
		Cortex::apply(Net::nets, [&](const auto& _net)
		{
			stat.update(_net->count(_elem));
		});

		//		dlog("[get_offset()] offset for", _elem, ":", stat.get_offset(_count));

		return stat.get_offset(_count);
	}

	void reset()
	{
		///=================
		/// Reset everything
		///=================

		/// Set the phase to undefined.
		phase = Phase::Undef;

		/// Reset the champion.
		Net::champ = nullptr;

		/// Clear the genome container.
		Genome::genomes.clear();
		Genome::id_generator.reset();

		/// Clear the genome container.
		Net::nets.clear();
		Net::id_generator.reset();
	}

	bool initialise()
	{
		dlog("---> Initialising environment...");

		reset();

		///=================
		/// Generate genome.
		///=================

		/// Population size (the number of networks per genome).
		uint net_quota(conf.net.init.count);

		/// If speciation is enabled, distribute the net quota
		/// evenly among the initial species.
		if (conf.species.enabled)
		{
			net_quota /= conf.species.init.count;

			/// Proto-genome.
			GenomePtr proto_genome(Genome::create(conf.net.init.layers));

			/// Generate proto-genomes and proto-nets
			while (true)
			{
				/// Add the proto-nets for this genome.
				for (uint n = 0; n < net_quota; ++n)
				{
					NetPtr proto_net(Net::create(*proto_genome));
					if (!proto_net)
					{
						dlog("Environment initialisation failed: error creating network.");
						return false;
					}
				}

				if (Genome::genomes.size() == conf.species.init.count)
				{
					break;
				}

				proto_genome = Genome::create(*proto_genome);
	//			dlog(">>> Genome", proto_genome->id, "inserted.");

				if (!proto_genome)
				{
					dlog("Environment initialisation failed: error creating genome.");
					return false;
				}

				/// Mutate the initial genome to get a different genome.
				MutationPtr mutation;

				do
				{
	//				dlog(">>> Waiting for mutation...");

					/// Mutate the genome until we get a valid mutation.
					while (!(mutation = proto_genome->mutate()));

					/// Check if a genome with the same genome exists.
					/// If it does, start over.
				} while (Genome::match(*proto_genome));

	//			dlog{mut};
			}
		}
		else
		{
			for (uint n = 0; n < net_quota; ++n)
			{
				Net::create(conf.net.init.layers);
			}
		}

		///=================
		/// Final sanity check.
		///=================

		if (Net::nets.empty())
		{
			dlog("Environment initialisation failed.");
			return false;
		}

		dlog("Environment initialised successfully");
		return true;
	}

	void evaluate()
	{
		dlog("Evaluating networks...");
	}

	void mark_solved(const NetPtr& _net)
	{
		static std::mutex mtx;
		glock lk(mtx);
		if (!solved)
		{
			solved = true;
			threadpool->stop();
			Net::champ = _net;
		}
	}

	void make_task()
	{
		/// Spawn the threadpool
		threadpool = mkup<ThreadPool>(conf.task.threads);

		/// Make history :)
		history = mkup<History>();

		task = mkup<std::function<void()>>([&]
		{
			/// Task run counter
			uint run(0);

			dlog report;

			while (++run <= conf.task.runs)
			{
				report << "\n==============[ Run" << run << "/" << conf.task.runs << "]==============\n";

				require(initialise(), "Environment initialisation failed. Exiting.");

				/// Add a new run to the history.
				history->new_run();

				/// Set the task to unsolved
				solved = false;

				/// Epoch counter.
				uint epoch(0);

				/// Store the number of evaluations from the last run.
				/// The number of evaluations for this run is computed
				/// by subtracting this stored number from the number
				/// of completed tasks in the threadpool.
				uint evals(threadpool->tasks_completed());

				while (++epoch <= conf.task.epochs)
				{
					evaluate();

					/// Update the history for this run.
					history->add(Stat::Nets, count(ElemType::Net));
					history->add(Stat::Species, count(ElemType::Genome));

					/// These are average values over all nets for the current epoch.
					history->add(Stat::Layers, count(ElemType::Layer) / flt(count(ElemType::Net)));
					history->add(Stat::Nodes, count(ElemType::Node) / flt(count(ElemType::Net)));
					history->add(Stat::Links, count(ElemType::Link) / flt(count(ElemType::Net)));
				}

				/// Update the history.
				history->add(Stat::SuccessRate, solved ? 1 : 0);
				history->add(Stat::Epochs, epoch - 1);
				history->add(Stat::Evaluations, threadpool->tasks_completed() - evals);
			}
		});
	}

	///=====================================
	/// Setup and execution
	///=====================================

	void parse(int& _argc, char** _argv)
	{
		auto result(opts.parse(clara::Args(_argc, _argv)));
	}

	void setup(const std::function<void(const NetPtr)>& _evaluator)
	{
		/// Ensure that we have a single task
		static bool called(false);
		require(!called, "You can only create a single task at a time.");
		called = true;

		if (conf.status == ConfigStatus::Generated)
		{
			return;
		}

		if (conf.status != ConfigStatus::Loaded)
		{
			auto report(conf.load());
			if (!report.empty())
			{
				conf.status = ConfigStatus::Undef;
				dlog("Configuration errors found:\n", report);
				return;
			}
		}

		// Seed the random number generator
		seed_rng();

		/// Store the evaluation function
		evaluator = _evaluator;

		/// Prepare the executable task.
		make_task();

		/// Issue a greeting.
		info();

		dlog("Task set up successfully.");
	}

	void run()
	{
		static bool called(false);
		require(!called, "You can only execute a task once.\nTry changing the number of runs in the configuration instead.");
		called = true;

		if (task)
		{
			/// Execute the task
			(*task)();

			/// Finalise the history
			history->close();

			/// Print statistics about the task.
			history->print();
		}
	}

	void enqueue(const NetPtr _net)
	{
		threadpool->enqueue(evaluator, _net);
	}

} // namespace Cortex
