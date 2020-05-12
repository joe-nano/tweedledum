/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Node.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/ir/operations/w3_op.h"
#include "tweedledum/ir/operations/wn32_op.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_PRODUCT_TEST_CASE("Common functionality for all networks",
    "[common][networks]", (Netlist, CircuitDAG), (w3_op, wn32_op))
{
	TestType network;
	SECTION("An empty network")
	{
		CHECK(network.size() == 0u);
		CHECK(network.num_wires() == 0u);
		CHECK(network.num_qubits() == 0u);
		CHECK(network.num_cbits() == 0u);
		CHECK(network.num_operations() == 0u);
	}
	SECTION("Reserving space")
	{
		uint32_t cap = network.capacity();
		network.reserve(cap << 2);
		CHECK(network.size() == 0u);
		CHECK(network.capacity() == (cap << 2));
		CHECK(network.num_wires() == 0u);
		CHECK(network.num_qubits() == 0u);
		CHECK(network.num_cbits() == 0u);
		CHECK(network.num_operations() == 0u);
	}
	SECTION("Create one of each wire type")
	{
		wire::Id qubit = network.create_qubit("qubit");
		CHECK(network.size() == 1u);
		CHECK(network.num_wires() == 1u);
		CHECK(network.num_qubits() == 1u);
		CHECK(network.num_cbits() == 0);
		wire::Id find = network.wire("qubit");
		CHECK(find == qubit);

		wire::Id cbit = network.create_cbit("cbit");
		CHECK(network.size() == 2u);
		CHECK(network.num_wires() == 2u);
		CHECK(network.num_qubits() == 1u);
		CHECK(network.num_cbits() == 1u);
		find = network.wire("cbit");
		CHECK(find == cbit);
	}
	SECTION("Creating wires")
	{
		for (uint32_t i = 0u; i < 8u; ++i) {
			std::string qname = fmt::format("q{}", i);
			std::string cname = fmt::format("c{}", i);
			wire::Id nqubit = network.create_qubit(qname);
			wire::Id qubit = network.create_qubit();
			wire::Id ncbit = network.create_cbit(cname);
			wire::Id cbit = network.create_cbit();

			CHECK(network.size() == ((i + 1) * 4));
			CHECK(network.num_wires() == ((i + 1) * 4));
			CHECK(network.num_qubits() == ((i + 1) * 2));
			CHECK(network.num_cbits() == ((i + 1) * 2));

			CHECK(network.wire_name(nqubit) == qname);
			CHECK(network.wire_name(qubit)
			      == fmt::format("__dum_q{}", (2 * i) + 1));
			CHECK(network.wire_name(!nqubit)
			      == network.wire_name(nqubit));
			CHECK(network.wire_name(!qubit)
			      == network.wire_name(qubit));

			CHECK(network.wire_name(ncbit) == cname);
			CHECK(network.wire_name(cbit)
			      == fmt::format("__dum_c{}", (2 * i) + 1));
			CHECK(network.wire_name(!ncbit)
			      == network.wire_name(ncbit));
			CHECK(network.wire_name(!cbit)
			      == network.wire_name(cbit));
		}
		CHECK(network.size() == 32u);
		CHECK(network.num_wires() == 32u);
		CHECK(network.num_qubits() == 16u);
		CHECK(network.num_cbits() == 16u);
		CHECK(network.num_operations() == 0u);
	}
}

TEMPLATE_PRODUCT_TEST_CASE("One-qubit operations", "[one-qubit][networks]",
    (Netlist, CircuitDAG), (w3_op, wn32_op))
{
	std::vector<Gate> gates
	    = {GateLib::i, GateLib::h, GateLib::x, GateLib::y, GateLib::z,
	        GateLib::s, GateLib::t, GateLib::sdg, GateLib::tdg};

	TestType network;
	wire::Id qubit = network.create_qubit("qubit_0");
	SECTION("Using wire identifier")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = network.create_op(gates.at(i), qubit);
			auto node = network.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.target() == qubit);
			CHECK(network.num_operations() == (i + 1));
		}
	}
	SECTION("Using wire name")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = network.create_op(gates.at(i), "qubit_"
			                                               "0");
			auto node = network.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.target() == qubit);
			CHECK(network.num_operations() == (i + 1));
		}
	}
}

TEMPLATE_PRODUCT_TEST_CASE("Two-qubit operations", "[two-qubit][networks]",
    (Netlist, CircuitDAG), (w3_op, wn32_op))
{
	std::vector<Gate> gates
	    = {GateLib::cx, GateLib::cy, GateLib::cz, GateLib::swap};

	TestType network;
	wire::Id q0 = network.create_qubit("__dum_q0");
	wire::Id q1 = network.create_qubit("__dum_q1");
	SECTION("Using wire identifier")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = network.create_op(gates.at(i), q0, q1);
			auto node = network.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			if (gates.at(i).id() == gate_ids::swap) {
				CHECK(node.op.target(0) == q0);
				CHECK(node.op.target(1) == q1);
			} else {
				CHECK(node.op.control() == q0);
				CHECK(node.op.target() == q1);
			}
			CHECK(network.num_operations() == (i + 1));
		}
	}
	SECTION("Using wire name")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = network.create_op(
			    gates.at(i), "__dum_q0", "__dum_q1");
			auto node = network.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			if (gates.at(i).id() == gate_ids::swap) {
				CHECK(node.op.target(0) == q0);
				CHECK(node.op.target(1) == q1);
			} else {
				CHECK(node.op.control() == q0);
				CHECK(node.op.target() == q1);
			}
			CHECK(network.num_operations() == (i + 1));
		}
	}
}

TEMPLATE_PRODUCT_TEST_CASE("Three-qubit operations", "[three-qubit][networks]",
    (Netlist, CircuitDAG), (w3_op, wn32_op))
{
	std::vector<Gate> gates = {GateLib::ncx, GateLib::ncy, GateLib::ncz};

	TestType network;
	wire::Id q0 = network.create_qubit("__dum_q0");
	wire::Id q1 = network.create_qubit("__dum_q1");
	wire::Id q2 = network.create_qubit("q2");
	SECTION("Using wire identifier")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id
			    = network.create_op(gates.at(i), q0, q1, q2);
			auto node = network.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.control(0) == q0);
			CHECK(node.op.control(1) == q1);
			CHECK(node.op.target() == q2);
			CHECK(network.num_operations() == (i + 1));
		}
	}
	SECTION("Using wire name")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = network.create_op(
			    gates.at(i), "__dum_q0", "__dum_q1", "q2");
			auto node = network.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.control(0) == q0);
			CHECK(node.op.control(1) == q1);
			CHECK(node.op.target() == q2);
			CHECK(network.num_operations() == (i + 1));
		}
	}
}