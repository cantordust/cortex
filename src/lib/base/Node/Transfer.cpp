#include "Conf.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Link.hpp"

namespace Cortex
{
	void Node::schedule(const real _time)
	{
		net.scheduler.push({self(), _time});
	}

	template<>
	void Node::transfer<TF::Sum>()
	{
		axon.val.update(soma.potential);
	}

	template<>
	void Node::transfer<TF::Tanh>()
	{
		axon.val.update(std::tanh(soma.potential));
	}

	template<>
	void Node::transfer<TF::Logistic>()
	{
		axon.val.update(Logistic(soma.potential));
	}

	template<>
	void Node::transfer<TF::ReLU>()
	{
		axon.val.update(ReLU(soma.potential));
	}

	template<>
	void Node::transfer<TF::Gaussian>()
	{
		axon.val.update(Gaussian(soma.potential));
	}

	template<>
	void Node::transfer<TF::Sin>()
	{
		axon.val.update(std::sin(soma.potential));
	}

	template<>
	void Node::transfer<TF::Cos>()
	{
		axon.val.update(std::cos(soma.potential));
	}

	template<>
	void Node::transfer<TF::Abs>()
	{
		axon.val.update(std::fabs(soma.potential));
	}

	template<>
	void Node::transfer<TF::Min>()
	{
		axon.val.update(*std::min_element(soma.values.begin(), soma.values.end()));
	}

	template<>
	void Node::transfer<TF::Max>()
	{
		axon.val.update(*std::max_element(soma.values.begin(), soma.values.end()));
	}

	template<>
	void Node::transfer<TF::Avg>()
	{
		real avg(0.0);
		for (uint arg = 0; arg < soma.values.size(); ++arg)
		{
			avg += (soma.values[arg] + avg) / (arg + 1);
		}
		axon.val.update(avg);
	}

	template<>
	void Node::transfer<TF::Const>()
	{
		axon.val.update(1.0);
	}

	template<>
	void Node::transfer<TF::Golden>()
	{
		axon.val.update(phi);
	}

	/// @brief Integrate and transfer.
	/// This version uses direct spike timings.
	template<>
	void Node::transfer<Enc::Direct>()
	{
		if (soma.potential >= 1.0)
		{
			/// The firing threshold has been reached.
			real delay(conf.rnd_real(0.0, conf.net.spike.max.delay));
			axon.val.update(1.0 / (soma.last_event + delay - axon.time));
			axon.time = soma.last_event + delay;
			schedule(axon.time);

			if (conf.stdp.enabled)
			{
				/// Apply LTP to all sources
				for (const auto& link : links.sources)
				{
					link.second->ltp<NT::Spiking>();
				}
			}

			soma.reset();
		}
	}

	/// @brief Integrate and transfer (spiking version, rank order encoding).
	/// @todo Implementation
	template<>
	void Node::transfer<Enc::RankOrder>()
	{
		transfer<Enc::Direct>();
	}

	///=========================================
	/// Dispatch functions
	///=========================================

	template<>
	void Node::transfer<NT::Classical>()
	{
		/// Update the input statistics
		soma.stat.update(soma.potential);

		/// Compute the output
		switch (tf)
		{
		case TF::Abs:
			transfer<TF::Abs>();
			break;

		case TF::Avg:
			transfer<TF::Avg>();
			break;

		case TF::Const:
			transfer<TF::Const>();
			break;

		case TF::Cos:
			transfer<TF::Cos>();
			break;

		case TF::Gaussian:
			transfer<TF::Gaussian>();
			break;

		case TF::Golden:
			transfer<TF::Golden>();
			break;

		case TF::Logistic:
			transfer<TF::Logistic>();
			break;

		case TF::Max:
			transfer<TF::Max>();
			break;

		case TF::Min:
			transfer<TF::Min>();
			break;

		case TF::ReLU:
			transfer<TF::ReLU>();
			break;

		case TF::Sin:
			transfer<TF::Sin>();
			break;

		case TF::Sum:
			transfer<TF::Sum>();
			break;

		case TF::Tanh:
			transfer<TF::Tanh>();
			break;

		default:
			break;
		}
	}
}
