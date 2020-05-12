/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/diagonal_synth.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/operations/w3_op.h"
#include "tweedledum/ir/operations/wn32_op.h"
#include "tweedledum/support/Angle.h"

#include <catch.hpp>
#include <sstream>

using namespace tweedledum;
TEMPLATE_PRODUCT_TEST_CASE("Synthesize diagonal unitaries",
    "[diagonal_synth][template]", (Netlist, CircuitDAG), (w3_op, wn32_op))
{
	SECTION("One-qubit")
	{
		std::vector<Angle> angles = {sym_angle::zero, sym_angle::pi};
		TestType network = diagonal_synth<TestType>(angles);
		// CHECK(network.num_qubits() == 1u);
		// CHECK(network.num_operations() == 1u);
	}
	SECTION("Two-qubit controlled r1(pi)")
	{
		std::vector<Angle> angles = {sym_angle::zero, sym_angle::zero,
		    sym_angle::zero, sym_angle::pi};
		TestType network = diagonal_synth<TestType>(angles);
		// CHECK(network.num_qubits() == 2u);
		// CHECK(network.num_operations() == 5u);
	}
	SECTION("Two-qubit controlled rz(pi/2)")
	{
		std::vector<Angle> angles = {sym_angle::zero, sym_angle::zero,
		    -sym_angle::pi_half, sym_angle::pi_half};
		TestType network = diagonal_synth<TestType>(angles);
		// CHECK(network.num_qubits() == 2u);
		// CHECK(network.num_operations() == 2u);
	}
	SECTION("Three-qubit Toffoli")
	{
		std::vector<Angle> angles(7, sym_angle::zero);
		angles.push_back(sym_angle::pi_half);
		TestType network = diagonal_synth<TestType>(angles);
		// CHECK(network.num_qubits() == 3u);
	}
	SECTION("Three-qubit Toffoli")
	{
		std::vector<Angle> angles(6, sym_angle::zero);
		angles.push_back(-sym_angle::pi);
		angles.push_back(sym_angle::pi);
		TestType network = diagonal_synth<TestType>(angles);
		// CHECK(network.num_qubits() == 3u);
		// std::ostringstream os;
		// write_qasm(network, os);
		// std::cout << os.str();
	}
}
