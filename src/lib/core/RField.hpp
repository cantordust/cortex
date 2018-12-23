#ifndef CORTEX_RFIELD_HPP
#define CORTEX_RFIELD_HPP

#include "Globals.hpp"
#include "GRF.hpp"
//#include "ARF.hpp"

namespace Cortex
{
	///=============================================================================
	/// @class Receptive field.
	///=============================================================================

	class RField
	{
	private:

		Node& node;

		sz size{1};

//		Dim size;

//		/// GRF
//		std::vector<GRF> grf;

		/// @todo ARF
		/// @todo Spatiotemporal

	public:

		RField(Node& _node);

		template<SensorType rf = SensorType::Undef>
		void init() {};

		void clear();

	private:

		friend class Model;
		friend class Vertex;
	};
}

#endif // CORTEX_RFIELD_HPP
