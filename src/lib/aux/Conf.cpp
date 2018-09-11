#include "Conf.hpp"
#include "Node.hpp"
#include "Data.hpp"
#include "Functions.hpp"

namespace Cortex
{
	///=============================================================================
	///	Parameter configuration
	///=============================================================================

	real ParamConf::initialise()
	{
		real value(init.value);
		switch (init.dist)
		{
		case Dist::Normal:
			value = rnd_nd(init.value, init.sd);
			break;

		case Dist::Uniform:
			value = rnd_real(lbound, ubound);
			break;

		case Dist::PosNormal:
			value = rnd_pos_nd(init.sd);
			break;

		case Dist::NegNormal:
			value = rnd_neg_nd(init.sd);
			break;

		default:
			break;
		}

		/// Check bounds
		if (value < lbound ||
			value > ubound)
		{
			value = rnd_real(lbound, ubound);
		}

		return value;
	}

	void ParamConf::load(json& _j)
	{
		JLOAD(_j, init.dist);
		JLOAD(_j, init.value);
		JLOAD(_j, init.sd);
		JLOAD(_j, mutation.sd);
		JLOAD(_j, lbound);
		JLOAD(_j, ubound);
	}

	void ParamConf::save(json& _j) const
	{
		JSAVE(_j, init.dist);
		JSAVE(_j, init.value);
		JSAVE(_j, init.sd);
		JSAVE(_j, mutation.sd);
		JSAVE(_j, lbound);
		JSAVE(_j, ubound);
	}

	void from_json(const json& _j, ParamConf& _conf)
	{
		_conf.load(const_cast<json&>(_j));
	}

	void to_json(json& _j, const ParamConf& _conf)
	{
		_conf.save(_j);
	}

	///=============================================================================
	///	Layer configuration
	///=============================================================================

	void LayerConf::load(json& _j)
	{
		JLOAD(_j, evolvable);
		JLOAD(_j, type);
		JLOAD(_j, nodes);
	}

	void LayerConf::save(json& _j) const
	{
		JSAVE(_j, evolvable);
		JSAVE(_j, type);
		JSAVE(_j, nodes);
	}

	bool operator == (const LayerConf& _lhs, const LayerConf& _rhs)
	{
		return (_lhs.type == _rhs.type &&
				_lhs.nodes == _rhs.nodes);
	}

	bool operator != (const LayerConf& _lhs, const LayerConf& _rhs)
	{
		return !(_lhs == _rhs);
	}

	void from_json(const json& _j, LayerConf& _conf)
	{
		_conf.load(const_cast<json&>(_j));
	}

	void to_json(json& _j, const LayerConf& _conf)
	{
		_conf.save(_j);
	}

	///=============================================================================
	///	Main configuration
	///=============================================================================

	void Conf::load()
	{
		dlog("Loading configuration...");

		try
		{
			parse(file, root);
		}
		catch (json::exception& _e)
		{
			die("Error parsing JSON file '", file, "'.\n", _e.what());
		}

		///=================
		/// Net
		///=================

		JLOAD(root, net.type);
		JLOAD(root, net.init.count);
		JLOAD(root, net.init.layers);
		JLOAD(root, net.max.count);
		JLOAD(root, net.max.age);
		JLOAD(root, net.rf.type);
		JLOAD(root, net.rf.adaptive.alpha);
		JLOAD(root, net.rf.gaussian.nodes);
		JLOAD(root, net.rf.gaussian.threshold);
		JLOAD(root, net.rf.gaussian.width);
		JLOAD(root, net.spike.lif);
		JLOAD(root, net.spike.enc);
		JLOAD(root, net.spike.mod);
		JLOAD(root, net.spike.max.delay);

		///=================
		/// Node
		///=================

		JLOAD(root, node.tf);
		JLOAD(root, node.tau);

		///=================
		/// Link
		///=================

		JLOAD(root, link.types);
		JLOAD(root, link.weight);

		///=================
		/// Species
		///=================

		JLOAD(root, species.enabled);
		JLOAD(root, species.init.count);
		JLOAD(root, species.max.count);

		///=================
		/// Learning
		///=================

		JLOAD(root, learning.mode);
		JLOAD(root, learning.mutation.scale);
		JLOAD(root, learning.stdp.rate);
		JLOAD(root, learning.stdp.dp_ratio);

		///=================
		/// Fitness
		///=================

		JLOAD(root, fitness.stat.alpha);

		///=================
		/// Task
		///=================

		JLOAD(root, task.mode);
		JLOAD(root, task.type);
		JLOAD(root, task.runs);
		JLOAD(root, task.generations);
		JLOAD(root, task.threads);

		///=================
		/// Data
		///=================

		JLOAD(root, data.type);
		JLOAD(root, data.image.order);
		JLOAD(root, data.image.depth);
		JLOAD(root, data.image.height);
		JLOAD(root, data.image.width);
		JLOAD(root, data.image.steps.depth);
		JLOAD(root, data.image.steps.height);
		JLOAD(root, data.image.steps.width);
		JLOAD(root, data.samples);
		JLOAD(root, data.sets);

		/// Various initialisation procedures
		init();

		/// Check the configuration and report any problems
		check();
	}

