/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/Unitary.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Basic unitary functionality", "[unitary]")
{
	SECTION("An empty unitary")
	{
		Unitary u;
		CHECK(u.num_wires() == 0u);
		CHECK(u.num_qubits() == 0u);
	}
	SECTION("One-qubit unitary")
	{
		Unitary u0(1u);
		CHECK(u0.num_wires() == 1u);
		CHECK(u0.num_qubits() == 1u);

		Unitary u1;
		u1.create_qubit();
		CHECK(u1.num_wires() == 1u);
		CHECK(u1.num_qubits() == 1u);
		CHECK(u0.is_apprx_equal(u1));
		CHECK(u1.is_apprx_equal(u0));
	}
	SECTION("Two-qubit unitary")
	{
		Unitary u0(2u);
		CHECK(u0.num_wires() == 2u);
		CHECK(u0.num_qubits() == 2u);

		Unitary u1;
		u1.create_qubit();
		u1.create_qubit();
		CHECK(u1.num_wires() == 2u);
		CHECK(u1.num_qubits() == 2u);
		CHECK(u0.is_apprx_equal(u1));
		CHECK(u1.is_apprx_equal(u0));
	}
	SECTION("Two-qubit unitary")
	{
		Unitary u0;
		wire::Id q0 = u0.create_qubit();
		u0.create_op(GateLib::x, q0);
		u0.create_qubit();

		Unitary u1(2u);
		u1.create_op(GateLib::x, q0);
		CHECK(u0.is_apprx_equal(u1));
		CHECK(u1.is_apprx_equal(u0));
	}
}