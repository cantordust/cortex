#include "eval.hpp"

int main( int argc, char* argv[] )
{
	TCLAP::CmdLine cmd( "Dummy experiment", ' ', version, false );
	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config.json", "string", cmd );
	cmd.parse( argc, argv );

	Config config(config_file.getValue());
	if (!Dummy::setup(config))
	{
		return 0;
	}

	Experiment dummy_exp;
	dummy_exp.add(config, &Dummy::eval);
	dummy_exp.run();

	dlog() << "Experiment completed, exiting.";

	return 0;
}