	void Conf::save()
	{
		///=================
		/// Net
		///=================

		JSAVE(root, net.type);
		JSAVE(root, net.init.count);
		JSAVE(root, net.init.layers);
		JSAVE(root, net.max.count);
		JSAVE(root, net.max.age);
		JSAVE(root, net.rf.type);
		JSAVE(root, net.rf.adaptive.alpha);
		JSAVE(root, net.rf.gaussian.nodes);
		JSAVE(root, net.rf.gaussian.threshold);
		JSAVE(root, net.rf.gaussian.width);
		JSAVE(root, net.spike.lif);
		JSAVE(root, net.spike.enc);
		JSAVE(root, net.spike.mod);
		JSAVE(root, net.spike.max.delay);

		///=================
		/// Node
		///=================

		JSAVE(root, node.tf);
		JSAVE(root, node.tau);

		///=================
		/// Link
		///=================

		JSAVE(root, link.types);
		JSAVE(root, link.weight);

		///=================
		/// Species
		///=================

		JSAVE(root, species.enabled);
		JSAVE(root, species.init.count);
		JSAVE(root, species.max.count);

		///=================
		/// Learning
		///=================

		JSAVE(root, learning.mode);
		JSAVE(root, learning.mutation.scale);
		JSAVE(root, learning.stdp.rate);
		JSAVE(root, learning.stdp.dp_ratio);

		///=================
		/// Fitness
		///=================

		JSAVE(root, fitness.stat.alpha);

		///=================
		/// Task
		///=================

		JSAVE(root, task.mode);
		JSAVE(root, task.type);
		JSAVE(root, task.runs);
		JSAVE(root, task.generations);
		JSAVE(root, task.threads);

		///=================
		/// Data
		///=================

		JSAVE(root, data.type);
		JSAVE(root, data.image.order);
		JSAVE(root, data.image.depth);
		JSAVE(root, data.image.height);
		JSAVE(root, data.image.width);
		JSAVE(root, data.image.steps.depth);
		JSAVE(root, data.image.steps.height);
		JSAVE(root, data.image.steps.width);
		JSAVE(root, data.samples);
		JSAVE(root, data.sets);

		std::ofstream ofs(file);

		if (ofs.is_open())
		{
			ofs << root.dump(4);
		}
	}

	void Conf::check()
	{
		///=================
		/// Net
		///=================

		check(net.type != NetType::Undef, "[Net] Invalid network type");
		check(net.init.layers.size() >= 2, "[Net] Please specify at least two layers (input and output)");

		/// Disable irrelevant parameter and element mutations
		if (net.type == NetType::Spiking)
		{
			/// Restrict the absolute magnitude
			/// of link weights to 1.
			link.weight.lbound = -1.0;
			link.weight.ubound = 1.0;

			check(net.spike.max.delay > 0.0, "[Net] Max spike delay must be > 0");
		};

		for (uint i = 0; i < net.init.layers.size(); ++i)
		{
			check(net.init.layers[i].nodes > 0, "[Layer ", i + 1, "] Invalid layer size");
		}

		///=================
		/// Node
		///=================

		check(!node.tf[LayerType::Regular].empty(), "[Node] Please specify at least one transfer function for nodes in regular layers");
		if (net.type == NetType::Convolutional)
		{
			check(!node.tf[LayerType::Convolutional].empty(), "[Node] Please specify at least one transfer function for nodes in convolutional layers");
		}

		///=================
		/// Link
		///=================

		/// Forward links cannot be disabled.
		/// Just insert the forward type to be sure.
		link.types.insert(LinkType::Forward);

		///=================
		/// Species
		///=================

		if (species.enabled)
		{
			check(species.init.count > 0, "[Species] Initial number of species count must be > 0");
			check(species.max.count > 0, "[Species] Maximal number of species count must be > 0");
		}
		else
		{
			///	In most cases, the code explicitly checks
			/// whether speciation is enabled, but this
			/// provides extra insurance that there will
			/// be only one (global) species if speciation
			/// is disabled.
			///
			/// In addition, the initial count is essential
			/// for computing the right network quota for
			/// the global species if speciation is disabled
			/// (cf. Env::initialise()).
			species.init.count = 1;
			species.max.count = 1;
		}

		///=================
		/// Learning
		///=================

		if (learning.mode == LearningMode::Mutation)
		{
			check(learning.mutation.scale > 0.0 && learning.mutation.scale < 1.0,  "[Learning] Mutation SD scale must be > 0 and < 1");
		}
		if (learning.mode == LearningMode::STDP)
		{
			check(learning.stdp.rate > 0.0 && learning.stdp.rate < 1.0,  "[Learning] STDP learning rate must be > 0 and < 1");
		}

		///=================
		/// Fitness
		///=================

		check(fitness.stat.alpha > 0.0, "[Fitness] Forgetting rate must be > 0");

		///=================
		/// Task
		///=================

		check(task.runs > 0, "[Task] Number of runs must be > 0");
		check(task.generations > 0, "[Task] Max. number of epochs must be > 0");
		check(task.threads > 0, "[Task] Number of threads must be > 0");

		///=================
		/// Data
		///=================

		if (data.type == DataType::Image)
		{
			check(data.image.height > 0, "[Data] Invalid image height");
			check(data.image.width > 0, "[Data] Invalid image width");
			check(data.image.depth > 0, "[Data] Invalid image depth");
		}

		/// Ensure that the data sets are not empty.
		if (data.sets.is_empty())
		{
			data.sets[Stage::Train] = 1.0;
		}

		/// Report problems
		if (!problems.str().empty())
		{
			die("Configuration errors found:\n", problems.str());
		}
	}

