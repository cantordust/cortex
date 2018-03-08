#ifndef CORTEX_DATACONF_HPP
#define CORTEX_DATACONF_HPP

#include "Globals.hpp"

namespace Cortex
{
	class DataConf final
	{

	public:

		Conf& conf;

		bool enabled;

		/// Number of samples to use.
		/// Set to 0 to use all samples.
		/// A nonzero value can be used
		/// to process a small batch
		/// for debugging purposes.
		uint samples;

		/// Respective sizes of the training,
		/// validation and test sets relative
		/// to the entire data set.
		real training;
		real test;
		real validation;

		/// Type of data we are processing
		DataType type;

	public:

		DataConf(Conf& _conf);

		void set_defaults();

		std::string validate();

		friend std::ostream& operator << (std::ostream& _strm, const DataConf& _conf);
	};
}

#endif // CORTEX_DATACONF_HPP
