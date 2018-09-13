#ifndef CORTEX_TASK_HPP
#define CORTEX_TASK_HPP

#include "Globals.hpp"
#include "Stat.hpp"

namespace Cortex
{
	class Task
	{

	public:

		///=====================================
		/// This holds the command line arguments
		/// accepted by Cortex by default
		/// (cf. set_args() in Task.cpp).
		/// It can be extended with more custom
		///	options in each executable.
		///=====================================
		inline static cxxopts::Options args = {"executable", Cortex::title};

	private:

		/// Threadpool for evaluating networks in parallel
		inline static ThreadPool threadpool;

		/// The actual task to execute
		inline static std::function<void()> task = []{};

		/// Statistics and individual records about the task -
		/// number of runs, network and species count,
		/// success rate, etc.
		inline static History history;

		///=====================================
		/// Evaluation function
		///=====================================

		inline static std::function<void(const NetPtr _net)> evaluate;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		Task() = delete;

		Task(const Task& _other) = delete;

		Task(Task&& _other) = delete;

		///=====================================
		/// Operators
		///=====================================

		Task& operator = (const Task& _other) = delete;

		///=====================================
		/// Setup and execution
		///=====================================

		/// @brief Set up the task
		static void setup(int _argc, char*_argv[], std::function<void(const NetPtr)> _evaluate);

		/// @brief Run the experiment
		static void execute();

	private:

		/// @brief Generate an executable task.
		static void make_task();

		/// @brief Mark the task as solved by net @p _net
		static bool is_solved(const NetPtr _net = nullptr, const bool _reset = false);

		friend class Env;
		friend class Net;
	};
}

#endif // CORTEX_TASK_HPP