	void Conf::init()
	{
		/// Initialise the transfer functions
		/// and their derivatives.
		Transfer::init();

		if (data.type == DataType::Image)
		{
			/// Compute the order of image element access
			/// according to the data.image.order parameter
			data.image.compute_steps();
		}
	}

	///=============================================================================
	///	Configuration printing
	///=============================================================================

	///=====================================
	/// Category name
	///=====================================

	std::string cat(const std::string& _name, const bool _enabled = true)
	{
		return "\n\n=======[ " + _name + (_enabled ? "" : " (disabled)" ) + " ]=======\n";
	}

	///=====================================
	/// Layer
	///=====================================

	os& operator << (os& _os, const LayerConf& _conf)
	{
		return _os << "\n------[ Layer ]------"
				   << "\nEvolvable: " << std::boolalpha << _conf.evolvable
				   << "\nType: " << _conf.type
				   << "\nNodes: " << _conf.nodes
				   << "\n---------------------";
	}

	///=====================================
	/// Parameter
	///=====================================

	os& operator << (os& _os, const ParamConf& _conf)
	{
		_os << "\n\tInit. distribution: " << _conf.init.dist
			<< "\n\tInit. value: " << _conf.init.value
			<< "\n\tInit. SD: " << _conf.init.sd
			<< "\n\tMutation SD: " << _conf.mutation.sd
			<< "\n\tLower bound: " << _conf.lbound
			<< "\n\tUpper bound: " << _conf.ubound;

		return _os;
	}

	///=====================================
	/// Net
	///=====================================

	os& operator << (os& _os, const Conf::Net& _conf)
	{
		_os << cat("Net")
			<< "\nType: " << _conf.type
			<< "\nInit. count: " << _conf.init.count
			<< "\nInit. layers:\n";

		for (const auto& layer : _conf.init.layers)
		{
			_os << layer;
		}

		_os << "\n\nMax count: " << _conf.max.count
			<< "\nMax age: " << _conf.max.age;

		_os << "\nReceptive field:"
			<< "\nType: " << _conf.rf.type;

		if (_conf.rf.type == RFType::Adaptive)
		{
			_os << "\n\tAlpha: " << _conf.rf.adaptive.alpha;
		}
		else if (_conf.rf.type == RFType::Gaussian)
		{
			_os << "\n\tNodes: " << _conf.rf.gaussian.nodes
				<< "\n\tCutoff: " << _conf.rf.gaussian.threshold
				<< "\n\tBeta: " << _conf.rf.gaussian.width;
		}

		if (_conf.type == NetType::Spiking)
		{
			_os << "\nSpike configuration:"
				<< "\n\tLIF: " << _conf.spike.lif
				<< "\n\tEncoding: " << _conf.spike.enc
				<< "\n\tModifier: " << _conf.spike.mod
				<< "\n\tMax. delay: " << _conf.spike.max.delay;
		}

		return _os;
	}

	///=====================================
	/// Node
	///=====================================

	os& operator << (os& _os, const Conf::Node& _conf)
	{
		_os << cat("Node")
			<< "\nTransfer functions: " << _conf.tf;
		if (conf->net.type == NetType::Spiking)
		{
			_os << "\nTime constant:\n" << _conf.tau;
		}

		return _os;
	}

