#include "Conf.hpp"
#include "Genotype.hpp"

namespace Cortex
{
	Genotype::Genotype(const Conf& _conf)
	{
		for (const auto& nr : _conf.node.roles)
		{
			genome[nr.first] = nr.second;
		}
	}

	bool Genotype::add(const NR _nr, const uint _count)
	{
		if (_count > 0)
		{
			genome[_nr] += _count;
		}
		return true;
	}

	bool Genotype::set(const NR _nr, const uint _count)
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

	bool Genotype::erase(const NR _nr, const uint _count)
	{
		if (genome.find(_nr) == genome.end() ||
			genome.at(_nr) < _count)
		{
			return false;
		}
		genome.at(_nr) -= _count;
		return true;
	}

	uint Genotype::count(const NR _nr)
	{
		return genome.at(_nr);
	}

	const hmap<NR, uint>& Genotype::get_genome() const
	{
		return genome;
	}

	bool Genotype::operator == (const Genotype& _other)
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
