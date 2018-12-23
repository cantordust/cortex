#include "Conf.hpp"
#include "Node.hpp"
#include "Data.hpp"

namespace Cortex
{
	namespace
	{
		/// JSON object holding the parsed configuration.
		inline json root;
	}

//	///=============================================================================
//	///	Subconfiguration methods
//	///=============================================================================

//	void compute_strides(Conf::Data::Image& _image)
//	{
//		/// Order: depth, height, width
//		switch (_image.order)
//		{
//		case DimOrder::DHW:
//			_image.strides = {1, _image.dim.depth(), _image.dim.height() * _image.dim.depth()};
//			break;

//		case DimOrder::DWH:
//			_image.strides = {1, _image.dim.width() * _image.dim.depth(), _image.dim.depth()};
//			break;

//		case DimOrder::HDW:
//			_image.strides = {_image.dim.height(), 1, _image.dim.height() * _image.dim.depth()};
//			break;

//		case DimOrder::HWD:
//			_image.strides = {_image.dim.height() * _image.dim.width(), 1, _image.dim.height()};
//			break;

//		case DimOrder::WDH:
//			_image.strides = {_image.dim.width(), _image.dim.width() * _image.dim.depth(), 1};
//			break;

//		case DimOrder::WHD:
//			_image.strides = {_image.dim.height() * _image.dim.width(), _image.dim.width(), 1};
//			break;

//		default:
//			die("Invalid input order ", _image.order);
//		}
//	}

	///=============================================================================
	///	Initialisation definition
	///=============================================================================

	InitDef::InitDef(const Dist _dist, const real _mean, const real _sd, const real _min, const real _max)
		:
		  dist(_dist),
		  mean(_mean),
		  sd(_sd),
		  min(_min),
		  max(_max)
	{}

	void InitDef::load(const json& _j)
	{
		JLOAD(_j, dist);
		JLOAD(_j, mean);
		JLOAD(_j, sd);
		JLOAD(_j, min);
		JLOAD(_j, max);
	}

	void InitDef::save(json& _j) const
	{
		JSAVE(_j, dist);
		JSAVE(_j, mean);
		JSAVE(_j, sd);
		JSAVE(_j, min);
		JSAVE(_j, max);
	}

	///=============================================================================
	///	Node definition
	///=============================================================================

	LinkDef::LinkDef(const LinkType _type, const opt<real> _weight)
		:
		  type(_type),
		  weight(_weight ? _weight.value() : Conf::init(ParamType::Weight))
	{}

	void LinkDef::load(const json& _j)
	{
		JLOAD(_j, type);
		JLOAD(_j, weight);
	}

	void LinkDef::save(json& _j) const
	{
		JSAVE(_j, type);
		JSAVE(_j, weight);
	}

	bool operator == (const LinkDef& _lhs, const LinkDef& _rhs)
	{
		return (_lhs.type == _rhs.type &&
				_lhs.weight == _rhs.weight);
	}

	bool operator != (const LinkDef& _lhs, const LinkDef& _rhs)
	{
		return !(_lhs == _rhs);
	}

	///=============================================================================
	///	Node definition
	///=============================================================================

	NodeDef::NodeDef(const VCube _dim, const opt<real> _tau)
		:
		  dim(_dim),
		  tau(_tau ? _tau.value() : conf.init(ParamType::TimeConstant))
	{}

	void NodeDef::load(const json& _j)
	{
		JLOAD(_j, dim);
		JLOAD(_j, tau);
	}

	void NodeDef::save(json& _j) const
	{
		JSAVE(_j, dim);
		JSAVE(_j, tau);
	}

	bool operator == (const NodeDef& _lhs, const NodeDef& _rhs)
	{
		return (_lhs.dim == _rhs.dim &&
				_lhs.tau == _rhs.tau);
	}

	bool operator != (const NodeDef& _lhs, const NodeDef& _rhs)
	{
		return !(_lhs == _rhs);
	}

	///=============================================================================
	///	Layer definition
	///=============================================================================

	LayerDef::LayerDef(const LayerType _type, const std::vector<NodeDef>& _nodes, const bool _fixed)
		:
		  type(_type),
		  nodes(_nodes),
		  fixed(_fixed)
	{}

	void LayerDef::load(const json& _j)
	{
		JLOAD(_j, type);
		JLOAD(_j, nodes);
		JLOAD(_j, fixed);
	}

	void LayerDef::save(json& _j) const
	{
		JSAVE(_j, type);
		JSAVE(_j, nodes);
		JSAVE(_j, fixed);
	}

	bool operator == (const LayerDef& _lhs, const LayerDef& _rhs)
	{
		return (_lhs.type == _rhs.type &&
				_lhs.nodes == _rhs.nodes);
	}

	bool operator != (const LayerDef& _lhs, const LayerDef& _rhs)
	{
		return !(_lhs == _rhs);
	}

