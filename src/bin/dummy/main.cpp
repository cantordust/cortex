#include "eval.hpp"

int main( int argc, char* argv[] )
{
	Cortex::Task::setup(argc, argv);

//	dlog(Task::args.help());

//	dlog{*conf};

	Cortex::Task::execute();

	Async::dlog("Task completed, exiting.");

	return 0;
}

