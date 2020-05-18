/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Node.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Common functionality for all circuits", "[common][circuits]")
{
	Module module;
	CircuitDAG& circuit = module.circuit_;
	SECTION("An empty circuit")
	{
		CHECK(circuit.size() == 0u);
		CHECK(circuit.num_wires() == 0u);
		CHECK(circuit.num_qubits() == 0u);
		CHECK(circuit.num_cbits() == 0u);
		CHECK(circuit.num_operations() == 0u);
	}
	SECTION("Reserving space")
	{
		uint32_t cap = circuit.capacity();
		circuit.reserve(cap << 2);
		CHECK(circuit.size() == 0u);
		CHECK(circuit.capacity() == (cap << 2));
		CHECK(circuit.num_wires() == 0u);
		CHECK(circuit.num_qubits() == 0u);
		CHECK(circuit.num_cbits() == 0u);
		CHECK(circuit.num_operations() == 0u);
	}
	SECTION("Create one of each wire type")
	{
		wire::Id qubit = circuit.create_qubit("qubit");
		CHECK(circuit.size() == 1u);
		CHECK(circuit.num_wires() == 1u);
		CHECK(circuit.num_qubits() == 1u);
		CHECK(circuit.num_cbits() == 0);
		wire::Id find = circuit.wire("qubit");
		CHECK(find == qubit);

		wire::Id cbit = circuit.create_cbit("cbit");
		CHECK(circuit.size() == 2u);
		CHECK(circuit.num_wires() == 2u);
		CHECK(circuit.num_qubits() == 1u);
		CHECK(circuit.num_cbits() == 1u);
		find = circuit.wire("cbit");
		CHECK(find == cbit);
	}
	SECTION("Creating wires")
	{
		for (uint32_t i = 0u; i < 8u; ++i) {
			std::string qname = fmt::format("q{}", i);
			std::string cname = fmt::format("c{}", i);
			wire::Id nqubit = circuit.create_qubit(qname);
			wire::Id qubit = circuit.create_qubit();
			wire::Id ncbit = circuit.create_cbit(cname);
			wire::Id cbit = circuit.create_cbit();

			CHECK(circuit.size() == ((i + 1) * 4));
			CHECK(circuit.num_wires() == ((i + 1) * 4));
			CHECK(circuit.num_qubits() == ((i + 1) * 2));
			CHECK(circuit.num_cbits() == ((i + 1) * 2));

			CHECK(circuit.wire_name(nqubit) == qname);
			CHECK(circuit.wire_name(qubit)
			      == fmt::format("__dum_q{}", (2 * i) + 1));
			CHECK(circuit.wire_name(!nqubit)
			      == circuit.wire_name(nqubit));
			CHECK(circuit.wire_name(!qubit)
			      == circuit.wire_name(qubit));

			CHECK(circuit.wire_name(ncbit) == cname);
			CHECK(circuit.wire_name(cbit)
			      == fmt::format("__dum_c{}", (2 * i) + 1));
			CHECK(circuit.wire_name(!ncbit)
			      == circuit.wire_name(ncbit));
			CHECK(circuit.wire_name(!cbit)
			      == circuit.wire_name(cbit));
		}
		CHECK(circuit.size() == 32u);
		CHECK(circuit.num_wires() == 32u);
		CHECK(circuit.num_qubits() == 16u);
		CHECK(circuit.num_cbits() == 16u);
		CHECK(circuit.num_operations() == 0u);
	}
}

TEST_CASE("One-qubit operations", "[one-qubit][circuits]")
{
	std::vector<Gate> gates
	    = {GateLib::i, GateLib::h, GateLib::x, GateLib::y, GateLib::z,
	        GateLib::s, GateLib::t, GateLib::sdg, GateLib::tdg};

	Module module;
	CircuitDAG& circuit = module.circuit_;
	wire::Id qubit = circuit.create_qubit("qubit_0");
	SECTION("Using wire identifier")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = circuit.create_op(gates.at(i), qubit);
			auto node = circuit.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.target() == qubit);
			CHECK(circuit.num_operations() == (i + 1));
		}
	}
	SECTION("Using wire name")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = circuit.create_op(gates.at(i), "qubit_"
			                                               "0");
			auto node = circuit.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.target() == qubit);
			CHECK(circuit.num_operations() == (i + 1));
		}
	}
}

TEST_CASE("Two-qubit operations", "[two-qubit][circuits]")
{
	std::vector<Gate> gates
	    = {GateLib::cx, GateLib::cy, GateLib::cz, GateLib::swap};

	Module module;
	CircuitDAG& circuit = module.circuit_;
	wire::Id q0 = circuit.create_qubit("__dum_q0");
	wire::Id q1 = circuit.create_qubit("__dum_q1");
	SECTION("Using wire identifier")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = circuit.create_op(gates.at(i), q0, q1);
			auto node = circuit.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			if (gates.at(i).id() == gate_ids::swap) {
				CHECK(node.op.target(0) == q0);
				CHECK(node.op.target(1) == q1);
			} else {
				CHECK(node.op.control() == q0);
				CHECK(node.op.target() == q1);
			}
			CHECK(circuit.num_operations() == (i + 1));
		}
	}
	SECTION("Using wire name")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = circuit.create_op(
			    gates.at(i), "__dum_q0", "__dum_q1");
			auto node = circuit.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			if (gates.at(i).id() == gate_ids::swap) {
				CHECK(node.op.target(0) == q0);
				CHECK(node.op.target(1) == q1);
			} else {
				CHECK(node.op.control() == q0);
				CHECK(node.op.target() == q1);
			}
			CHECK(circuit.num_operations() == (i + 1));
		}
	}
}

TEST_CASE("Three-qubit operations", "[three-qubit][circuits]")
{
	std::vector<Gate> gates = {GateLib::ncx, GateLib::ncy, GateLib::ncz};

	Module module;
	CircuitDAG& circuit = module.circuit_;
	wire::Id q0 = circuit.create_qubit("__dum_q0");
	wire::Id q1 = circuit.create_qubit("__dum_q1");
	wire::Id q2 = circuit.create_qubit("q2");
	SECTION("Using wire identifier")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id
			    = circuit.create_op(gates.at(i), q0, q1, q2);
			auto node = circuit.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.control(0) == q0);
			CHECK(node.op.control(1) == q1);
			CHECK(node.op.target() == q2);
			CHECK(circuit.num_operations() == (i + 1));
		}
	}
	SECTION("Using wire name")
	{
		for (uint32_t i = 0; i < gates.size(); ++i) {
			node::Id n_id = circuit.create_op(
			    gates.at(i), "__dum_q0", "__dum_q1", "q2");
			auto node = circuit.node(n_id);
			CHECK(node.op.id() == gates.at(i).id());
			CHECK(node.op.control(0) == q0);
			CHECK(node.op.control(1) == q1);
			CHECK(node.op.target() == q2);
			CHECK(circuit.num_operations() == (i + 1));
		}
	}
}