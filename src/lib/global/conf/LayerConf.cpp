#include "Conf.hpp"
#include "LayerConf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, LayerConf& _c)
	{
		load(_j, "in", _c.in);
		load(_j, "out", _c.out);
		load(_j, "conv", _c.conv);
	}

	LayerConf::LayerConf(Conf& _conf)
		:
		  ConfBase(_conf, "Layer configuration"),
		  conv(_conf)
	{
		set_defaults();
	}

	void LayerConf::set_defaults()
	{
		type = LayerType::Regular;
		in = 0;
		out = 0;
	}

	///=========================================
	/// Dispatch functions
	///=========================================

	void LayerConf::validate()
	{
		check(in > 0, "Invalid number of input nodes (in: ", in, ")");
		check(out > 0, "Invalid number of output nodes (out: ", out, ")");
		if (type == LayerType::Conv)
		{
			conv.validate();
		}
	}

	std::ostream& operator << (std::ostream& _strm, const LayerConf& _conf)
	{
		_strm << _conf.header()
			  << "\n\ttype: " << _conf.type
			  << "\n\tinput size: " << _conf.in
			  << "\n\toutput size: " << _conf.out;

		if (_conf.type == LayerType::Conv)
		{
			_strm << "\n\tconv: " << _conf.conv;
		}

		return _strm << "\n";
	}
}
