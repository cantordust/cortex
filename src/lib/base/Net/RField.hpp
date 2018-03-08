#ifndef CORTEX_RFIELD_HPP
#define CORTEX_RFIELD_HPP

#include "Globals.hpp"
#include "GRF.hpp"

namespace Cortex
{
	/// @class RField (receptive field) class.
	/// Receives input and optionally passes it through
	/// a conversion or preprocessing function.
	class RField
	{
	private:

		/// Containing network
		Net& net;

		/// Receptive field type.
		RF rf;

		/// GRF
		std::vector<GRF> grf;

		/// Nodes for corresponding variables
		std::vector<std::vector<NodePtr>> nodes;

	public:

		RField(Net& _net);

		void init();

		void convert(const Sample& _sample) const;

		void convert(const std::vector<std::vector<real>>& _input) const;

		void convert(const std::vector<real>& _input) const;

		void convert(std::vector<std::vector<real>>&& _input);

		void convert(std::vector<real>&& _input);

		void clear();

	private:

		template<RF rf>
		void init();

		template<NT nt = NT::Undef, RF rf = RF::Undef>
		void convert(const std::vector<std::vector<real>>& _input) const;

		template<NT nt = NT::Undef, RF rf = RF::Undef>
		void convert(std::vector<std::vector<real>>&& _input);

	};
}

#endif // CORTEX_RFIELD_HPP
