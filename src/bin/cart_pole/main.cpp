#include "eval.hpp"

using namespace CartPole;

int main( int argc, char* argv[] )
{
	TCLAP::CmdLine cmd( "Cart pole balancing experiment", ' ', Cortex::version, true );
	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config.json", "string", cmd );
	cmd.parse(argc, argv);

	Conf conf(config_file.getValue());
	if (!setup(conf))
	{
		return 0;
	}

	test();
	return 0;

	Experiment exp(conf);
	exp.setup(&CartPole::eval);
	exp.run();

	dlog("Task completed, exiting.");

	return 0;
}
