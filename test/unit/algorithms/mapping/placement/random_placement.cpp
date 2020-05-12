/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/placement/random_placement.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/ir/operations/w3_op.h"
#include "tweedledum/ir/operations/wn32_op.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_PRODUCT_TEST_CASE("Test for random intial placement heuristic",
    "[random_placement][mapping]", (Netlist, CircuitDAG), (w3_op, wn32_op))
{
	TestType network;
	SECTION("Empty network")
	{
		Device device = Device::path(network.num_qubits());
		std::vector<wire::Id> placement
		    = detail::random_placement(device);
		CHECK(placement.size() == 0u);
	}
	SECTION("Simple circuit (UNSAT)")
	{
		wire::Id q0 = network.create_qubit();
		network.create_cbit();
		wire::Id q1 = network.create_qubit();
		network.create_cbit();
		wire::Id q2 = network.create_qubit();
		network.create_cbit();

		network.create_op(GateLib::cx, q1, q0);
		network.create_op(GateLib::cx, q1, q2);
		network.create_op(GateLib::cx, q2, q0);

		Device device = Device::path(network.num_qubits());
		std::vector<wire::Id> placement
		    = detail::random_placement(device);
		CHECK(placement.size() == 3u);
	}
}
