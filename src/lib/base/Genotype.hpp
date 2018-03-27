#ifndef CORTEX_GENOTYPE_HPP
#define CORTEX_GENOTYPE_HPP

#include "Globals.hpp"

namespace Cortex
{
	class Genotype
	{
	private:

		hmap<NR, uint> genome;

	public:

		Genotype(const Conf& _conf);

		bool add(const NR _nr, const uint _count = 1);

		bool set(const NR _nr, const uint _count);

		bool erase(const NR _nr, const uint _count = 1);

		uint count(const NR _nr);

		const hmap<NR, uint>& get_genome() const;

		bool operator == (const Genotype& _other);
	};
}
#endif // CORTEX_GENOTYPE_HPP
