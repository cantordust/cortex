#include "eval.hpp"

int main( int argc, char* argv[] )
{
	TCLAP::CmdLine cmd( "XOR experiment", ' ', Cortex::version, true );
	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config.json", "string", cmd );
	cmd.parse( argc, argv );

	Conf conf(config_file.getValue());

	dlog() << conf;

	if (!Xor::setup(conf))
	{
		return 0;
	}

	Experiment xor_exp;
	xor_exp.setup(conf, &Xor::eval);
	xor_exp.run();

	dlog() << "Experiment completed, exiting.";

	return 0;
}

