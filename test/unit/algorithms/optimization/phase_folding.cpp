/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/optimization/phase_folding.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Operation.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_TEST_CASE(
    "Phase folding", "[phase_folding][template]", Netlist, CircuitDAG)
{
	TestType network;
	network.create_qubit("x1");
	network.create_qubit("x2");
	network.create_qubit("x3");
	network.create_qubit("x4");

	network.create_op(GateLib::cx, "x3", "x4");

	network.create_op(GateLib::t, "x1");
	network.create_op(GateLib::t, "x4");

	network.create_op(GateLib::cx, "x1", "x2");
	network.create_op(GateLib::cx, "x3", "x4");

	network.create_op(GateLib::cx, "x2", "x3");

	network.create_op(GateLib::cx, "x2", "x1");
	network.create_op(GateLib::cx, "x4", "x3");

	network.create_op(GateLib::cx, "x2", "x3");

	network.create_op(GateLib::cx, "x1", "x2");
	network.create_op(GateLib::tdg, "x3");

	network.create_op(GateLib::t, "x2");

	auto opt_network = phase_folding(network);
	// CHECK(check_optimized(network, opt_network));
}
