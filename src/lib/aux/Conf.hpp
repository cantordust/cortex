#ifndef CORTEX_CONF_HPP
#define CORTEX_CONF_HPP

#include "Globals.hpp"
#include "Random.hpp"
#include "Functions.hpp"
#include "Stat.hpp"

/// Load parameter values from JSON.
#define JLOAD(json_node, keys) \
	embed(json_node, #keys); \
	Cortex::load(json_node, #keys, keys);

/// Save parameters to JSON nodes.
#define JSAVE(json_node, keys) \
	Cortex::save(json_node, #keys, keys);

namespace Cortex
{
	///=============================================================================
	///	Configuration helpers
	///=============================================================================

	///=====================================
	/// Parameter configuration
	///=====================================

	struct ParamConf
	{
		/// Initialisation parameters
		struct
		{
			/// Distribution for the initial value
			Dist dist = Dist::Normal;

			/// Initial value
			real value = 0.0;

			/// Standard deviation for initialisation
			real sd = 1.0;
		} init;

		/// Configuration for parameter mutation
		struct
		{
			real sd = 1.0;
		} mutation;

		/// Upper and lower bounds.
		real lbound{min<real>};
		real ubound{max<real>};

		real initialise()
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

		void load(json& _j)
		{
			JLOAD(_j, init.dist);
			JLOAD(_j, init.value);
			JLOAD(_j, init.sd);
			JLOAD(_j, mutation.sd);
			JLOAD(_j, lbound);
			JLOAD(_j, ubound);
		}

		void save(json& _j) const
		{
			JSAVE(_j, init.dist);
			JSAVE(_j, init.value);
			JSAVE(_j, init.sd);
			JSAVE(_j, mutation.sd);
			JSAVE(_j, lbound);
			JSAVE(_j, ubound);
		}
	};

	inline void from_json(const json& _j, ParamConf& _conf)
	{
		_conf.load(const_cast<json&>(_j));
	}

	inline void to_json(json& _j, const ParamConf& _conf)
	{
		_conf.save(_j);
	}

	///=====================================
	/// Layer configuration
	///=====================================

	struct LayerConf
	{
		/// Indicates whether we can add / erase
		/// nodes from / to the layer.
		bool evolvable = true;

		/// The layer type determines how the
		/// layer is evaluated.
		LayerType type = LayerType::Regular;

		/// Number of nodes in the layer.
		uint nodes = 0;

		void load(json& _j)
		{
//			/// Add an entry to the vector of layers.
//			_j.emplace_back(json::object());

//			/// Get a reference to the entry
//			json& j(_j.back());

			JLOAD(_j, evolvable);
			JLOAD(_j, type);
			JLOAD(_j, nodes);
		}

		void save(json& _j)	const
		{
			JSAVE(_j, evolvable);
			JSAVE(_j, type);
			JSAVE(_j, nodes);
		}

		friend bool operator == (const LayerConf& _lhs, const LayerConf& _rhs)
		{
			return (_lhs.type == _rhs.type &&
					_lhs.nodes == _rhs.nodes);
		}

		friend bool operator != (const LayerConf& _lhs, const LayerConf& _rhs)
		{
			return !(_lhs == _rhs);
		}

		friend os& operator << (os& _os, const LayerConf& _lc);
	};

	inline void from_json(const json& _j, LayerConf& _conf)
	{
		_conf.load(const_cast<json&>(_j));
	}

	inline void to_json(json& _j, const LayerConf& _conf)
	{
		_conf.save(_j);
	}

	///=============================================================================
	///	Main configuration
	///=============================================================================

	struct Conf
	{
	public:

		///=====================================
		/// Net
		///=====================================

		struct Net
		{
			/// Limited to classical, convolutional or spiking
			NetType type = NetType::Classical;

			struct Init
			{
				/// Initial number of networks
				uint count = 50;

				/// Network layers.
				/// This has to contain at least two layers
				/// (input and output)
				std::vector<LayerConf> layers;
			} init;

			struct Max
			{
				/// Maximal number of networks
				uint count = 200;

				/// The maximal number of generations that a
				/// network is allowed to exist for.
				/// Set to 0 to disable the age limit.
				uint age = 50;
			} max;

			struct RF
			{
				/// Receptive field type
				RFType type = RFType::Direct;

				struct Adaptive
				{
					real alpha = 0.25;
				} adaptive;

				struct Gaussian
				{
					/// Number of receptors per variable.
					/// The default is 10.
					uint nodes = 10;

					/// Cutoff threshold for spike delays.
					real threshold = 0.95;

					/// This controls the width of the receptive fields.
					real width = 1.5;
				} gaussian;
			} rf;

			struct Spike
			{
				/// The membrane potential of LIF spiking nodes
				/// decays over time, whereas non-LIF nodes
				/// maintain their activation indefinitely.
				bool lif = true;

				/// The type of spike encoding determines
				/// the efficiency of information processing.
				SpikeEnc enc = SpikeEnc::Time;

				/// Coefficient for rank order encoding
				real mod = 0.9;

				struct Max
				{
					/// Maximal spike delay
					real delay = 70.0;
				} max;
			} spike;
		} net;

		///=====================================
		/// Node
		///=====================================

		struct Node
		{
			/// Transfer functions allowed
			hmap<LayerType, hset<Func>> tf =
			{
				{LayerType::Regular, {Func::Tanh}},
				{LayerType::Convolutional, {Func::ReLU}}
			};

			/// Parameter configuration for the
			/// membrane decay constant
			ParamConf tau;
		} node;

		///=====================================
		/// Link
		///=====================================

		struct Link
		{
			/// Types of links allowed.
			hset<LinkType> types =
			{
				LinkType::Forward,
				LinkType::Lateral,
				LinkType::Skip
			};

			/// Parameter configuration for link weights
			ParamConf weight;
		} link;

		///=====================================
		/// Species
		///=====================================

		struct Species
		{
			bool enabled = true;

			struct Init
			{
				/// The initial number of species
				uint count = 5;
			} init;

			struct Max
			{
				uint count = 20;
			} max;
		} species;

		///=====================================
		/// Learning
		///=====================================

		struct Learning
		{
			LearningMode mode = LearningMode::Mutation;

			struct Mutation
			{
				/// Scaling factor for the SD used for mutations.
				/// Default: 0.05 (5%)
				real scale = 0.05;
			} mutation;

			struct STDP
			{
				/// Learning rate for STDP.
				real rate = 0.005;

				/// Ratio of LTD to LTP strength.
				real dp_ratio = 1.05;
			} stdp;

			/// @todo Backprop

		} learning;

		///=====================================
		/// Fitness
		///=====================================

		/// Fitness statistics
		struct Fitness
		{
			struct Stat
			{
				/// @brief Forgetting rate
				real alpha = 0.25;
			} stat;
		} fitness;

		///=====================================
		/// Task
		///=====================================

		struct Task
		{
			/// Network evolution mode
			TaskMode mode = TaskMode::Autonomous;

			/// Task type
			TaskType type = TaskType::Control;

			/// Number of times to run the experiment.
			uint runs = 1;

			/// Maximal number of generations.
			/// @todo Indefinite evolution.
			uint generations = 100;

			/// Number of threads to use for network evaluation.
			uint threads = std::thread::hardware_concurrency();
		} task;

		///=====================================
		///	Data
		///=====================================

		struct Data
		{
			/// Type of data we are processing
			DataType type = DataType::RealValued;

			struct Image
			{
				/// Indicates the order of dimensions
				/// in the input image vector
				/// E.g., DHW = Depth, Height, Width.
				DimOrder order;

				/// Input depth (# of channels)
				uint depth = 0;

				/// Original input height (rows).
				uint height = 0;

				/// Original input width (columns)
				uint width = 0;

				struct Step
				{
					/// The step size for accesing consecutive
					/// elements in the depth dimension.
					uint depth = 0;

					/// The step size for accesing consecutive
					/// elements in the height dimension.
					uint height = 0;

					/// The step size for accesing consecutive
					/// elements in the width dimension.
					uint width = 0;
				} steps;

				void compute_steps()
				{
					switch (order)
					{
					case DimOrder::HDW:
						steps.height = 1;
						steps.width = height * depth;
						steps.depth = height;
						break;

					case DimOrder::HWD:
						steps.height = 1;
						steps.width = height;
						steps.depth = height * width;
						break;

					case DimOrder::DHW:
						steps.height = depth;
						steps.width = height * depth;
						steps.depth = 1;
						break;

					case DimOrder::WHD:
						steps.height = width;
						steps.width = 1;
						steps.depth = height * width;
						break;

					case DimOrder::DWH:
						steps.height = width * depth;
						steps.width = depth;
						steps.depth = 1;
						break;

					case DimOrder::WDH:
						steps.height = width * depth;
						steps.width = 1;
						steps.depth = width;
						break;

					default:
						die("Invalid input order ", order);
					}
				}

				uint conv_layer_limit();

			} image;

			/// Number of samples to use.
			/// Set to 0 to use all samples.
			/// A nonzero value can be used
			/// to process a small batch
			/// for debugging purposes.
			uint samples = 0;

			/// Respective proportions of the training,
			/// validation and test sets relative
			/// to the entire data set.
			wmap<Stage> sets
			{
				{Stage::Train, 0.8},
				{Stage::Dev, 0.1},
				{Stage::Test, 0.1}
			};
		} data;

	private:

		/// Path to the configuration file.
		inline static std::string file;

		/// JSON object holding the parsed configuration.
		inline static json root;

		/// Report containing problems
		/// different subconfigurations
		inline static std::stringstream problems;

	public:

		///=====================================
		/// Configuration loading and validation
		///=====================================

		template<typename ... Args>
		static void check(const bool _condition, Args&& ... _args)
		{
			Cortex::check(problems, _condition, std::forward<Args>(_args)...);
		}

		friend os& operator << (os& _os, const Conf& _conf);

		~Conf()
		{
			dlog("Destroying configuration...");
		}

	private:

		/// @brief Private constructor.
		Conf() {};

		/// @brief Load and check the configuration.
		void load();

		/// @brief Save the current configuration.
		void save();

		/// @brief Check all the loaded parameters
		void check();

		/// @brief Initialise transfer and backprop functions
		void init()
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

		friend class Cortex::Task;
	};

	///=====================================
	/// A unique pointer to a global
	/// configuration instance.
	/// All configuration options
	/// are stored in this variable.
	///=====================================

	inline ConfPtr conf;
}

#endif // CORTEX_CONF_HPP
