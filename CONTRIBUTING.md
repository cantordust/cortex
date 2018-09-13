# Guidelines for Cortex development

## Coding style

- **Allman-style indentation**
    - Please use braces for all statements, even one-line `for` and `if` statements.

- **Avoid macros and all-capitals for variables**.
    - `#define VAR 5` should be `const int var(5);`
	- Macros are OK for things that cannot be accomplished from within the language.
		- In particular, _do_ use the ENUM macro for defining enums, which provides nice facilities for printing and serialisation.

- **No manual allocations** (forget about `new` and `delete`).
    - STL containers such as `std::vector` automatically manage their resources, so use `std::vector<MyClass>` instead of `std::vector<MyClass*>`.
	- If you need to use pointers, use `std::unique_ptr` of `std::shared_ptr`.
		- There are helper functions and aliases for smart pointers. Cf. `Types.hpp`, `Globals.hpp` and `ContainerOps.hpp` for more details.
	- Passing raw pointers to functions and storing them in containers is OK provided the following conditions are satisfied:
		- The object pointed to is managed by a smart pointer _somewhere_;
		- The lifetime of the raw pointer is clearly not going to exceed that of the managing smart pointer.
		
*Important*: The evaluation function takes a `const NetPtr`. Do *not* pass a raw pointer (`Net*`) to your evaluation function!

- `std::reference_wrapper<T>` can be used for storing references in containers

- `std::long_and_unwieldy_container_names` can be aliased.
	- Use `using alias = type` rather than `typedef type alias`. This allows for alias templates such as `template<typename T> using uptr = std::unique_ptr<T>`

- `const auto&` can be used for long and unwieldy types (like `std::vector<std::pair<unsigned int, double>>`) when iterating over containers. This is particularly useful when iterating over containers containing `std::unique_ptr`.

- Documenting new functions using Doxygen tags would be highly appreciated.

I am preparing an appropriate `uncrustify.cfg` configuration, which you can use to format your code before pushing. This would help to keep the code formatting consistent.
