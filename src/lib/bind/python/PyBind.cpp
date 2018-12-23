#ifdef GENERATE_PYTHON_BINDINGS

#include "Cortex.hpp"

using namespace Cortex;

PYBIND11_MODULE(Cortex, module)
{
	module.doc() = "Python bindings for the Cortex neuroevolution library.";

	auto smCortex(module.def_submodule("Cortex"));

	smCortex.def("info", &Cortex::info)
			.def("setup", &Cortex::setup)
			.def("initialise", &Cortex::initialise)
			.def("run", &Cortex::run)
			.def("evaluate", &Cortex::evaluate)
			.def("enqueue", &Cortex::enqueue)
			;

//	py::class_<Cortex::Env>(module, "Env")
//			.def_static("test", &Cortex::Env::test);

	module.def("rnd_int", &Cortex::rnd_int<int>, "A function that returns a random integer");
}

#endif // GENERATE_PYTHON_BINDINGS
