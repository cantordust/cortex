# TODO List

## Core features

- Novelty search, substrates (a la HyperNEAT), CPPN.
- Modules within networks
- Single-network operation
- Interactive tasks
- [WIP] Hybrid strategy employing both training and evolution
- Serialisation of networks using JSON

## System

- Provide a way to define a custom path to Armadillo.
- Provide a choice to *not* use Armadillo at all. Add a check in the source as the Evaluator currently depends on Armadillo.
- Link to TF / Caffe and/or export networks to ONNX
- Look into including dlog, threadpool, json as submodules?

## C++
- Structured bindings in range `for` loops (`C++17`)

## Python
- [WIP] Python bindings using pybind11 (https://github.com/pybind/pybind11)
	
## Usability
- A proper manual (part of the API is already documented using Doxygen)
- Network visualisation / GUI
	- three.js
	- Qt
