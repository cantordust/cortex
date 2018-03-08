#include "eval.hpp"

using namespace CartPole;

int main( int argc, char* argv[] )
{
	TCLAP::CmdLine cmd( "Cart pole balancing experiment", ' ', Cortex::version, true );
	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config.json", "string", cmd );
	cmd.parse(argc, argv);

	Conf config(config_file.getValue());
	if (!setup(config))
	{
		return 0;
	}

	test();
	return 0;

	Experiment cart_pole_exp;
	cart_pole_exp.setup(config, &CartPole::eval);
	cart_pole_exp.run();

	dlog() << "Experiment completed, exiting.";

	return 0;
}
