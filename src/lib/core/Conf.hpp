#ifndef CORTEX_CONF_HPP
#define CORTEX_CONF_HPP

#include "Globals.hpp"
#include "Ops.hpp"

namespace Cortex
{
	///=============================================================================
	/// @struct Parameter definition.
	///=============================================================================
	struct InitDef : Serialiser
	{
		/// Distribution for the initial value
		Dist dist{Dist::Normal};

		/// Mean
		real mean{0.0};

		/// Standard deviation for initialisation
		real sd{1.0};

		/// Minimal acceptable value
		real min{std::numeric_limits<real>::min()};

		/// Maximal acceptable value
		real max{std::numeric_limits<real>::max()};

		InitDef(const Dist _dist = Dist::Normal,
				const real _mean = 0.0,
				const real _sd = 1.0,
				const real _min = std::numeric_limits<real>::min(),
				const real _max = std::numeric_limits<real>::max());

		virtual void load(const json& _j) override;

		virtual void save(json& _j) const override;
	};

	///=============================================================================
	/// @struct Link definition.
	///=============================================================================
	struct LinkDef : Serialiser
	{
		/// Dimensions of the vertex tensor
		LinkType type{LinkType::Forward};

		/// Membrane time constant
		real weight{0.0};

		LinkDef(const LinkType _type = LinkType::Forward,
				const opt<real> _weight = nopt);

		virtual void load(const json& _j) override;

		virtual void save(json& _j)	const override;

		friend bool operator == (const LinkDef& _lhs, const LinkDef& _rhs);

		friend bool operator != (const LinkDef& _lhs, const LinkDef& _rhs);

		friend os& operator << (os& _os, const LinkDef& _lc);
	};

	///=============================================================================
	/// @struct Node definition.
	/// This serves the role of a gene.
	///=============================================================================
	struct NodeDef : Serialiser
	{
		/// Dimensions of the vertex tensor
		VCube dim{1, 1, 1};

		/// Membrane time constant
		real tau{5.0};

		NodeDef(const VCube _dim = {1, 1, 1},
				const opt<real> _tau = nopt);

		virtual void load(const json& _j) override;

		virtual void save(json& _j)	const override;

		friend bool operator == (const NodeDef& _lhs, const NodeDef& _rhs);

		friend bool operator != (const NodeDef& _lhs, const NodeDef& _rhs);

		friend os& operator << (os& _os, const NodeDef& _lc);
	};

	///=============================================================================
	/// @struct Layer definition.
	/// This serves the role of a chromosome.
	///=============================================================================
	struct LayerDef : Serialiser
	{
		/// The layer type determines how links should be organised.
		LayerType type{LayerType::Regular};

		/// Node definitions.
		std::vector<NodeDef> nodes{};

		/// Indicates whether we can add / erase
		/// nodes to / from the layer.
		bool fixed{false};

		LayerDef(const LayerType _type = LayerType::Regular,
				 const std::vector<NodeDef>& _nodes = {},
				 const bool _fixed = false);

		virtual void load(const json& _j) override;

		virtual void save(json& _j)	const override;

		friend bool operator == (const LayerDef& _lhs, const LayerDef& _rhs);

		friend bool operator != (const LayerDef& _lhs, const LayerDef& _rhs);

		friend os& operator << (os& _os, const LayerDef& _lc);
	};

