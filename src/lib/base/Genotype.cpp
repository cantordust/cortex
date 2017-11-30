#include "Genotype.hpp"

namespace Cortex
{
	Genotype::Genotype(const emap<NR, uint>& _genome)
	{
		for (const auto& nr : Enum<NR>::entries)
		{
			if (_genome.find(nr.first) != _genome.end())
			{
				genome[nr.first] = _genome.at(nr.first);
			}
			else
			{
				genome[nr.first] = 0;
			}
		}
	}

	inline bool Genotype::add(const NR _nr, const uint _count)
	{
		if (_count > 0)
		{
			genome[_nr] += _count;
		}
		return true;
	}

	inline bool Genotype::set(const NR _nr, const uint _count)
	{
		if (_count == 0)
		{
			genome.erase(_nr);
		}
		else
		{
			genome[_nr] = _count;
		}
		return true;
	}

	inline bool Genotype::erase(const NR _nr, const uint _count)
	{
		if (genome.find(_nr) == genome.end() ||
			genome.at(_nr) < _count)
		{
			return false;
		}
		genome.at(_nr) -= _count;
		return true;
	}

	inline uint Genotype::count(const NR _nr)
	{
		return genome.at(_nr);
	}

	inline const auto& Genotype::get_genome() const
	{
		return genome;
	}

	inline bool Genotype::operator == (const Genotype& _other)
	{
		for (const auto& gene : genome)
		{
			if (_other.genome.find(gene.first) == _other.genome.end() ||
				gene.second != _other.genome.at(gene.first))
			{
				return false;
			}
		}
		return true;
	}
}
