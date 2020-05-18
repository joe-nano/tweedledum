/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/placement/random_placement.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Test for random intial placement heuristic", "[random_placement][mapping]")
{
	Circuit circuit(nullptr);
	SECTION("Empty circuit")
	{
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::random_placement(device);
		CHECK(placement.size() == 0u);
	}
	SECTION("Simple circuit (UNSAT)")
	{
		wire::Id q0 = circuit.create_qubit();
		circuit.create_cbit();
		wire::Id q1 = circuit.create_qubit();
		circuit.create_cbit();
		wire::Id q2 = circuit.create_qubit();
		circuit.create_cbit();

		circuit.create_op(GateLib::cx, q1, q0);
		circuit.create_op(GateLib::cx, q1, q2);
		circuit.create_op(GateLib::cx, q2, q0);

		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::random_placement(device);
		CHECK(placement.size() == 3u);
	}
}
