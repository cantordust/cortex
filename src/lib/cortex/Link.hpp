#ifndef CORTEX_LINK_HPP
#define CORTEX_LINK_HPP

#include "Parameter.hpp"

namespace Cortex
{
	template<>
	class Element<ElemType::Link>
	{

	public:

		LinkType type = LinkType::Undef;

		/// Age of the link.
		uint age = 0;

	protected:

		/// Synaptic weight.
		Parameter weight;

	public:

		/// @brief Constructor with explicit link type.
		Element(const LinkType _type, const opt<real> _weight = nopt);

		/// @brief Copy constructor.
		Element(const Link& _other);

		/// @brief Deleted default constructor.
		Element() = delete;

		/// @brief Deleted move constructor.
		Element(Link&& _other) = delete;

		/// @brief Deleted assignment operator.
		Element& operator = (const Link& _other) = delete;

		friend os& operator << (os& _os, const Link& _link);

		friend bool operator == (const Link& _lhs, const Link& _rhs);

		friend bool operator != (const Link& _lhs, const Link& _rhs);

	private:

		/// @brief Long-term potentiation.
		void ltp(const real _dw);

		/// @brief Long-term depression.
		void ltd(const real _dw);

		///=====================================
		/// Friends
		///=====================================

		friend class Net;
		friend class Evaluator;
		template<ElemType> friend class Element;
	};
}

#endif // CORTEX_LINK_HPP