	///=============================================================================
	///	Main configuration
	///=============================================================================

	std::string Conf::load(const std::string& _file)
	{
//		dlog("Loading configuration...");

		try
		{
			parse(_file, root);
		}
		catch (json::exception& _e)
		{
			die("Error parsing JSON file '", _file, "'.\n", _e.what());
		}

		///=================
		/// Net
		///=================

		JLOAD(root, net.type);
		JLOAD(root, net.init.count);
		JLOAD(root, net.init.layers);
		JLOAD(root, net.max.count);
		JLOAD(root, net.max.age);
		JLOAD(root, net.sensors.type);
		JLOAD(root, net.sensors.adaptive.alpha);
		JLOAD(root, net.sensors.gaussian.cells);
		JLOAD(root, net.sensors.gaussian.threshold);
		JLOAD(root, net.sensors.gaussian.width);
		JLOAD(root, net.spike.lif);
		JLOAD(root, net.spike.enc);
		JLOAD(root, net.spike.mod);
		JLOAD(root, net.spike.max.delay);

		///=================
		/// Layer
		///=================

		JLOAD(root, layer.activations);

		///=================
		/// Node
		///=================

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
		/// Training
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
		JLOAD(root, task.epochs);
		JLOAD(root, task.threads);

		///=================
		/// Data
		///=================

		JLOAD(root, data.type);
		JLOAD(root, data.image.order);
		JLOAD(root, data.image.dim);
		JLOAD(root, data.samples);
		JLOAD(root, data.sets);

//		/// Various initialisation procedures
//		init();

		status = ConfigStatus::Loaded;

		/// Check the configuration and report any problems
		return validate();

	}

	void Conf::save(const std::string& _file)
	{
		///=================
		/// Net
		///=================

		JSAVE(root, net.type);
		JSAVE(root, net.init.count);
		JSAVE(root, net.init.layers);
		JSAVE(root, net.max.count);
		JSAVE(root, net.max.age);
		JSAVE(root, net.sensors.type);
		JSAVE(root, net.sensors.adaptive.alpha);
		JSAVE(root, net.sensors.gaussian.cells);
		JSAVE(root, net.sensors.gaussian.threshold);
		JSAVE(root, net.sensors.gaussian.width);
		JSAVE(root, net.spike.lif);
		JSAVE(root, net.spike.enc);
		JSAVE(root, net.spike.mod);
		JSAVE(root, net.spike.max.delay);

		///=================
		/// Layer
		///=================

		JSAVE(root, layer.activations);

		///=================
		/// Node
		///=================

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
		/// Training
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
		JSAVE(root, task.epochs);
		JSAVE(root, task.threads);

		///=================
		/// Data
		///=================

		JSAVE(root, data.type);
		JSAVE(root, data.image.order);
		JSAVE(root, data.image.dim);
		JSAVE(root, data.samples);
		JSAVE(root, data.sets);

		std::ofstream ofs(_file);

		if (ofs.is_open())
		{
			ofs << root.dump(4);
		}

		status = ConfigStatus::Saved;
	}

	void Conf::generate(const std::string& _file)
	{
		if (_file.empty())
		{
			save();
		}
		save(_file);
		status = ConfigStatus::Generated;
	}

	std::string Conf::validate()
	{
		std::stringstream report;

		///=================
		/// Net
		///=================

		check(report, net.type != NetType::Undef, "[Net] Invalid network type");
		check(report, net.init.layers.size() >= 2, "[Net] Please specify at least two layers (input and output)");

		/// Disable irrelevant parameter and element mutations
		if (net.type == NetType::Spiking)
		{
			check(report, net.spike.max.delay > 0.0, "[Net] Max spike delay must be > 0");
		}
		else if (net.type == NetType::Convolutional)
		{
			check(report, net.init.layers.front().type == LayerType::Convolutional, "[Net] Invalid initial layers for convolutional network.");
		}

		for (uint i = 0; i < net.init.layers.size(); ++i)
		{
			check(report, !net.init.layers[i].nodes.empty(), "[Layer ", i + 1, "] Invalid layer size");
		}

		///=================
		/// Layer
		///=================

		for (const auto& af : layer.activations)
		{
			check(report, af.second != FuncType::Undef, "[Layer] Invalid activation function for layer type", af.first);
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
			check(report, species.init.count > 0, "[Species] Initial number of species count must be > 0");
			check(report, species.max.count > 0, "[Species] Maximal number of species count must be > 0");
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
			check(report, learning.mutation.scale > 0.0 && learning.mutation.scale < 1.0,  "[Learning] Mutation SD scale must be > 0 and < 1");
		}
		if (learning.mode == LearningMode::STDP)
		{
			check(report, learning.stdp.rate > 0.0 && learning.stdp.rate < 1.0,  "[Learning] STDP learning rate must be > 0 and < 1");
		}

		///=================
		/// Fitness
		///=================

		check(report, fitness.stat.alpha > 0.0, "[Fitness] Forgetting rate must be > 0");

		///=================
		/// Task
		///=================

		check(report, task.runs > 0, "[Task] Number of runs must be > 0");
		check(report, task.epochs > 0, "[Task] Max. number of epochs must be > 0");
		check(report, task.threads > 0, "[Task] Number of threads must be > 0");

		///=================
		/// Data
		///=================

		if (data.type == InputType::Image)
		{
			check(report, data.image.dim.height() > 0, "[Data] Invalid image height");
			check(report, data.image.dim.width() > 0, "[Data] Invalid image width");
			check(report, data.image.dim.depth() > 0, "[Data] Invalid image depth");
		}

		for (const auto& set : data.sets)
		{
			check(report, set.second >= 0.0, "[Data] ", set.first, " set proportion is negative.");
		}

		/// Ensure that the data sets are not empty.
		if (data.sets.empty())
		{
			data.sets[Stage::Train] = 1.0;
		}

		/// Normalise data sets
		Roulette<Stage, FuncType::Tanh> wheel(data.sets);
		for (uint elem = 0; elem < wheel.elements.size(); ++elem)
		{
			data.sets[wheel.elements[elem]] = wheel.get_weights().normal[elem];
		}

		/// Return a report
		return report.str();
	}

//	void Conf::init()
//	{
//		/// Initialise the activation functions
//		/// and their derivatives.
////		Activation::init();

//		if (data.type == InputType::Image)
//		{
//			/// Compute the order of image element access
//			/// according to the data.image.order parameter
//			compute_strides(data.image);
//		}
//	}

