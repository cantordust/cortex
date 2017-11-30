#include "Cortex.hpp"

using namespace Cortex;

uint check_fitness( std::map<uint, std::string>& _classes,
					std::map<uint, std::string>& _outputs )
{
	uint fitness(0);

	for ( const std::pair<uint, std::string>& c : _classes )
	{
		if ( c.second == _outputs[c.first] )
		{
			++fitness;
		}
	}

	return fitness;
}


int main( int argc, char* argv[] )
{
//	TCLAP::CmdLine cmd( "Cortex: Wisconsin Diagnostic Breast Cancer (WDBC) classification task with 9 features", ' ', "version", false );
//	TCLAP::ValueArg<std::string> config_file( "c", "config", "Configuration file", false, "config_9.json", "string", cmd );
//	cmd.parse( argc, argv );
//	std::string entry;
//	std::ifstream data_file( "wdbc_9.data" );
//	std::stringstream ss;

//	if ( data_file )
//	{
//		ss << data_file.rdbuf();
//		data_file.close();
//	}
//	else
//	{
//		std::cout << "The data file is missing! Exiting..." << std::endl;
//		exit( 1 );
//	}

//	std::map<uint, std::vector<real>> data;
//	std::map<uint, std::string> classes;

//	// Gobble the entries from the file and
//	// organise them into tables for easy access
//	while ( ss >> entry )
//	{
//		uint delim = 0;
//		std::string datum;
//		uint id;

//		for ( uint i = 0; i < entry.size(); ++i )
//		{
//			if ( entry.at( i ) == ',' ||
//					i == entry.size() - 1 )
//			{
//				if ( i == entry.size() - 1 )
//				{
//					datum.insert( datum.end(), entry.at( i ) );
//				}

//				++delim;

//				if ( delim == 1 )
//				{
//					id = std::atoi( datum.c_str() );
//				}
//				else if ( delim == 2 )
//				{
//					classes[id] = datum;
//				}
//				else
//				{
//					data[id].push_back( std::atof( datum.c_str() ) );
//				}

//				datum = "";
//			}
//			else
//			{
//				datum.insert( datum.end(), entry.at( i ) );
//			}
//		}
//	}

//	for ( const std::pair<uint, std::string>& entry : classes )
//	{
//		std::cout << "ID: " << entry.first << ", label: " << entry.second << std::endl;

//		for ( uint n = 0; n < data[entry.first].size(); ++n )
//		{
//			if ( n == 0 )
//			{
//				std::cout << "\tClump thickness: " << std::flush;
//			}
//			else if ( n == 1 )
//			{
//				std::cout << "\tUniformity of cell size: " << std::flush;
//			}
//			else if ( n == 2 )
//			{
//				std::cout << "\tUniformity of cell shape: " << std::flush;
//			}
//			else if ( n == 3 )
//			{
//				std::cout << "\tMarginal adhesion: " << std::flush;
//			}
//			else if ( n == 4 )
//			{
//				std::cout << "\tSingle epithelial cell size: " << std::flush;
//			}
//			else if ( n == 5 )
//			{
//				std::cout << "\tBare nuclei: " << std::flush;
//			}
//			else if ( n == 6 )
//			{
//				std::cout << "\tBland chromatin: " << std::flush;
//			}
//			else if ( n == 7 )
//			{
//				std::cout << "\tNormal nucleoli: " << std::flush;
//			}
//			else if ( n == 8 )
//			{
//				std::cout << "\tMitoses: " << std::flush;
//			}

//			std::cout << data[entry.first][n] << std::endl;
//		}
//	}

//	Ecosystem<NetType::Spiking> es;
//	uint experiments( cfg().runs );
//	uint max_age( cfg().ecosystem.max.age );
//	real target_fitness( static_cast<real>( classes.size() ) );
//	uint experiment( 1 );
//	uint successes( 0 );
//	std::vector<uint> evaluations;

//	while ( experiment <= experiments )
//	{
//		es.init();
//		es.set_target_fitness( target_fitness );
//		std::map<uint, std::vector<real>> training_set;
//		std::map<uint, std::vector<real>> test_set;

//		for ( const std::pair<uint, std::vector<real>> datum : data )
//		{
//			if ( rnd_real( 0.0, 1.0 ) < 0.5 )
//			{
//				training_set[datum.first] = datum.second;
//			}
//			else
//			{
//				test_set[datum.first] = datum.second;
//			}
//		}

//		std::cout << "Training set size: " << training_set.size() << std::endl;
//		std::cout << "Test set size: " << test_set.size() << std::endl;
//		bool solution_found = false;

//		while ( true )
//		{
//			std::cout << "------------------------" << std::endl;
//			std::cout << "Generation: " << es.get_age() << std::endl;
//			std::cout << "Species: " << es.get_species_count() << std::endl;
//			std::cout << "Ecosystem size: " << es.get_size() << std::endl;
//			std::map<uint, std::string> outputs;

//			for ( const uint n_id : es.get_net_ids() )
//			{
//				if ( !es.is_evaluated( n_id ) )
//				{
//					for ( uint i = 0; i < 1; ++i )
//					{
//						outputs.clear();

//						for ( const std::pair<uint, std::vector<real>>& datum : training_set )
//						{
//							es.evaluate( n_id, datum.second, true );
//						}
//					}

//					outputs.clear();
//					std::queue<std::pair<real, uint>> spikes;

//					for ( const std::pair<uint, std::vector<real>>& datum : test_set )
//					{
//						spikes = es.evaluate( n_id, datum.second, false );

//						if ( spikes.size() == 0 )
//						{
//							outputs[datum.first] = "NA";
//						}
//						else if ( spikes.front().second == *( es.get_output_neuron_ids().begin() ) )
//						{
//							outputs[datum.first] = "B";
//						}
//						else if ( spikes.front().second == *( es.get_output_neuron_ids().rbegin() ) )
//						{
//							outputs[datum.first] = "M";
//						}
//					}

//					uint fitness = check_fitness( classes, outputs );

//					if ( fitness == test_set.size() )
//					{
//						std::cout << "Champion found!\n";
//						std::cout << &es.get_net( n_id );
//						solution_found = true;
//						break;
//					}

//					es.set_fitness( n_id, fitness );
//				}
//			}

//			if ( solution_found )
//			{
//				++successes;
//				evaluations.push_back( es.get_total_evals() );
//				break;
//			}

//			std::cout << "Highest fitness after generation " << es.get_age()
//					  << ": " << es.get_highest_fitness()
//					  << "/"
//					  << test_set.size()
//					  << " ("
//					  << 100.0 * es.get_highest_fitness() / static_cast<real>( test_set.size() )
//					  << "% accuracy)" << std::endl;

//			// Evolve the ecosystem
//			if ( es.get_age() == max_age )
//			{
//				std::cout << "Total evaluations: " << es.get_total_evals() << "\n";
//				std::cout << "Highest fitness ever: " << es.get_highest_fitness() << "\n";
//				uint fittest_network = es.get_champion();
//				std::cout << "Last champion: " << fittest_network << "\n";
//				evaluations.push_back( es.get_total_evals() );
//				std::cout << &es.get_net( fittest_network );
//				break;
//			}

//			std::cout << "Evolving ecosystem..." << std::endl;
//			es.evolve();
//			std::cout << "------------------------" << std::endl;
//		}

//		++experiment;
//	}

//	std::cout << "Success rate: " << successes << " / " << experiments << std::endl;
//	std::cout << "Average number of evaluations: " << std::accumulate( evaluations.begin(), evaluations.end(), 0.0 ) / static_cast<real>( evaluations.size() ) << std::endl;
	return 0;
}
