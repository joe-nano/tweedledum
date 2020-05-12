/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/optimization/gate_cancellation.h"

#include "tweedledum/algorithms/verification/unitary_verify.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/ir/operations/w3_op.h"
#include "tweedledum/ir/operations/wn32_op.h"

#include <catch.hpp>
#include <vector>

using namespace tweedledum;

TEMPLATE_PRODUCT_TEST_CASE("Simple gate cancellations",
    "[gate_cancellation][optmization]", (CircuitDAG), (w3_op, wn32_op))
{
	TestType network;
	SECTION("Single qubit gates")
	{
		wire::Id q0 = network.create_qubit("q0");
		wire::Id q1 = network.create_qubit();
		network.create_op(GateLib::h, q0);
		network.create_op(GateLib::h, q0);
		network.create_op(GateLib::h, q1);
		network.create_op(GateLib::t, q1);
		network.create_op(GateLib::tdg, q1);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("1")
	{
		wire::Id q0 = network.create_qubit("q0");
		wire::Id q1 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q1, q0);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("2")
	{
		wire::Id q0 = network.create_qubit("q0");
		wire::Id q1 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::t, q0);
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q1, q0);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 2);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("3")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q2);
		network.create_op(GateLib::cx, q1, q0);
		network.create_op(GateLib::cx, q1, q0);
		network.create_op(GateLib::cx, q0, q2);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("4")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q0, q2);
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q0, q2);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("5")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		network.create_op(GateLib::cx, q2, q1);
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q2, q1);
		network.create_op(GateLib::cx, q0, q1);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("6")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		network.create_op(GateLib::cx, q2, q1);
		network.create_op(GateLib::cx, q0, q2);
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q2, q1);
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::cx, q0, q2);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 4);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		network.create_op(GateLib::ncz, q0, q1, q2);
		network.create_op(GateLib::ncx, q0, q1, q2);
		network.create_op(GateLib::ncx, q0, q1, q2);
		network.create_op(GateLib::ncz, q0, q1, q2);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::ncx, q0, q1, q2);
		network.create_op(GateLib::cx, q0, q1);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 3);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q2);
		network.create_op(GateLib::ncx, q0, q1, q2);
		network.create_op(GateLib::cx, q0, q2);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		wire::Id q3 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q1);
		network.create_op(GateLib::ncx, q1, q2, q3);
		network.create_op(GateLib::cx, q0, q1);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 3);
		CHECK(unitary_verify(network, optmized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		wire::Id q3 = network.create_qubit();
		wire::Id q4 = network.create_qubit();
		network.create_op(GateLib::cx, q0, q2);
		network.create_op(GateLib::cx, q1, q2);
		network.create_op(GateLib::ncx, q2, q3, q4);
		network.create_op(GateLib::cx, q1, q2);
		network.create_op(GateLib::cx, q0, q2);
		network.create_op(GateLib::cx, q0, q2);
		network.create_op(GateLib::ncx, q2, q3, q4);
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 5);
		CHECK(unitary_verify(network, optmized));
	}
}

TEMPLATE_PRODUCT_TEST_CASE("Even Sequences", "[gate_cancellation][optmization]",
    (CircuitDAG), (wn32_op, w3_op))
{
	TestType network;
	SECTION("Even sequece of hadamards")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			network.create_op(GateLib::h, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
	}
	SECTION("Even sequece of pauli-x")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			network.create_op(GateLib::x, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
	}
	SECTION("Even sequece of pauli-z")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			network.create_op(GateLib::z, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
	}
	SECTION("Even sequece of pauli-y")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			network.create_op(GateLib::y, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
	}
	SECTION("Even sequece of cx")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			network.create_op(GateLib::cx, q0, q1);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
	}
	SECTION("Even sequece of cx")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			network.create_op(GateLib::cx, q0, q1);
			network.create_op(GateLib::cx, q2, q1);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 0);
	}
}

TEMPLATE_PRODUCT_TEST_CASE("Odd Sequences", "[gate_cancellation][optmization]",
    (CircuitDAG), (wn32_op, w3_op))
{
	TestType network;
	SECTION("Odd sequece of hadamards")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			network.create_op(GateLib::h, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
	}
	SECTION("Odd sequece of pauli-x")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			network.create_op(GateLib::x, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
	}
	SECTION("Odd sequece of pauli-z")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			network.create_op(GateLib::z, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
	}
	SECTION("Odd sequece of pauli-y")
	{
		wire::Id q0 = network.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			network.create_op(GateLib::y, q0);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
	}
	SECTION("Odd sequece of cx")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			network.create_op(GateLib::cx, q0, q1);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 1);
	}
	SECTION("Odd sequece of cx")
	{
		wire::Id q0 = network.create_qubit();
		wire::Id q1 = network.create_qubit();
		wire::Id q2 = network.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			network.create_op(GateLib::cx, q0, q1);
			network.create_op(GateLib::cx, q2, q1);
		}
		auto optmized = gate_cancellation(network);
		CHECK(optmized.num_operations() == 2);
	}
}
