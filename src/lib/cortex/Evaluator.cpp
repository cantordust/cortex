#include "Evaluator.hpp"
#include "Net.hpp"
#include "Layer.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	///=============================================================================
	/// Custom comparator for spike events
	///=============================================================================
	bool operator < (const Spike& _lhs, const Spike& _rhs) noexcept
	{
		return _lhs.node->last.out > _rhs.node->last.out;
	}

	///=============================================================================
	///	Vectorised layer
	///=============================================================================

	///=====================================
	/// Constructor and destructor
	///=====================================

	Evaluator::Layer::Layer(const uint _nodes)
	{
		output = Mat(_nodes + 1 /* bias */, 1, arma::fill::zeros);
		output.col(0).fill(1.0);
	}

	///=============================================================================
	///	Evaluator
	///=============================================================================

	///=====================================
	/// Constructor and destructor
	///=====================================

	Evaluator::Evaluator()
		:
		  rfield(*this)
	{}

	///=====================================
	///	Structural operations
	///=====================================
	void Evaluator::compile(const Net& _net)
	{
		layers.clear();
		for (uint l = 0; l < _net.layers.size(); ++l)
		{
			layers.emplace_back(_net.layers[l]->lconf.nodes);
			if (l > 0)
			{
				/// @todo Populate weights
				layers.back().weights = Mat(layers.back().output.n_rows, layers[layers.size() - 2].output.n_rows, arma::fill::zeros);
			}
		}
	}

	///=====================================
	/// Transfer
	///=====================================

	/// Integrate and transfer routine for spiking networks.
	/// This version uses direct spike times.
	void Evaluator::evaluate(const Spike& _spike)
	{
		Node& src(*(_spike.node));
		for (const auto& tgt : _spike.node->targets)
		{
			/// Update the membrane potential.
			/// This can be done as a single calculation by
			/// computing the repolarisation due to leakm
			/// current and then adding the contribution
			/// of the current spike, which amounts to the
			/// weight of the corresponding synapse.
			tgt->potential.update(tgt->potential.value * std::exp((tgt->last.in - src.last.out) / tgt->tau.value) +
								  tgt->sources.at(_spike.node).weight.value);

			/// Update the timing of the last input.
			tgt->last.in = src.last.out;

			/// Check if the firing threshold has been reached.
			///
			/// @todo Replace 1.0 with a dynamic threshold.
			/// (Cf. https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1003560)
			if (tgt->potential.value >= 1.0)
			{
				/// Get a random spike delay and update the
				/// last spike time for the target node.
				tgt->last.out = src.last.out + rnd_real(0.0, conf->net.spike.max.delay);

				/// Schedule a spike from the target node.
				scheduler.emplace(tgt);

				/// STDP
				if (conf->learning.mode == LearningMode::STDP)
				{
					/// Apply LTP to all links from source nodes
					for (auto& src : tgt->sources)
					{
						/// Both excitatory and inhibitory links are potentiated
						src.second.ltp((conf->learning.stdp.rate * std::exp( - (tgt->last.out - src.first->last.out) / tgt->tau.value )));
					}
				}

				/// Reset the membrane potential to 0.
				/// There is no refractory period.
				tgt->potential.update(0.0);
			}
		}
	}

	///=====================================
	/// Node
	///=====================================

//	template<>
//	void Node::receive_input<NetType::Classical>()
//	{
//		for (const auto& link : links.sources)
//		{
//			if (link.second->src.axon.potential() != 0.0)
//			{
//				soma.add<NetType::Classical>(link.second);
//			}
//		}
//	}

//	template<>
//	void Node::receive_input<SpikeEnc::Time>(const LinkPtr& _link)
//	{
//		soma.add<SpikeEnc::Time>(_link);
//	}

//	/// @todo Implementation
//	template<>
//	void Node::receive_input<SpikeEnc::RankOrder>(const LinkPtr& _link)
//	{

//	}

//	/// @todo Implementation.
//	template<>
//	void Node::eval<SpikeEnc::RankOrder>(const LinkPtr& _link)
//	{
//		receive_input<SpikeEnc::RankOrder>(_link);
//	}

//	///=====================================
//	/// Net
//	///=====================================

//	template<>
//	void Net::convert<RFType::Gaussian>(const Mat& _input)
//	{
//		real delay(0.0);

//		for (uint var = 0; var < _input.size(); ++var)
//		{
//			for (uint r = 0; r < nodes[var].size(); ++r)
//			{
//				delay = grf[r].convert(_input[var].front());
//				if (delay < conf->net.rf.grf.cutoff)
//				{
//					nodes[var][r]->schedule(delay);
//				}
//			}
//		}
//	}

//	template<>
//	void Net::convert<RFType::Convolutional>(const Mat& _input)
//	{

//	}

//	template<>
//	void Net::convert<RFType::Direct>(const Mat& _input)
//	{
//		for (uint var = 0; var < _input.size(); ++var)
//		{
//			for (uint r = 0; r < _input[var].size(); ++r)
//			{
//				nodes[var][r]->soma.add<NetType::Classical>(_input[var][r]);
//			}
//		}
//	}

//	template<>
//	void Net::eval<SpikeEnc::Time>()
//	{
//		while (!scheduler.empty())
//		{
//			scheduler.top().node->axon.last_event = scheduler.top().time;
//			for (const auto& tgt : scheduler.top().node->links.targets)
//			{
//				tgt.second->tgt.eval<SpikeEnc::Time>(tgt.second);
//			}
//			scheduler.pop();
//		}
//	}

//	/// @todo Implementation
//	template<>
//	void Net::eval<SpikeEnc::RankOrder>()
//	{
//		eval<SpikeEnc::Time>();
//	}

//	std::vector<real> Net::get_output()
//	{
//		std::vector<real> output;
//		output.reserve(nodes.at(NodeType::Output).size());
//		for (const auto& out : nodes.at(NodeType::Output))
//		{
//			output.push_back(out->axon.potential());
//		}

////		dlog d;
////		d << "Net " << id << " output:";
////		for (const real o : output)
////		{
////			d << " " << o;
////		}

//		return std::move(output);
//	}

//	void Net::eval(const std::vector<real>& _input)
//	{

//	}

//	///=====================================
//	/// Dispatch functions
//	///=====================================

//	template<>
//	void Net::eval<NetType::Spiking>()
//	{
//		switch (conf->net.spike.enc)
//		{
//		case SpikeEnc::Time:
//			return eval<SpikeEnc::Time>();

//		case SpikeEnc::RankOrder:
//			return eval<SpikeEnc::RankOrder>();

//		default:
//			die("Invalid encoding type ", conf->net.spike.enc);
//		}
//	}

	void Evaluator::evaluate(const Sample& _sample)
	{
		/// Convert the sample and present it to the network
		rfield.convert(_sample);

//		switch (conf->net.type)
//		{
//		case NetType::Classical:
//			return eval<NetType::Classical>();

//		case NetType::Spiking:
//			return eval<NetType::Spiking>();

//		default:
//			die("Invalid network type ", conf->net.type);
		//		}
	}

}
