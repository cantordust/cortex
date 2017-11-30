#ifndef GENOTYPE_HPP
#define GENOTYPE_HPP

#include "Config.hpp"

namespace Cortex
{
	class Genotype
	{
	private:

		emap<NR, uint> genome;

	public:

		Genotype(const emap<NR, uint>& _genome);

		bool add(const NR _nr, const uint _count = 1);

		bool set(const NR _nr, const uint _count);

		bool erase(const NR _nr, const uint _count = 1);

		uint count(const NR _nr);

		const auto& get_genome() const;

		bool operator == (const Genotype& _other);
	};

}
#endif // GENOTYPE_HPP
