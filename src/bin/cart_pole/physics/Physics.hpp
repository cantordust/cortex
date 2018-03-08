#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Track.hpp"

namespace CartPole
{
	namespace Gen
	{
		enum class Var : uchar
		{
			pos,
			theta,
			vel,
			omega
		};
		static std::vector<real> pos;
		static std::vector<real> theta;
		static std::vector<real> vel;
		static std::vector<real> omega;
		static std::vector<Var> vars;
		static std::vector<Track> tracks;
	};
}

#endif // PHYSICS_HPP