	///=============================================================================
	/// @struct Main configuration.
	///=============================================================================
	inline struct Conf
	{
	public:

		///=====================================
		/// Net
		///=====================================

		struct Net
		{
			/// Limited to classical, convolutional or spiking
			NetType type{NetType::Classical};

			struct Init
			{
				/// Initial number of networks
				uint count{50};

				/// Initial network layers.
				std::vector<LayerDef> layers{2, {LayerType::Regular, {1, NodeDef{}}, true}};
			} init;

			struct Max
			{
				/// Maximal number of networks
				uint count{200};

				/// The maximal number of generations that a
				/// network is allowed to exist for.
				/// Set to 0 to disable the age limit.
				uint age{50};
			} max;

			struct Sensors
			{
				/// Sensor type
				SensorType type{SensorType::Direct};

				struct Adaptive
				{
					real alpha{0.25};
				} adaptive;

				struct Gaussian
				{
					/// Number of receptor cells per variable.
					/// The default is 10.
					uint cells{10};

					/// Cutoff threshold for spike delays.
					real threshold{0.95};

					/// This controls the width of the receptive fields.
					real width{1.5};
				} gaussian;
			} sensors;

			struct Spike
			{
				/// The membrane potential of LIF spiking nodes
				/// decays over time, whereas non-LIF nodes
				/// maintain their activation indefinitely.
				bool lif{true};

				/// The type of spike encoding determines
				/// the efficiency of information processing.
				SpikeEnc enc{SpikeEnc::Time};

				/// Coefficient for rank order encoding
				real mod{0.9};

				struct Max
				{
					/// Maximal spike delay
					real delay{70.0};
				} max;
			} spike;
		} net;

		///=====================================
		/// Layer
		///=====================================

		struct Layer
		{
			/// Transfer functions allowed
			hmap<LayerType, FuncType> activations =
			{
				{LayerType::Regular, FuncType::Tanh},
				{LayerType::Convolutional, FuncType::ReLU}
			};
		} layer;

		///=====================================
		/// Node
		///=====================================

		struct Node
		{
			/// Parameter configuration for the
			/// membrane decay constant
			InitDef tau;
		} node;

		///=====================================
		/// Link
		///=====================================

		struct Link
		{
			/// Types of links allowed.
			hset<LinkType> types
			{
				LinkType::Forward,
				LinkType::Lateral,
				LinkType::Skip
			};

			/// Parameter configuration for link weights
			InitDef weight;
		} link;

		///=====================================
		/// Param
		///=====================================

		struct Param
		{
			hmap<ParamType, InitDef> init
			{
				{ParamType::Weight, {Dist::Normal, 0.0, 1.0}},
				{ParamType::TimeConstant, {Dist::PosNormal, 10.0, 1.0}}
			};
		} param;

		///=====================================
		/// Species
		///=====================================

		struct Species
		{
			bool enabled{true};

			struct Init
			{
				/// The initial number of species
				uint count{5};
			} init;

			struct Max
			{
				uint count{20};
			} max;
		} species;

		///=====================================
		/// Learning
		///=====================================

		struct Learning
		{
			LearningMode mode{LearningMode::Mutation};

			struct Mutation
			{
				/// Scaling factor for the SD used for mutations.
				/// Default: 0.05 (5%)
				real scale{0.05};
			} mutation;

			struct STDP
			{
				/// Learning rate for STDP.
				real rate{0.005};

				/// Ratio of LTD to LTP strength.
				real dp_ratio{1.05};
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
				real alpha{0.25};
			} stat;
		} fitness;

		///=====================================
		/// Task
		///=====================================

		struct Env
		{
			/// Network evolution mode
			EnvType mode{EnvType::Population};

			/// Task type
			TaskType type{TaskType::Control};

			/// Number of times to run the experiment.
			uint runs{1};

			/// Maximal number of epochs.
			///
			/// @todo Indefinite training.
			uint epochs{100};

			/// Number of threads to use for network evaluation.
			uint threads{std::thread::hardware_concurrency()};
		} task;

		///=====================================
		///	Data
		///=====================================

		struct Data
		{
			/// Type of data we are processing
			InputType type{InputType::RealValued};

			struct Image
			{
				/// Indicates the order of dimensions
				/// in the input image vector.
				/// Default: DHW (Depth, Height, Width).
				DimOrder order{DimOrder::DHW};

				/// Image dimensions
				VCube dim;
			} image;

			/// Number of samples to use.
			/// Set to 0 to use all samples.
			/// A nonzero value can be used
			/// to process a small batch
			/// for debugging purposes.
			uint samples{0};

			/// Respective proportions of the training,
			/// validation and test sets relative
			/// to the entire data set.
			hmap<Stage, real> sets
			{
				{Stage::Train, 0.8},
				{Stage::Dev, 0.1},
				{Stage::Test, 0.1}
			};
		} data;

	public:

		/// Indicates the current status of the configuration.
		ConfigStatus status{ConfigStatus::Undef};

		/// @brief Load and check a configuration.
		std::string load(const std::string& _file = "config.json");

		/// @brief Save the current configuration.
		void save(const std::string& _file = "config.json");

		/// @brief Generate a default configuration.
		void generate(const std::string& _file = "default_config.json");

		/// @brief Sanity check for loaded parameters.
		std::string validate();

		/// @brief Parameter initialisation
		static real init(const ParamType _type);

//	private:

//		/// @brief Perform initialisation steps.
//		void init();

	} conf;
}

#endif // CORTEX_CONF_HPP
