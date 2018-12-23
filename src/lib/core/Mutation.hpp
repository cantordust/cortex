#ifndef CORTEX_MUTATION_HPP
#define CORTEX_MUTATION_HPP

#include "Globals.hpp"

namespace Cortex
{
	///=====================================
	///	@struct A mutation event.
	///=====================================
	struct Mutation
	{
		ElemType element{ElemType::Undef};
		Action action{Action::Undef};

		struct
		{
			LayerType type{LayerType::Undef};
			uint index{0};
		} layer;

		friend os& operator << (os& _os, const Mutation& _mut)
		{
			return _os << "\nElement: " << _mut.element
					   << "\nAction: " << _mut.action
					   << "\nLayer type: " << _mut.layer.type
					   << "\nLayer index: " << _mut.layer.index
					   << "\n";
		}
	};

} // namespace Cortex

#endif // CORTEX_MUTATION_HPP
