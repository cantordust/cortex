#include "Env.hpp"
#include "Net.hpp"
#include "Species.hpp"
#include "Node.hpp"
#include "Conf.hpp"
#include "Data.hpp"

namespace Cortex
{
	bool EventComp::operator () (const Event& _lhs, const Event& _rhs) const noexcept
	{
		return _lhs.time > _rhs.time;
	}

	Net::Net(const uint _id,
			 const SpcPtr& _spc,
			 Env& _env)
		:
		  id(_id),
		  age(1),
		  conf(_env.conf),
		  env(_env),
		  spc(_spc),
		  rfield(*this),
		  fitness(_env.conf)
	{
		/// Add the network to its species.
		if (spc)
		{
			spc->add_net(*this);
		}
	}

	Net::~Net()
	{
		disconnect();
	}

	const NodePtr Net::get_node(const NodeID& _id)
	{
		return nodes.at(_id.role).at(_id.idx - 1)->id != _id ? nullptr
															 : nodes.at(_id.role).at(_id.idx - 1);
	}

	const std::vector<NodePtr>& Net::get_nodes(const NR _nr)
	{
		return nodes[_nr];
	}

	uint Net::get_node_count() const
	{
		uint sum(0);
		for (const auto& node : nodes)
		{
			sum += node.second.size();
		}
		return sum;
	}

	uint Net::get_node_count(const NR _role)
	{
		return nodes[_role].size();
	}

	const NodePtr Net::get_rnd_node(const NR _role)
	{
		if (_role == NR::Undef)
		{
			NR nr(NR::Undef);
			do
			{
				nr = conf.rnd_key(nodes);
			} while (nodes[nr].empty());

			return conf.rnd_elem(nodes[nr]);
		}
		else if (nodes[_role].empty())
		{
			return nullptr;
		}
		return conf.rnd_elem(nodes.at(_role));
	}

	real Net::get_fitness() const
	{
		return fitness.abs.last;
	}

	void Net::set_fitness(const real _val)
	{
		/// The default is to set the absolute fitness
		fitness.abs.update(_val);
		env.inc_evals();
		if (fitness.is_solved())
		{
			mark_solved();
		}
	}

	bool Net::is_old() const
	{
		return (conf.net.max.age > 0 && age > conf.net.max.age);
	}

	void Net::reset_marks()
	{
		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				node->mark = NodeMark::Undef;
			}
		}
	}

	template<>
	void Net::connect<Layout::Random>()
	{
		//		dlog() << "Network " << id << ": connecting...";
		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				node->connect();
			}
		}

		/// Create the evaluation graph
		make_graph();
	}

	template<>
	void Net::connect<Layout::Layered>()
	{
		//		dlog() << "Network " << id << ": connecting...";
		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				node->connect();
			}
		}

		/// Create the evaluation graph
		make_graph();
	}

	void Net::disconnect()
	{
		rfield.clear();
		spc->erase_net(id);

		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				node->disconnect();
			}
		}
	}

	void Net::make_graph(const bool _update)
	{
//				dlog() << "Network " << id << ": creating graph...";

		if (_update)
		{
			graph.clear();
		}

		reset_marks();

		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				if (node->mark == NodeMark::Undef &&
					!node->visit(_update))
				{
					dlog() << "Net::make_graph(): Failed to make graph for network "
						   << id << ", exiting...\n" << *this;
					exit(EXIT_FAILURE);
				}
			}
		}

		// The graph is constructed backwards,
		// so we need to reverse it.
		if (_update &&
			!graph.empty())
		{
			std::reverse(graph.begin(), graph.end());
		}
	}

	void Net::mark_solved()
	{
		env.mark_solved(id);
	}

	real Net::get_saturation()
	{
		return get_link_count() / (std::pow(get_node_count(), 2) - get_node_count());
	}

	uint Net::get_link_count() const
	{
		uint sum(0);
		for (const auto& role : nodes)
		{
			for (const auto& node : role.second)
			{
				sum += node->tgt_count();
			}
		}
		return sum;
	}

	json Net::to_json()
	{
		json j;

		/// @todo populate json
		return j;
	}

	std::ostream& operator<< (std::ostream& _strm, const Net& _net)
	{
		_strm << "\n---------------- Network ----------------\n";

		_strm << "ID: " << _net.id << "\n"
			  << "Age: " << _net.age << "\n"
			  << "Species: " << _net.spc->id << "\n"
			  << "Absolute fitness: " << _net.fitness.abs.last << "\n"
			  << "Relative fitness: " << _net.fitness.rel.last << "\n";

		_strm << "Evaluation order: ";

		for (const auto& node : _net.graph)
		{
			_strm << node->id << " ";
		}

		_strm << "\n\n";

		std::vector<NR> order({NR::O, NR::H, NR::I, NR::B});

		for (const NR role : order)
		{
			if (_net.nodes.find(role) != _net.nodes.end())
			{
				for (const auto& node : _net.nodes.at(role))
				{
					_strm << *node;
				}
			}
		}

		_strm << "\nTotal nodes: " << _net.get_node_count()
			  << "\n--------------------------------------\n\n";

		return _strm;
	}

	///=========================================
	/// Dispatch functions
	///=========================================

	void Net::init()
	{
		if (spc)
		{
			/// Create the phenome by adding
			/// nodes to the network
			for (const auto& role : spc->get_genome())
			{
				insert_nodes(role.first, role.second);
			}
		}

		/// Initialise the receptive fields
		rfield.init();

		/// Connect the network
		connect();
	}

	void Net::connect()
	{
		switch (conf.net.layout)
		{
		case Layout::Layered:
			return connect<Layout::Layered>();

		case Layout::Random:
			return connect<Layout::Random>();

		default:
			return;
		}
	}
}
