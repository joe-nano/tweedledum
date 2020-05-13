/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include <mockturtle/networks/xag.hpp>
#include <pybind11/pybind11.h>
#include <tweedledum/algorithms/synthesis/oracles/xag_synth.h>
#include <tweedledum/ir/CircuitDAG.h>
#include <tweedledum/ir/operations/w3_op.h>

namespace tweedledum {

PYBIND11_MODULE(tweedledum, m)
{
	using Circuit = CircuitDAG<w3_op>;
	namespace py = pybind11;
	m.doc() = "tweedledum ";

	py::class_<wire::Id>(m, "wire_id");

	py::enum_<wire::Mode>(m, "wire_mode")
	    .value("in", wire::Mode::in)
	    .value("out", wire::Mode::out)
	    .value("inout", wire::Mode::inout)
	    .value("ancilla", wire::Mode::ancilla)
	    .export_values();

	py::class_<Circuit>(m, "CircuitDAG")
	    .def(py::init<>())
	    .def("name", &Circuit::name)
	    .def("create_qubit",
	        py::overload_cast<std::string_view, wire::Mode>(
	            &Circuit::create_qubit),
	        py::arg("name") = "", py::arg("mode") = wire::Mode::inout)
	    // Structural properties
	    .def("num_qubits", &Circuit::num_qubits)
	    .def("num_operations", &Circuit::num_operations);

	py::class_<xag_synth_params>(m, "xag_synth_params");
	m.def("synth",
	    py::overload_cast<mockturtle::xag_network const&,
	        xag_synth_params const&>(&xag_synth<Circuit>),
	    py::arg("xag_ntk"), py::arg("params") = xag_synth_params(),
	    "A function synthesize a XAG");
}

} // namespace tweedledum
