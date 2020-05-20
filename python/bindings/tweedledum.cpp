/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/xag.hpp>
#include <pybind11/pybind11.h>
#include <sstream>
#include <string>
#include <tweedledum/algorithms/decomposition/barenco.h>
#include <tweedledum/algorithms/synthesis/oracles/xag_synth.h>
#include <tweedledum/algorithms/synthesis/stg.h>
#include <tweedledum/algorithms/utility/rewrite.h>
#include <tweedledum/ir/Circuit.h>
#include <tweedledum/ir/Module.h>
#include <tweedledum/ir/Wire.h>
#include <tweedledum/writers/write_qasm.h>

namespace tweedledum {

enum class SynthMethod { exact_esop, pkrm, pprm, spectrum, xag };

std::string synthesize_xag(
    mockturtle::xag_network const& ntk, SynthMethod method)
{
	using namespace mockturtle;
	if (method == SynthMethod::xag || ntk.num_pis() > 16) {
		Circuit circuit = xag_synth(ntk);
		std::ostringstream os;
		write_qasm(circuit, os);
		return os.str();
	}
	auto const functions = simulate<kitty::dynamic_truth_table>(
	    ntk, default_simulator<kitty::dynamic_truth_table>(ntk.num_pis()));

	// FIXME: Properly handle multi-output oracles!
	Circuit circuit(nullptr);
	std::vector<wire::Id> qubits;
	for (uint32_t i = 0u; i < functions[0].num_vars() + 1; ++i) {
		qubits.emplace_back(circuit.create_qubit());
	}
	switch (method) {
	case SynthMethod::exact_esop:
		stg_from_exact_esop()(circuit, qubits, functions[0]);
		break;

	case SynthMethod::pkrm:
		stg_from_pkrm()(circuit, qubits, functions[0]);
		break;

	case SynthMethod::pprm:
		stg_from_pprm()(circuit, qubits, functions[0]);
		break;

	case SynthMethod::spectrum:
		stg_from_spectrum()(circuit, qubits, functions[0]);
		break;

	default:
		break;
	}

	using op_type = typename Circuit::op_type;
	barenco_params params;
	params.use_ncrx = false;
	auto ncx_rewriter = [&](Circuit& dest, op_type const& op) {
		if (op.is(gate_ids::ncx)) {
			if (op.num_wires() == dest.num_qubits()) {
				dest.create_qubit(wire::Mode::ancilla);
			}
			std::vector<wire::Id> controls;
			std::vector<wire::Id> targets;
			op.foreach_control([&](wire::Id control) {
				controls.push_back(control);
			});
			op.foreach_target([&](wire::Id target) {
				targets.push_back(target);
			});
			detail::barenco_decomp(dest, GateLib::ncx, controls,
			    targets.at(0), params);
			return true;
		}
		return false;
	};
	Circuit decomposed = rewrite_circuit(circuit, ncx_rewriter);
	std::ostringstream os;
	write_qasm(decomposed, os);
	return os.str();
}

} // namespace tweedledum

void init_tweedledum(pybind11::module& m)
{
	using namespace tweedledum;
	namespace py = pybind11;

	py::enum_<SynthMethod>(m, "SynthMethod")
	    .value("ExactESOP", SynthMethod::exact_esop)
	    .value("PKRM", SynthMethod::pkrm)
	    .value("PPRM", SynthMethod::pprm)
	    .value("Spectrum", SynthMethod::spectrum)
	    .value("XAG", SynthMethod::xag)
	    .export_values();

	m.def("synthesize_xag", &synthesize_xag, py::arg("xag"),
	    py::arg("method") = SynthMethod::xag, "Synthesize XAG");
}
