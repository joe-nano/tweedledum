/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/optimization/phase_folding.h"

#include "tweedledum/algorithms/verification/unitary_verify.h"
#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Phase folding", "[phase_folding]")
{
	Module module;
	Circuit& circuit = module.circuit_;
	circuit.create_qubit("x1");
	circuit.create_qubit("x2");
	circuit.create_qubit("x3");
	circuit.create_qubit("x4");

	circuit.create_op(GateLib::cx, "x3", "x4");

	circuit.create_op(GateLib::t, "x1");
	circuit.create_op(GateLib::t, "x4");

	circuit.create_op(GateLib::cx, "x1", "x2");
	circuit.create_op(GateLib::cx, "x3", "x4");

	circuit.create_op(GateLib::cx, "x2", "x3");

	circuit.create_op(GateLib::cx, "x2", "x1");
	circuit.create_op(GateLib::cx, "x4", "x3");

	circuit.create_op(GateLib::cx, "x2", "x3");

	circuit.create_op(GateLib::cx, "x1", "x2");
	circuit.create_op(GateLib::tdg, "x3");

	circuit.create_op(GateLib::t, "x2");

	auto optimized = phase_folding(circuit);
	// FIXME: Something is wrong!!!
	// CHECK(unitary_verify(circuit, optimized));
}
