/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/Circuit.h"

#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Node.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Operations DAG 'foreach_input' iterator", "[Circuit]")
{
	using node_type = typename Circuit::node_type;
	Module module;
	Circuit& circuit = module.circuit_;
	SECTION("Input iterator")
	{
		circuit.create_qubit();
		circuit.create_qubit();
		circuit.create_qubit();
		circuit.create_qubit();
		uint32_t i = 0u;
		circuit.foreach_input([&](node::Id const id) {
			CHECK(id == i);
			++i;
		});
		i = 0u;
		circuit.foreach_input(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::input));
			    CHECK(id == i);
			    ++i;
		    });
		i = 0u;
		circuit.foreach_input([&](node_type const& node) {
			CHECK(node.op.is(gate_ids::input));
		});
	}
	SECTION("Output iterator")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		uint32_t i = 0u;
		circuit.foreach_output([&](node::Id const id) {
			CHECK(id == i);
			++i;
		});
		i = 0u;
		circuit.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::input));
			    CHECK(id == i);
			    ++i;
		    });
		i = 0u;
		circuit.foreach_output([&](node_type const& node) {
			CHECK(node.op.is(gate_ids::input));
		});

		node::Id n = circuit.create_op(GateLib::ncx, q0, q1, q2);
		circuit.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::ncx));
			    CHECK(id == n);
		    });
	}
}

TEST_CASE("Operations 'foreach_output' iterator", "[Circuit]")
{
	using node_type = typename Circuit::node_type;
	Module module;
	Circuit& circuit = module.circuit_;
	wire::Id q0 = circuit.create_qubit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	SECTION("No operations")
	{
		uint32_t i = 0u;
		circuit.foreach_output([&](node::Id const id) {
			CHECK(id == i);
			++i;
		});
		i = 0u;
		circuit.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::input));
			    CHECK(id == i);
			    ++i;
		    });
		i = 0u;
		circuit.foreach_output([&](node_type const& node) {
			CHECK(node.op.is(gate_ids::input));
		});
	}
	SECTION("One operation")
	{
		node::Id n = circuit.create_op(GateLib::ncx, q0, q1, q2);
		circuit.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::ncx));
			    CHECK(id == n);
		    });
	}
	SECTION("One operation")
	{
		circuit.create_op(GateLib::cx, q1, q0);
		node::Id n1 = circuit.create_op(GateLib::cx, q1, q2);
		node::Id n2 = circuit.create_op(GateLib::cx, q2, q0);

		uint32_t i = 0;
		std::array<node::Id, 3> node_ids = {n2, n1, n2};
		circuit.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::cx));
			    CHECK(id == node_ids.at(i));
			    ++i;
		    });
	}
}