#include "Evaluator.hpp"
#include "Net.hpp"
#include "Layer.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	void Evaluator::compile(const Net& _net)
	{
		for (const auto& layer : _net.layers)
		{
			switch(layer->lconf.type)
			{
			case LayerType::Convolutional:
				break;

			case LayerType::Regular:
				break;

			default:
				break;
			}
		}
	}

	//	Mat Layer::eval()
	//	{
	//		/// Get values from visitors via
	//		/// recurrent, skip or lateral links.
	//		/// This is only effective for non-convolutional layers.
	//		uint elem(nodes.size());
	//		for (const auto& visitor : visitors)
	//		{
	//			input.at(elem++) = visitor->potential.value;
	//		}

	//		output = weights * input;
	//		output.each_row() += bias;
	//		return transfer[lconf.func](output);
	//	}

	/// @brief Integrate and transfer.
	/// This version uses direct spike times.
//	template<>
//	void Node::transfer<SpikeEnc::Time>()
//	{
//		/// Check if the firing threshold has been reached.
//		if (potential.value >= 1.0)
//		{
//			/// Get a random delay from a normal distribution
//			real delay(rnd_pos_nd(conf->net.spike.max.delay));

//			/// Update the spiking statistics.
//			/// The statistics represent frequency (= 1 / time)
//			potential.update(1.0 / (last.in + delay - last.out));

//			/// Update the last spike time
//			last.out = last.in + delay;

//			/// Schedule a spike from this node
//			layer.net.scheduler.emplace(this, last.out);

//			/// STDP
//			if (conf->learning.mode == LearningMode::STDP)
//			{
//				/// Apply LTP to all links from source nodes
//				for (auto& src : sources)
//				{
//					/// Both excitatory and inhibitory links are potentiated
//					src.second.ltp((conf->learning.stdp.rate * std::exp( - (last.out - src.first->last.out) / tau.value )));
//				}
//			}

//			/// Reset the membrane potential to 0.
//			/// There is no refractory period.
//			potential.update(0.0);
//		}
//	}


	///=====================================
	/// Transfer
	///=====================================

	/// @brief Transfer function for spiking networks.
//	template<SpikeEnc _enc = SpikeEnc::Time>
//	void transfer();


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
//	void Net::eval<NetType::Classical>(const Mat& _input)
//	{
//		/// Convert the input into the appropriate format
//		if (layers.front()->type == LayerType::Convolutional)
//		{
//			convert<RFType::Convolutional>(_input);
//		}

//		for (const auto& layer : layers)
//		{
//			if (layer->type == LayerType::Convolutional)
//			{

//			}
//		}

////		/// Evaluate the nodes in order
////		for (const auto& node : graph)
////		{
////			node->eval<NT::Classical>();
////		}
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

//	template<>
//	void Net::eval<NetType::Undef>(const Sample& _sample)
//	{
//		/// Present the sample to the network
//		convert(_sample);

//		switch (conf->net.type)
//		{
//		case NetType::Classical:
//			return eval<NetType::Classical>();

//		case NetType::Spiking:
//			return eval<NetType::Spiking>();

//		default:
//			die("Invalid network type ", conf->net.type);
//		}
//	}


}