	///=====================================
	/// Link
	///=====================================
	os& operator << (os& _os, const Conf::Link& _conf)
	{
		_os << cat("Link")
			<< "\nTypes: " << _conf.types
			<< "\nWeight:\n" << _conf.weight;

		return _os;
	}

	///=====================================
	/// Species
	///=====================================

	os& operator << (os& _os, const Conf::Species& _conf)
	{
		_os << cat("Species", _conf.enabled);

		if (_conf.enabled)
		{
			_os << "\nInit. count: " << _conf.init.count
				<< "\nMax count: " << _conf.max.count;
		}

		return _os;
	}

	///=====================================
	/// Learning
	///=====================================

	os& operator << (os& _os, const Conf::Learning& _conf)
	{
		_os << cat("Learning")
			<< "\nMode: " << _conf.mode;

		if (_conf.mode == LearningMode::Mutation)
		{
			_os << "\nSD scaling factor: " << _conf.mutation.scale;
		}

		if (_conf.mode == LearningMode::STDP)
		{
			_os << "\nLearning rate: " << _conf.stdp.rate
				<< "\nD/P ratio: " << _conf.stdp.dp_ratio;
		}

		return _os;
	}

	///=====================================
	/// Fitness
	///=====================================

	os& operator << (os& _os, const Conf::Fitness& _conf)
	{
		_os << cat("Fitness")
			<< "\nForgetting rate: " << _conf.stat.alpha;

		return _os;
	}

	///=====================================
	/// Task
	///=====================================

	os& operator << (os& _os, const Conf::Task& _conf)
	{
		_os << cat("Task")
			<< "\nTask mode: " << _conf.mode
			<< "\nRuns: " << _conf.runs
			<< "\nEpochs: " << _conf.generations
			<< "\nThreads: " << _conf.threads;

		return _os;
	}

	///=====================================
	/// Data
	///=====================================

	os& operator << (os& _os, const Conf::Data& _conf)
	{
		_os << cat("Data")
			<< "\nType: " << _conf.type;

		if (_conf.type == DataType::Image)
		{
			_os << "\nImage parameters:\n"
				<< "\n\tHeight: " << _conf.image.height
				<< "\n\tWidth: " << _conf.image.width
				<< "\n\tDepth: " << _conf.image.depth
				<< "\nOrder: " << _conf.image.order
				<< "\nStep parameters:\n"
				<< "\n\tHeight: " << _conf.image.steps.height
				<< "\n\tWidth: " << _conf.image.steps.width
				<< "\n\tDepth: " << _conf.image.steps.depth;
		}

		_os << "\nSamples: " << _conf.samples
			<< "\nSets:" << _conf.sets;

		return _os;
	}

	///=====================================
	/// Main configuration
	///=====================================

	os& operator << (os& _os, const Conf& _conf)
	{
		_os << std::boolalpha
			<< "\n" << std::string(37, '=')
			<< "\n" << std::string(12, ' ')
			<< "Configuration"
			<< "\n" << std::string(37, '=')
			<< _conf.net
			<< _conf.node
			<< _conf.link
			<< _conf.species
			<< _conf.learning
			<< _conf.fitness
			<< _conf.task
			<< _conf.data;

		return _os << "\n";
	}

	///=============================================================================
	///	Subconfiguration methods
	///=============================================================================

	void Conf::Data::Image::compute_steps()
	{
		switch (order)
		{
		case DimOrder::DHW:
			steps.depth = 1;
			steps.height = depth;
			steps.width = height * depth;
			break;

		case DimOrder::DWH:
			steps.depth = 1;
			steps.height = width * depth;
			steps.width = depth;
			break;

		case DimOrder::HDW:
			steps.depth = height;
			steps.height = 1;
			steps.width = height * depth;
			break;

		case DimOrder::HWD:
			steps.depth = height * width;
			steps.height = 1;
			steps.width = height;
			break;

		case DimOrder::WDH:
			steps.depth = width;
			steps.height = width * depth;
			steps.width = 1;
			break;

		case DimOrder::WHD:
			steps.depth = height * width;
			steps.height = width;
			steps.width = 1;
			break;

		default:
			die("Invalid input order ", order);
		}
	}

	uint Conf::Data::Image::conv_layer_limit()
	{
		/// Each convolutional layer applies 2 x 2 pooling,
		///	so the size of the response layer after
		/// pooling is half of the input.
		/// We can easily compute the maximal number
		/// of convolutional layers in this setup.
		///
		/// @todo: Should this take the maximum instead of the minimum?
		return std::min(std::floor(std::log2(height)), std::floor(std::log2(width)));
	}
}
