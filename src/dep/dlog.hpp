#ifndef DLOG_HPP
#define DLOG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <future>
#include <queue>
#include <mutex>
#include <iomanip>
#include <unordered_map>

namespace Async
{
	template<typename T1, typename T2>
	using hmap = std::unordered_map<T1, T2>;

	using glock = std::lock_guard<std::mutex>;

	/// Set of strings affixed to the input
	/// at various positions.
	struct AffixSet
	{
		/// Default values
		struct Default
		{
			inline static uint log_level{0};
			inline static std::string prefix{""};
			inline static std::string infix{" "};
			inline static std::string suffix{"\n"};
		};

		uint log_level{Default::log_level};
		std::string prefix{Default::prefix};
		std::string infix{Default::infix};
		std::string suffix{Default::suffix};
	};

	/// @class The dlog class.
	/// @details
	/// dlog ("debug log") is a tiny header-only library
	/// for printing output from multiple threads to
	/// arbitrary streams without garbling the output.
	class dlog
	{

	public:

		/// Version string.
		inline static const std::string version{"0.2.4"};

	private:

		/// Default log level.
		inline static uint log_level{0};

		/// Master mutex for accessing the semaphores.
		inline static std::mutex semaphore_mutex;

		/// Pointers to output streams and
		/// corresponding mutexes.
		inline static hmap<std::ostream*, std::mutex> semaphores;

		bool out{true};

		/// Strings appended to the input.
		/// If not set in the constructor, these
		/// are given the default values.
		AffixSet afx;

		std::shared_ptr<std::ostream> ofs{nullptr};

		/// Stream associated with this log.
		std::ostream& stream{std::cout};

		/// Buffer for storing the output.
		std::stringstream buffer;

	public:

		template<typename Arg, typename ... Args>
		dlog(std::ostream& _stream, AffixSet _afx, Arg&& _arg, Args&& ... _args)
			:
			  out(_afx.log_level == 0 || _afx.log_level >= log_level),
			  afx(_afx),
			  stream(_stream)
		{
			init(std::forward<Args>(_args)...);
		}

		template<typename ... Args>
		dlog(std::shared_ptr<std::ofstream> _stream, Args&& ... _args)
			:
			  dlog(static_cast<std::ostream&>(*_stream), std::forward<Args>(_args)...)
		{
			ofs = _stream;
		}

		template<typename ... Args>
		dlog(std::ostream& _stream, Args&& ... _args)
			:
			  stream(_stream)
		{
			init(std::forward<Args>(_args)...);
		}

		template<typename ... Args>
		dlog(AffixSet _afx, Args&& ... _args)
			:
			  out(_afx.log_level == 0 || _afx.log_level >= log_level),
			  afx(_afx)
		{
			init(std::forward<Args>(_args)...);
		}

		template<typename ... Args>
		dlog(Args&& ... _args)
		{
			init(std::forward<Args>(_args)...);
		}

		~dlog()
		{
			if (out)
			{
				buffer << afx.suffix;
				flush(stream, buffer.str());
			}
		}

		template<typename T>
		friend dlog& operator << (dlog& _dlog, T&& _t)
		{
			_dlog.gobble(std::forward<T>(_t));
			return _dlog;
		}

		friend dlog& operator << (dlog& _dlog, std::ostream& (*_fp)(std::ostream&))
		{
			if (_dlog.out)
			{
				_dlog.buffer << _fp;
			}
			return _dlog;
		}

		///=====================================
		/// Other convenience functions.
		///=====================================

		template<typename T>
		dlog& add(T&& _t)
		{
			if (out)
			{
				buffer << std::forward<T>(_t);
			}
			return *this;
		}

		template<typename T>
		dlog& operator + (T&& _t)
		{
			add(std::forward<T>(_t));
			return *this;
		}

		template<typename T>
		dlog& format(T&& _t, const uint _width)
		{
			if (out)
			{
				buffer << std::setw(_width) << std::forward<T>(_t);
			}
			return *this;
		}

		///=====================================
		/// Output formatting
		///=====================================

		inline dlog& left()
		{
			if (out)
			{
				buffer << std::left;
			}
			return *this;
		}

		inline dlog& internal()
		{
			if (out)
			{
				buffer << std::internal;
			}
			return *this;
		}

		inline dlog& right()
		{
			if (out)
			{
				buffer << std::right;
			}
			return *this;
		}

		inline dlog& setfill(const char _ch = ' ')
		{
			if (out)
			{
				buffer << std::setfill(_ch);
			}
			return *this;
		}

		///=====================================
		/// Static functions.
		///=====================================

		static void set_log_level(const uint _level)
		{
			log_level = _level;
		}

	private:

		static void spawn_printer()
		{
			std::thread([&]()
			{

			});
		}

		template<typename ... Args>
		void init() {}

		template<typename Arg, typename ... Args>
		void init(Arg&& _arg, Args&& ... _args)
		{
			if (out)
			{
				buffer << afx.prefix << std::forward<Arg>(_arg);
				gobble(std::forward<Args>(_args)...);
			}
		}

		template<typename ... Args>
		void gobble(Args&& ... _args)
		{
			if (out)
			{
				std::array<int, sizeof...(_args)> status{(buffer << afx.infix << std::forward<Args>(_args), 0) ...};
			}
		}

		static void flush(std::ostream& _stream, const std::string& _content)
		{
			if (_content.size() > 0)
			{
				glock lock(semaphore_mutex);
				std::ostream* os(std::addressof(_stream));
				if (os)
				{
					glock lk(semaphores[os]);
					*os << _content;
				}
				else
				{
					semaphores.erase(os);
				}
			}
		}
	};
}

#endif // DLOG_HPP