	real Conf::init(const ParamType _type)
	{
		real value(0.0);
		auto idef(conf.param.init[_type]);

		switch (idef.dist)
		{
		case Dist::Normal:
			value = rnd_nd(idef.mean, idef.sd);
			break;

		case Dist::Uniform:
			value = rnd_real(idef.min, idef.max);
			break;

		case Dist::PosNormal:
			value = rnd_pos_nd(idef.mean, idef.sd);
			break;

		case Dist::NegNormal:
			value = rnd_neg_nd(idef.mean, idef.sd);
			break;

		default:
			break;
		}

		/// Check bounds
		/// @todo Better fallback logic - this falls back to a uniform distriution
		if (value < idef.min ||
			value > idef.max)
		{
			value = rnd_real(idef.min, idef.max);
		}

		return value;
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
	/// Parameter configuration
	///=====================================

	os& operator << (os& _os, const InitDef& _def)
	{
		_os << "\n\tInit. distribution: " << _def.dist
			<< "\n\tInit. mean: " << _def.mean
			<< "\n\tInit. SD: " << _def.sd
			<< "\n\tLower bound: " << _def.min
			<< "\n\tUpper bound: " << _def.max;

		return _os;
	}

	///=====================================
	/// Layer configuration
	///=====================================

	os& operator << (os& _os, const LayerDef& _conf)
	{
		return _os << "\n------[ Layer ]------"
				   << "\nFixed: " << std::boolalpha << _conf.fixed
				   << "\nType: " << _conf.type
				   << "\nNodes: " << _conf.nodes
				   << "\n---------------------";
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
			<< "\nType: " << _conf.sensors.type;

		if (_conf.sensors.type == SensorType::Adaptive)
		{
			_os << "\n\tAlpha: " << _conf.sensors.adaptive.alpha;
		}
		else if (_conf.sensors.type == SensorType::Gaussian)
		{
			_os << "\n\tNodes: " << _conf.sensors.gaussian.cells
				<< "\n\tCutoff: " << _conf.sensors.gaussian.threshold
				<< "\n\tBeta: " << _conf.sensors.gaussian.width;
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
	/// Layer
	///=====================================

	os& operator << (os& _os, const Conf::Layer& _conf)
	{
		_os << cat("Layer")
			<< "\nActivation functions: " << _conf.activations;

		return _os;
	}

	///=====================================
	/// Node
	///=====================================

	os& operator << (os& _os, const Conf::Node& _conf)
	{
		_os << cat("Node");
		if (conf.net.type == NetType::Spiking)
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

	os& operator << (os& _os, const Conf::Env& _conf)
	{
		_os << cat("Task")
			<< "\nTask mode: " << _conf.mode
			<< "\nRuns: " << _conf.runs
			<< "\nEpochs: " << _conf.epochs
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

		if (_conf.type == InputType::Image)
		{
			_os << "\nImage parameters:\n"
				<< "\nDimension order: " << _conf.image.order
				<< "\nDimensions: " << _conf.image.dim;
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
			<< _conf.layer
			<< _conf.node
			<< _conf.link
			<< _conf.species
			<< _conf.learning
			<< _conf.fitness
			<< _conf.task
			<< _conf.data;

		return _os << "\n";
	}

}
