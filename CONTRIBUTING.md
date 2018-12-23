# Guidelines for contributing to Cortex

## Coding style

- **Allman-style indentation and braces**
    - Please use braces on new lines for all statements, even one-line `for` and `if` statements.

- **No macros for constants**.
    - `#define VAR 5` should be `const int var(5);`
	- Macros can be used (sparingly) for things that cannot be accomplished using language features or libraries.
		- In particular, _do_ use the ENUM macro for defining enums, which provides nice facilities for printing and serialisation of `enum`s.

- **No manual allocations**
	- Please do not use `new`, `delete`, `malloc` and so forth.
	- If you need to use pointers, use `std::unique_ptr` of `std::shared_ptr`.
		- There are helper functions and aliases for smart pointers.
	- Passing raw pointers to functions and storing them in containers is OK provided the following conditions are satisfied:
		- The object pointed to is managed by a smart pointer _somewhere_;
		- The lifetime of the raw pointer is clearly not going to exceed that of the managing smart pointer.
		
- `std::reference_wrapper<T>` can be used for storing references in containers

- `std::long_and_unwieldy_container_names` can be aliased.
	- Use `using alias = type` rather than `typedef type alias`. This allows for alias templates such as `template<typename T> using up = std::unique_ptr<T>`

- `const auto&` can be used for long and unwieldy types (like `std::vector<std::pair<unsigned int, double>>`) when iterating over containers. This is particularly useful when iterating over containers containing `std::unique_ptr`.

- Documenting new functions using Doxygen tags would be highly appreciated.
