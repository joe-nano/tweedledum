/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/CircuitDAG.h"

#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Node.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/ir/operations/w3_op.h"
#include "tweedledum/ir/operations/wn32_op.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_PRODUCT_TEST_CASE("Operations DAG 'foreach_input' iterator",
    "[CircuitDAG][template]", (CircuitDAG), (w3_op, wn32_op))
{
	using node_type = typename TestType::node_type;
	TestType network;
	SECTION("Input iterator")
	{
		network.create_qubit();
		network.create_qubit();
		network.create_qubit();
		network.create_qubit();
		uint32_t i = 0u;
		network.foreach_input([&](node::Id const id) {
			CHECK(id == i);
			++i;
		});
		i = 0u;
		network.foreach_input(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::input));
			    CHECK(id == i);
			    ++i;
		    });
		i = 0u;
		network.foreach_input([&](node_type const& node) {
			CHECK(node.op.is(gate_ids::input));
		});
	}
	SECTION("Output iterator")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		uint32_t i = 0u;
		network.foreach_output([&](node::Id const id) {
			CHECK(id == i);
			++i;
		});
		i = 0u;
		network.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::input));
			    CHECK(id == i);
			    ++i;
		    });
		i = 0u;
		network.foreach_output([&](node_type const& node) {
			CHECK(node.op.is(gate_ids::input));
		});

		node::Id n = network.create_op(GateLib::ncx, q0, q1, q2);
		network.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::ncx));
			    CHECK(id == n);
		    });
	}
}

TEMPLATE_PRODUCT_TEST_CASE("Operations 'foreach_output' iterator",
    "[CircuitDAG][template]", (CircuitDAG), (w3_op, wn32_op))
{
	using node_type = typename TestType::node_type;
	TestType network;
	wire::Id q0 = network.create_qubit();
	wire::Id q1 = network.create_qubit();
	wire::Id q2 = network.create_qubit();
	SECTION("No operations")
	{
		uint32_t i = 0u;
		network.foreach_output([&](node::Id const id) {
			CHECK(id == i);
			++i;
		});
		i = 0u;
		network.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::input));
			    CHECK(id == i);
			    ++i;
		    });
		i = 0u;
		network.foreach_output([&](node_type const& node) {
			CHECK(node.op.is(gate_ids::input));
		});
	}
	SECTION("One operation")
	{
		node::Id n = network.create_op(GateLib::ncx, q0, q1, q2);
		network.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::ncx));
			    CHECK(id == n);
		    });
	}
	SECTION("One operation")
	{
		network.create_op(GateLib::cx, q1, q0);
		node::Id n1 = network.create_op(GateLib::cx, q1, q2);
		node::Id n2 = network.create_op(GateLib::cx, q2, q0);

		uint32_t i = 0;
		std::array<node::Id, 3> node_ids = {n2, n1, n2};
		network.foreach_output(
		    [&](node_type const& node, node::Id const id) {
			    CHECK(node.op.is(gate_ids::cx));
			    CHECK(id == node_ids.at(i));
			    ++i;
		    });
	}
}