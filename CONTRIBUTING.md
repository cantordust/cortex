# Guidelines for Cortex development

## Coding style

- **No macros and all-capital variables**
    - `#define VAR 5` should be `const int var(5);`
- **No manual allocations**
    - In particular, STL containers such as `std::vector` automatically manage their resources
	on the heap, so use `std::vector<MyClass>` instead of `std::vector<MyClass*>`.
	- If you need to use pointers, cf. the comment about`std::unique_ptr` below
- **Allman-style indentation**
    - Please use braces for all statements, even one-line **for** and **if** statements
- `enum class` rather than plain `enum`
- `std::unique_ptr<T>` rather than `T*` if you *have to* use pointers
    - `std::reference_wrapper<T>` can be used for storing references in containers
	- `std::long_and_unwieldy_container_names` can be aliased (cf. next point)
- `using alias = type` rather than `typedef type alias`
    - This allows for alias templates such as `template<typename T> using uptr = std::unique_ptr<T>`
- `auto` keyword for long and unwieldy types (like `std::vector<std::pair<unsigned int, double>>`)
    - `const auto&` (`auto&` only if absolutely necessary) in range `for` loops
	- Always `const auto&` when iterating over containers containing `std::unique_ptr`

Also, for the sake of clarity, documenting new functions using Doxygen tags would be highly appreciated.

I am in the process of creating an appropriate `uncrustify.cfg` configuration,
which will be part of the distribution. It should be able to do most of the formatting for you.
