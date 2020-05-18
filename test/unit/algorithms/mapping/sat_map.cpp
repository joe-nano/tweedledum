/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/sat_map.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Test for SAT mapper", "[sat_map][mapping]")
{
	CircuitDAG circuit(nullptr);
	SECTION("Empty circuit")
	{
		Device device = Device::path(circuit.num_qubits());
		auto mapped = sat_map(circuit, device);
		CHECK(mapped.size() == 0u);
		CHECK(mapped.num_wires() == 0u);
		CHECK(mapped.num_qubits() == 0u);
		CHECK(mapped.num_cbits() == 0u);
		CHECK(mapped.num_operations() == 0u);
	}
	SECTION("Network with no operations")
	{
		circuit.create_qubit();
		circuit.create_cbit();
		circuit.create_qubit();
		circuit.create_cbit();
		circuit.create_qubit();
		circuit.create_cbit();
		Device device = Device::path(circuit.num_qubits());
		auto mapped = sat_map(circuit, device);
		CHECK(mapped.size() == circuit.size());
		CHECK(mapped.num_wires() == circuit.num_wires());
		CHECK(mapped.num_qubits() == circuit.num_qubits());
		CHECK(mapped.num_cbits() == circuit.num_cbits());
		CHECK(mapped.num_operations() == 0u);
	}
	SECTION("Simple circuit (SAT)")
	{
		wire::Id q0 = circuit.create_qubit();
		circuit.create_cbit();
		wire::Id q1 = circuit.create_qubit();
		circuit.create_cbit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_cbit();

		circuit.create_op(GateLib::cx, q1, q0);
		circuit.create_op(GateLib::cx, q2, q0);

		Device device = Device::path(circuit.num_qubits());
		auto mapped = sat_map(circuit, device);
		CHECK(mapped.num_operations() == 2u);
	}
	SECTION("Simple circuit (UNSAT)")
	{
		wire::Id q0 = circuit.create_qubit();
		circuit.create_cbit();
		wire::Id q1 = circuit.create_qubit();
		circuit.create_cbit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_cbit();

		circuit.create_op(GateLib::cx, q1, q0);
		circuit.create_op(GateLib::cx, q1, q2);
		circuit.create_op(GateLib::cx, q2, q0);

		Device device = Device::path(circuit.num_qubits());
		auto mapped = sat_map(circuit, device);
		CHECK(mapped.num_operations() == 0u);
	}
}
