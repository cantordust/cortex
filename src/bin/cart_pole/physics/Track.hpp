#ifndef TRACK_HPP
#define TRACK_HPP

#include "Cart.hpp"

namespace CartPole
{
	class Track
	{
	public:

		struct Def
		{
			/// Total track length [m].
			/// Centred at the origin, so cart movement
			/// is limited to (-len/2, len/2)
			static real track_len;

			/// Determines whether the controller is given information
			/// about positions and velocities or just positions
			static bool with_vel;

			/// Coupled oscillator mode.
			/// Only valid if carts > 1.
			/// @todo Implement coupled oscillator mode
			static bool coupled;

			/// Spring constants.
			/// @todo Implement coupled oscillator mode
			static std::vector<real> ks;
		};

	private:

		std::vector<Cart> carts;

	public:

		void init();

		//// @todo Implement coupled oscillator mode
		void add_cart(const Cart& _cart);

		std::vector<Cart>& get_carts();

		void update(const std::vector<real>& _actions);

		std::string as_csv(const std::vector<real>& _v) const;

		real restrict(const real _theta);

		std::vector<real> state();

		std::vector<real> norm_state();

		bool is_in_range();

		uint cart_count();

		uint pole_count();

		friend std::ostream& operator << (std::ostream& _strm, const Track& _env);
	};
}

#endif // TRACK_HPP
