#include "eval.hpp"

int main( int argc, char* argv[] )
{
	TCLAP::CmdLine cmd( "XOR experiment", ' ', version, false );
	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config.json", "string", cmd );
	cmd.parse( argc, argv );

	Config config(config_file.getValue());
	if (!Xor::setup(config))
	{
		return 0;
	}

	Experiment xor_exp;
	xor_exp.add(config, &Xor::eval);
	xor_exp.run();

	dlog() << "Experiment completed, exiting.";

	return 0;
}

