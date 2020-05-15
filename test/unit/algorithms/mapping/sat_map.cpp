/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/sat_map.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_TEST_CASE(
    "Test for SAT mapper", "[sat_map][mapping]", Netlist, CircuitDAG)
{
	TestType network;
	SECTION("Empty network")
	{
		Device device = Device::path(network.num_qubits());
		auto mapped_ntk = sat_map(network, device);
		CHECK(mapped_ntk.size() == 0u);
		CHECK(mapped_ntk.num_wires() == 0u);
		CHECK(mapped_ntk.num_qubits() == 0u);
		CHECK(mapped_ntk.num_cbits() == 0u);
		CHECK(mapped_ntk.num_operations() == 0u);
	}
	SECTION("Network with no operations")
	{
		network.create_qubit();
		network.create_cbit();
		network.create_qubit();
		network.create_cbit();
		network.create_qubit();
		network.create_cbit();
		Device device = Device::path(network.num_qubits());
		auto mapped_ntk = sat_map(network, device);
		CHECK(mapped_ntk.size() == network.size());
		CHECK(mapped_ntk.num_wires() == network.num_wires());
		CHECK(mapped_ntk.num_qubits() == network.num_qubits());
		CHECK(mapped_ntk.num_cbits() == network.num_cbits());
		CHECK(mapped_ntk.num_operations() == 0u);
	}
	SECTION("Simple circuit (SAT)")
	{
		wire::Id q0 = network.create_qubit();
		network.create_cbit();
		wire::Id q1 = network.create_qubit();
		network.create_cbit();
		wire::Id q2 = network.create_qubit();
		network.create_cbit();

		network.create_op(GateLib::cx, q1, q0);
		network.create_op(GateLib::cx, q2, q0);

		Device device = Device::path(network.num_qubits());
		auto mapped_ntk = sat_map(network, device);
		CHECK(mapped_ntk.num_operations() == 2u);
	}
	SECTION("Simple circuit (UNSAT)")
	{
		wire::Id q0 = network.create_qubit();
		network.create_cbit();
		wire::Id q1 = network.create_qubit();
		network.create_cbit();
		wire::Id q2 = network.create_qubit();
		network.create_cbit();

		network.create_op(GateLib::cx, q1, q0);
		network.create_op(GateLib::cx, q1, q2);
		network.create_op(GateLib::cx, q2, q0);

		Device device = Device::path(network.num_qubits());
		auto mapped_ntk = sat_map(network, device);
		CHECK(mapped_ntk.num_operations() == 0u);
	}
}
