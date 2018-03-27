#include "eval.hpp"

int main( int argc, char* argv[] )
{
	TCLAP::CmdLine cmd( "Dummy experiment", ' ', Cortex::version, true );
	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config.json", "string", cmd );
	cmd.parse( argc, argv );

	Conf conf(config_file.getValue());
	if (!Dummy::setup(conf))
	{
		return 0;
	}

	Experiment exp(conf);
	exp.setup(&Dummy::eval);
	exp.run();

	dlog() << "Experiment completed, exiting.";

	return 0;
}

