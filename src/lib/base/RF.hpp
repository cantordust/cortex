#ifndef RF_HPP
#define RF_HPP

#include "Config.hpp"

namespace Cortex
{
	class Receptor
	{
	private:

		RF rf;



	public:

		Receptor(const RF _rf = RF::Undef)
			:
			  rf(_rf)
		{}

		inline real get_delay(const real _input)
		{
			switch (rf)
			{
			case RF::GRF:
				return
			}

			return 0.0;
		}

	};
}

#endif // RF_HPP
