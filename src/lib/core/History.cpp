#include "History.hpp"

namespace Cortex
{
	///=====================================
	/// Archive struct
	///=====================================

	void History::Record::add(const uint _val)
	{
		archive.emplace_back(_val);
		stats.update(_val);
	}

	///=====================================
	/// Constructors and destructors
	///=====================================

	History::~History()
	{
		if (!closed)
		{
			close();
		}
	}

	void History::new_run()
	{
		if (!closed)
		{
			archive[archive.size() + 1] = {};
		}
	}

	void History::add(const Stat _stat, const uint _val)
	{
		if (!closed)
		{
			archive[archive.size()][_stat].add(_val);
		}
	}

	void History::close()
	{
		for(const auto& run : archive)
		{
			for (const auto& stat : run.second)
			{
				stats[stat.first].update(stat.second.archive.back());
			}
		}
		closed = true;
	}

	const hmap<uint, hmap<Stat, History::Record>>& History::get()
	{
		return archive;
	}

	///=====================================
	/// Printing and (de)serialisation
	///=====================================

	void History::print()
	{
		/// Stats report
		dlog report;

		std::string header("--------------------[ Task statistics ("
						   + std::to_string(archive.size())
						   + " run"
						   + (archive.size() == 1 ? "" : "s")
						   + ") ]--------------------");
		std::vector<std::string> fields{"Variable", "Mean", "SD"};
		std::vector<uint> width;
		uint total(std::accumulate(fields.cbegin(),
								   fields.cend(),
								   0,
								   [](const uint _sum, const std::string& _field)
		{
			return _sum + _field.size();
		}));

		/// Compute the field sizes.
		for (const auto& f : fields)
		{
			width.emplace_back((f.size() / flt(total)) * header.size() - 2);
		}

		/// Header
		report + "\n" + header + "\n";

		/// Justify left and set the filler to ' '.
		report.left().setfill(' ');

		/// Print stat fields
		for (uint f = 0; f < fields.size(); ++f)
		{
			report.add(" ").format(fields[f], width[f]).add((f < fields.size() - 1 ? "|" : ""));
		}
		report + "\n" + std::string(header.size(), '-');

		for (const auto& stat : stats)
		{
			uint idx(0);
			report.add("\n ").format(pretty(stat.first), width[idx]);
			report.add("| ").format(stat.second.mean, width[++idx]);
			report.add("| ").format(stat.second.sd(), width[++idx]);
		}

		/// Bottom line
		report + "\n" + std::string(header.size(), '-') + "\n";
	}

	void History::load(const json& _j)
	{

	}

	void History::save(json& _j) const
	{

	}



} // namespace Cortex
