#ifndef CORTEX_GLOBALS_HPP
#define CORTEX_GLOBALS_HPP

#include "Tensor.hpp"
#include "Spike.hpp"
#include "ID.hpp"
#include "WeightMap.hpp"
#include "History.hpp"
#include "Random.hpp"

namespace Cortex
{
	///=============================================================================
	///	Debugging
	///=============================================================================

	/// Debug level for debugging with "if constexpr" expressions.
	/// @todo Implementation.
	inline constexpr uint debug(0);

	/// @brief Print info about the library.
	void info();

	/// @brief Setup procedure.
	void setup(const std::function<void(const NetPtr)>& _evaluator);

	bool initialise();

	/// @brief Run the task.
	void run();

	void evaluate();

	/// @brief Enqueue network @p _net for evaluation
	void enqueue(const NetPtr _net);

	template<typename Countable, OnlyEnum<Countable>...>
	uint count(const Countable _countable);

	real get_offset(const ElemType _elem, const uint _count);

	void mark_solved(const NetPtr _net);

	/// @brief Parse command line arguments
	void parse(int& _argc, char** _argv);
}

#endif // CORTEX_GLOBALS_HPP
