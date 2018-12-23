#include "eval.hpp"

int main( int _argc, char* _argv[] )
{
	cortex::parse(_argc, _argv);

	cortex::setup(&Dummy::evaluate);

//	dlog(Cortex::args.help());

//	dlog{conf};

	cortex::run();

	Async::dlog("Task completed, exiting.");

	return 0;
}

