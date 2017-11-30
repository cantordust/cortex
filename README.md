# Cortex neuroevolution platform

Cortex evolves neural networks efficiently by employing a complexity-based speciation paradigm and an intuitive genotype representation with virtually no redundancy.
It supports classical and spiking networks and can evolve both the topology and the parameters of the networks.
Agents are evaluated in parallel using a lightweight threadpool library.

## Development status
Cortex is under active development, and the API is not completely stable. I have included a couple of experiments with the distribution, and I'm working on more, so stay tuned.

## Coding style
Please use:
- **Allman-style indentation**;
- Braces for all statements, even one-line **for** and **if** statements;
- `auto` keyword for long and unwieldy types (like `std::vector<std::pair<unsigned int, double>>`);
- `enum class` rather than plain `enum`
- `std::unique_ptr<T>` and `std::reference_wrapper<T>` rather than `T*` and `T&`
- `using alias = type` rather than `typedef type alias`

## Short-term TODO list
- A proper manual
- Network visualisation
- GUI

## Mid-term TODO list
- Layered networks
    - CNN, LSTM, etc.
- Hybrid strategy employing both training and evolution

## Long-term TODO list
- TensorFlow bindings via a Python wrapper
- GPU acceleration
