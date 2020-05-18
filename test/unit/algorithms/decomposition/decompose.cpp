/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/decomposition/decompose.h"

#include "tweedledum/algorithms/simulation/simulate_classically.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Unitary.h"
#include "tweedledum/ir/Wire.h"

#include <algorithm>
#include <catch.hpp>
#include <vector>

using namespace tweedledum;

TEST_CASE("Decompose using barenco", "[decomp]")
{
	Module module;
	CircuitDAG& circuit = module.circuit_;
	std::vector<wire::Id> qubits(5, wire::invalid_id);
	std::generate(qubits.begin(), qubits.end(), [&]() {
		return circuit.create_qubit();
	});

	circuit.create_op(GateLib::ncx,
	    std::vector(
	        {qubits.at(0), qubits.at(1), qubits.at(2), qubits.at(3)}),
	    std::vector({qubits.at(4)}));
	circuit.create_op(GateLib::ncx,
	    std::vector({qubits.at(0), qubits.at(1), qubits.at(2)}),
	    std::vector({qubits.at(3)}));
	circuit.create_op(
	    GateLib::ncx, qubits.at(0), qubits.at(1), qubits.at(2));
	circuit.create_op(GateLib::cx, qubits.at(0), qubits.at(1));
	circuit.create_op(GateLib::x, qubits.at(0));

	decomp_params params;
	params.barenco_controls_threshold = 2u;
	params.gate_set = gate_set::classic_rev;
	CircuitDAG decomposed = decompose(circuit, params);
	CHECK(decomposed.num_qubits() == circuit.num_qubits() + 1);

	// Create ancilla
	circuit.create_qubit();
	for (uint64_t i = 0ull; i < 32ull; ++i) {
		CHECK(simulate_classically(circuit, i)
		      == simulate_classically(decomposed, i));
	}
}

TEST_CASE("IBM", "[decomp]")
{
	decomp_params params;
	params.gate_set = gate_set::ibm;

	// Non-parameterisable
	std::vector<Gate> one_wire = {GateLib::h, GateLib::x, GateLib::y,
	    GateLib::z, GateLib::s, GateLib::t, GateLib::sdg, GateLib::tdg};
	std::vector<Gate> two_wire = {GateLib::cx, GateLib::cy, GateLib::cz};

	std::vector<Angle> commmon_angles
	    = {sym_angle::pi, sym_angle::pi_half, sym_angle::pi_quarter};
	for (Angle const& a : commmon_angles) {
		one_wire.emplace_back(GateLib::r1(a));
		one_wire.emplace_back(GateLib::rx(a));
		one_wire.emplace_back(GateLib::ry(a));
		one_wire.emplace_back(GateLib::rz(a));

		two_wire.emplace_back(GateLib::crx(a));
		two_wire.emplace_back(GateLib::cry(a));
		two_wire.emplace_back(GateLib::crz(a));
	}
	for (Gate const& g : one_wire) {
		// Create quantum circuit
		Module module;
		CircuitDAG& circuit = module.circuit_;
		wire::Id q0 = circuit.create_qubit();
		circuit.create_op(g, q0);
		// Decompose
		CircuitDAG decomposed = decompose(circuit, params);
		// Create unitaries
		Unitary u_decomp(circuit);
		Unitary u(1u);
		u.create_op(g, q0);
		CHECK(u.is_apprx_equal(u_decomp));
	}
	for (Gate const& g : two_wire) {
		// Create quantum circuit
		Module module;
		CircuitDAG& circuit = module.circuit_;
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		circuit.create_op(g, q0, q1);
		// Decompose
		CircuitDAG decomposed = decompose(circuit, params);
		// Create unitaries
		Unitary u_decomp(circuit);
		Unitary u(2u);
		u.create_op(g, q0, q1);
		CHECK(u.is_apprx_equal(u_decomp));
	}

	// Parameterisable
}