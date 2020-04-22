/*--------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-------------------------------------------------------------------------------------------------*/
#include "tweedledum/algorithms/decomposition/decompose.hpp"

#include "tweedledum/algorithms/simulation/simulate_classically.hpp"
#include "tweedledum/gates/gate.hpp"
#include "tweedledum/networks/netlist.hpp"
#include "tweedledum/networks/op_dag.hpp"
#include "tweedledum/networks/unitary.hpp"
#include "tweedledum/networks/wire.hpp"
#include "tweedledum/operations/w3_op.hpp"
#include "tweedledum/operations/wn32_op.hpp"

#include <algorithm>
#include <catch.hpp>
#include <vector>

using namespace tweedledum;

TEMPLATE_PRODUCT_TEST_CASE("Decompose using barenco", "[decomp]", (netlist), (wn32_op))
{
	TestType circuit;
	std::vector<wire::id> qubits(5, wire::invalid_id);
	std::generate(qubits.begin(), qubits.end(), [&] () { return circuit.create_qubit(); });

	circuit.create_op(gate_lib::ncx,
	                  std::vector({qubits.at(0), qubits.at(1), qubits.at(2), qubits.at(3)}),
	                  std::vector({qubits.at(4)}));
	circuit.create_op(gate_lib::ncx, std::vector({qubits.at(0), qubits.at(1), qubits.at(2)}),
	                  std::vector({qubits.at(3)}));
	circuit.create_op(gate_lib::ncx, qubits.at(0), qubits.at(1), qubits.at(2));
	circuit.create_op(gate_lib::cx, qubits.at(0), qubits.at(1));
	circuit.create_op(gate_lib::x, qubits.at(0));

	decomp_params params;
	params.barenco_controls_threshold = 2u;
	params.gate_set = gate_set::classic_rev;
	TestType decomposed = decompose(circuit, params);
	CHECK(decomposed.num_qubits() == circuit.num_qubits() + 1);

	// Create ancilla
	circuit.create_qubit();
	for (uint64_t i = 0ull; i < 32ull; ++i) {
		CHECK(simulate_classically(circuit, i) == simulate_classically(decomposed, i));
	}
}

TEMPLATE_PRODUCT_TEST_CASE("IBM", "[decomp]", (netlist, op_dag), (w3_op, wn32_op))
{
	decomp_params params;
	params.gate_set = gate_set::ibm;

	// Non-parameterisable
	std::vector<gate> one_wire = {gate_lib::h, gate_lib::x, gate_lib::y,   gate_lib::z,
	                              gate_lib::s, gate_lib::t, gate_lib::sdg, gate_lib::tdg};
	std::vector<gate> two_wire = {gate_lib::cx, gate_lib::cy, gate_lib::cz};

	std::vector<angle> commmon_angles = {sym_angle::pi, sym_angle::pi_half,
	                                     sym_angle::pi_quarter};
	for (angle const& a : commmon_angles) {
		one_wire.emplace_back(gate_lib::r1(a));
		one_wire.emplace_back(gate_lib::rx(a));
		one_wire.emplace_back(gate_lib::ry(a));
		one_wire.emplace_back(gate_lib::rz(a));

		two_wire.emplace_back(gate_lib::crx(a));
		two_wire.emplace_back(gate_lib::cry(a));
		two_wire.emplace_back(gate_lib::crz(a));
	}
	for (gate const& g : one_wire) {
		// Create quantum circuit
		TestType circuit;
		wire::id q0 = circuit.create_qubit();
		circuit.create_op(g, q0);
		// Decompose
		TestType decomposed = decompose(circuit, params);
		// Create unitaries
		unitary u_decomp(circuit);
		unitary u(1u);
		u.create_op(g, q0);
		CHECK(u.is_apprx_equal(u_decomp));
	}
	for (gate const& g : two_wire) {
		// Create quantum circuit
		TestType circuit;
		wire::id q0 = circuit.create_qubit();
		wire::id q1 = circuit.create_qubit();
		circuit.create_op(g, q0, q1);
		// Decompose
		TestType decomposed = decompose(circuit, params);
		// Create unitaries
		unitary u_decomp(circuit);
		unitary u(2u);
		u.create_op(g, q0, q1);
		CHECK(u.is_apprx_equal(u_decomp));
	}

	// Parameterisable
}