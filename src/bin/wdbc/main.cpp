#include "eval.hpp"

using namespace WDBC;

int main( int argc, char* argv[] )
{
	TCLAP::CmdLine cmd( "Wisconsin Diagnostic Breast Cancer (WDBC) classification task", ' ', Cortex::version, true );
	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config.json", "string", cmd );
	cmd.parse( argc, argv );

	Conf conf(config_file.getValue());
	if (!setup(conf))
	{
		dlog("Setup failed!");
		return 0;
	}

	test(conf);
	return 0;

	Experiment exp(conf);
	exp.setup(&WDBC::eval);
	exp.run();

	dlog("Task completed, exiting.");
	return 0;
}
