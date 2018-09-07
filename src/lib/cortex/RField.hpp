#ifndef CORTEX_RFIELD_HPP
#define CORTEX_RFIELD_HPP

#include "Globals.hpp"
#include "GRF.hpp"
//#include "ARF.hpp"

namespace Cortex
{
	/// @class RField (receptive field) class.
	/// Receives input and optionally passes it through
	/// a conversion or preprocessing function.
	class RField
	{
	private:

		/// Receptive field type.
		RFType rf;

		/// GRF
		GRF grf;

	public:

		template<RFType rf = RFType::Undef>
		void init();

		template<RFType rf = RFType::Undef>
		void convert(const Sample& _sample) const;

		void clear();
	};
}

#endif // CORTEX_RFIELD_HPP
