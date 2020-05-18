/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/optimization/gate_cancellation.h"

#include "tweedledum/algorithms/verification/unitary_verify.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>
#include <vector>

using namespace tweedledum;

TEST_CASE("Simple gate cancellations", "[gate_cancellation][opt]")
{
	Module module;
	CircuitDAG& circuit = module.circuit_;
	SECTION("Single qubit gates")
	{
		wire::Id q0 = circuit.create_qubit("q0");
		wire::Id q1 = circuit.create_qubit();
		circuit.create_op(GateLib::h, q0);
		circuit.create_op(GateLib::h, q0);
		circuit.create_op(GateLib::h, q1);
		circuit.create_op(GateLib::t, q1);
		circuit.create_op(GateLib::tdg, q1);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("1")
	{
		wire::Id q0 = circuit.create_qubit("q0");
		wire::Id q1 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q1, q0);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("2")
	{
		wire::Id q0 = circuit.create_qubit("q0");
		wire::Id q1 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::t, q0);
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q1, q0);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 2);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("3")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q2);
		circuit.create_op(GateLib::cx, q1, q0);
		circuit.create_op(GateLib::cx, q1, q0);
		circuit.create_op(GateLib::cx, q0, q2);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("4")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q0, q2);
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q0, q2);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("5")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q2, q1);
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q2, q1);
		circuit.create_op(GateLib::cx, q0, q1);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("6")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q2, q1);
		circuit.create_op(GateLib::cx, q0, q2);
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q2, q1);
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q0, q2);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 4);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_op(GateLib::ncz, q0, q1, q2);
		circuit.create_op(GateLib::ncx, q0, q1, q2);
		circuit.create_op(GateLib::ncx, q0, q1, q2);
		circuit.create_op(GateLib::ncz, q0, q1, q2);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::ncx, q0, q1, q2);
		circuit.create_op(GateLib::cx, q0, q1);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 3);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q2);
		circuit.create_op(GateLib::ncx, q0, q1, q2);
		circuit.create_op(GateLib::cx, q0, q2);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		wire::Id q3 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::ncx, q1, q2, q3);
		circuit.create_op(GateLib::cx, q0, q1);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 3);
		CHECK(unitary_verify(circuit, optimized));
	}
	SECTION("Multiple qubit gates")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		wire::Id q3 = circuit.create_qubit();
		wire::Id q4 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q0, q2);
		circuit.create_op(GateLib::cx, q1, q2);
		circuit.create_op(GateLib::ncx, q2, q3, q4);
		circuit.create_op(GateLib::cx, q1, q2);
		circuit.create_op(GateLib::cx, q0, q2);
		circuit.create_op(GateLib::cx, q0, q2);
		circuit.create_op(GateLib::ncx, q2, q3, q4);
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 5);
		CHECK(unitary_verify(circuit, optimized));
	}
}

TEST_CASE("Even Sequences", "[gate_cancellation][opt]")
{
	Module module;
	CircuitDAG& circuit = module.circuit_;
	SECTION("Even sequece of hadamards")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			circuit.create_op(GateLib::h, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
	}
	SECTION("Even sequece of pauli-x")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			circuit.create_op(GateLib::x, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
	}
	SECTION("Even sequece of pauli-z")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			circuit.create_op(GateLib::z, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
	}
	SECTION("Even sequece of pauli-y")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			circuit.create_op(GateLib::y, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
	}
	SECTION("Even sequece of cx")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			circuit.create_op(GateLib::cx, q0, q1);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
	}
	SECTION("Even sequece of cx")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1024; ++i) {
			circuit.create_op(GateLib::cx, q0, q1);
			circuit.create_op(GateLib::cx, q2, q1);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 0);
	}
}

TEST_CASE("Odd Sequences", "[gate_cancellation][opt]")
{
	Module module;
	CircuitDAG& circuit = module.circuit_;
	SECTION("Odd sequece of hadamards")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			circuit.create_op(GateLib::h, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
	}
	SECTION("Odd sequece of pauli-x")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			circuit.create_op(GateLib::x, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
	}
	SECTION("Odd sequece of pauli-z")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			circuit.create_op(GateLib::z, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
	}
	SECTION("Odd sequece of pauli-y")
	{
		wire::Id q0 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			circuit.create_op(GateLib::y, q0);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
	}
	SECTION("Odd sequece of cx")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			circuit.create_op(GateLib::cx, q0, q1);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 1);
	}
	SECTION("Odd sequece of cx")
	{
		wire::Id q0 = circuit.create_qubit();
		wire::Id q1 = circuit.create_qubit();
		wire::Id q2 = circuit.create_qubit();
		for (uint32_t i = 0; i < 1023; ++i) {
			circuit.create_op(GateLib::cx, q0, q1);
			circuit.create_op(GateLib::cx, q2, q1);
		}
		auto optimized = gate_cancellation(circuit);
		CHECK(optimized.num_operations() == 2);
	}
}
