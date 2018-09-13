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

		real initialise();

		void load(json& _j);

		void save(json& _j) const;
	};

	inline void from_json(const json& _j, ParamConf& _conf);

	inline void to_json(json& _j, const ParamConf& _conf);

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

		void load(json& _j);

		void save(json& _j)	const;

		friend bool operator == (const LayerConf& _lhs, const LayerConf& _rhs);

		friend bool operator != (const LayerConf& _lhs, const LayerConf& _rhs);

		friend os& operator << (os& _os, const LayerConf& _lc);
	};

	inline void from_json(const json& _j, LayerConf& _conf);

	inline void to_json(json& _j, const LayerConf& _conf);

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
			TrainingMode mode = TrainingMode::Mutation;

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

		} training;

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
			TaskMode mode = TaskMode::Population;

			/// Task type
			TaskType type = TaskType::Control;

			/// Number of times to run the experiment.
			uint runs = 1;

			/// Maximal number of epochs.
			///
			/// @todo Indefinite training.
			uint epochs = 100;

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
				DimOrder order = DimOrder::DHW;

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

				void compute_steps();

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

		/// @brief Check all the loaded parameters.
		void check();

		/// @brief Initialise transfer and backprop functions.
		void init();

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
