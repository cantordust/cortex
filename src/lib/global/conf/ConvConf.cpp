#include "Conf.hpp"
#include "ConvConf.hpp"

namespace Cortex
{
	template<>
	void load(const json& _j, ConvConf& _c)
	{
		load(_j, "w", _c.w);
		load(_j, "h", _c.h);
		load(_j, "d", _c.d);
		load(_j, "k", _c.k);
		load(_j, "f", _c.f);
		load(_j, "s", _c.s);
		load(_j, "p", _c.p);
	}

	ConvConf::ConvConf(Conf& _conf)
		:
		  ConfBase(_conf, "Convolution configuration")
	{
		set_defaults();
	}

	void ConvConf::set_defaults()
	{
		w = 0;
		h = 0;
		d = 0;
		k = 0;
		f = 0;
		s = 0;
		p = 0;
	}

	void ConvConf::validate()
	{
		check(w > 0, "Invalid input width (w: ", w, ")");
		check(h > 0, "Invalid input height (h: ", h, ")");
		check(d > 0, "Invalid input depth (d: ", d, ")");
		check(k > 0, "Invalid number of filters (k: ", k, ")");
		check(f > 0, "Invalid filter size (f: ", f, ")");
		check(s > 0, "Invalid stride (s: ", s, ")");
		check(p > 0, "Invalid padding (p: ", p, ")");
		check(f < w, "Filter size greater than input width (f: ", f, ", w: ", w, ")");
		check(f < h, "Filter size greater than input height (f: ", f, ", h: ", h, ")");
	}

	std::ostream& operator << (std::ostream& _strm, const ConvConf& _conf)
	{
		_strm << _conf.header()
			  << "\n\twidth: " << _conf.w
			  << "\n\theight: " << _conf.h
			  << "\n\tdepth: " << _conf.d
			  << "\n\tfilter count: " << _conf.k
			  << "\n\tfilter size: " << _conf.f
			  << "\n\tstride: " << _conf.s
			  << "\n\tzero-padding: " << _conf.p;

		return _strm << "\n";
	}
}
