#ifndef CORTEX_RFIELD_HPP
#define CORTEX_RFIELD_HPP

#include "Globals.hpp"
#include "GRF.hpp"
//#include "ARF.hpp"

namespace Cortex
{
	/// @class RField (receptive field) class.
	/// Receives input samples and optionally
	/// applies a conversion or preprocessing function.
	class RField
	{
	private:

		/// The evaluator for this receptive field
		Evaluator& evaluator;

		/// GRF
		std::vector<GRF> grf;

		/// @todo ARF
		/// @todo Spatiotemporal

	public:

		RField(Evaluator& _evaluator);

		template<RFType rf = RFType::Undef>
		void init() {};

		template<RFType rf = RFType::Undef>
		void convert(const Sample& _sample) const;

		Mat stretch(const Mat& _image, const uint _max_fs);

		void clear();
	};
}

#endif // CORTEX_RFIELD_HPP
