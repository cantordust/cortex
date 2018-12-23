#ifndef CORTEX_LINK_HPP
#define CORTEX_LINK_HPP

#include "Globals.hpp"

namespace Cortex
{
	class Link : Serialiser
	{
	protected:

		/// Link type (cf. Enum.hpp)
		LinkType type = LinkType::Undef;

		/// Synaptic weight.
//		Parameter weight;

		real weight;

	public:

		///=====================================
		/// Constructors and destructors
		///=====================================

		/// @brief Constructor with explicit link type.
		Link(const LinkType _type, const opt<real> _weight = nopt);

		/// @brief Copy constructor.
		Link(const Link& _other);

		/// @brief Deleted default constructor.
		Link() = delete;

		/// @brief Deleted move constructor.
		Link(Link&& _other) = delete;

		///=====================================
		/// Operators
		///=====================================

		friend bool operator == (const Link& _lhs, const Link& _rhs);

		friend bool operator != (const Link& _lhs, const Link& _rhs);

		///=====================================
		/// Printing and (de)serialisation
		///=====================================

		/// @brief Print to a stream.
		friend os& operator << (os& _os, const Link& _link);

		/// @brief Load from JSON.
		virtual void load(const json& _j) override;

		/// @brief Save to JSON.
		virtual void save(json& _j) const override;

	private:

		///=====================================
		/// STDP
		///=====================================

		/// @brief Long-term potentiation.
		void ltp(const real _dw);

		/// @brief Long-term depression.
		void ltd(const real _dw);

		///=====================================
		/// Friends
		///=====================================

		friend class Net;
		friend class Layer;
		friend class Node;
		friend class Model;
	};
}

#endif // CORTEX_LINK_HPP
