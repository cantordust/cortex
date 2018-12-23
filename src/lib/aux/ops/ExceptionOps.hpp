#ifndef CORTEX_EXCEPTIONS_HPP
#define CORTEX_EXCEPTIONS_HPP

#include "Enum.hpp"

namespace Cortex
{
	///=============================================================================
	///	Exception handling.
	///=============================================================================

	/// @todo Destroy global instances after an unhandled exception.
	inline void destroy()
	{
//		conf.reset(nullptr);
//		data.reset(nullptr);
	}

	/// All exceptions should be funnelled through this function.
	inline void pitch(const ErrorType _error, std::string&& _msg)
	{
		/// Destroy static variables
		destroy();
	}

} // namespace Cortex

#endif // CORTEX_EXCEPTIONS_HPP
