/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/views/LayersView.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Layers view", "[LayersView][views]")
{
	Circuit circuit(nullptr);
	SECTION("Empty circuit")
	{
		LayersView layered_ntk(circuit);
		CHECK(layered_ntk.depth() == 0u);
		CHECK(layered_ntk.num_layers() == 0u);
	}

	wire::Id const q0 = circuit.create_qubit();
	wire::Id const q1 = circuit.create_qubit();
	wire::Id const q2 = circuit.create_qubit();
	SECTION("With qubits, but no gates")
	{
		LayersView layered_ntk(circuit);
		CHECK(layered_ntk.depth() == 0u);
		CHECK(layered_ntk.num_layers() == 1u);
		CHECK(layered_ntk.layer(0).size() == 3u);
	}
	SECTION("One layer of gates")
	{
		circuit.create_op(GateLib::h, q0);
		circuit.create_op(GateLib::cx, q1, q2);

		LayersView layered_ntk(circuit);
		CHECK(layered_ntk.depth() == 1u);
		CHECK(layered_ntk.num_layers() == 2u);
		CHECK(layered_ntk.layer(0).size() == 3u);
		CHECK(layered_ntk.layer(1).size() == 2u);
	}
	SECTION("Two layer of gates")
	{
		circuit.create_op(GateLib::h, q2);
		circuit.create_op(GateLib::cx, q0, q1);
		circuit.create_op(GateLib::cx, q2, q1);
		circuit.create_op(GateLib::h, q0);

		LayersView layered_ntk(circuit);
		CHECK(layered_ntk.depth() == 2u);
		CHECK(layered_ntk.num_layers() == 3u);
		CHECK(layered_ntk.layer(0).size() == 3u);
		CHECK(layered_ntk.layer(1).size() == 2u);
		CHECK(layered_ntk.layer(2).size() == 2u);
	}
	SECTION("All outputs are in the last layer")
	{
		wire::Id const q3 = circuit.create_qubit();
		circuit.create_op(GateLib::cx, q1, q2);
		circuit.create_op(GateLib::cx, q2, q3);
		circuit.create_op(GateLib::cx, q0, q3);
		circuit.create_op(GateLib::h, q3);

		LayersView layered_ntk(circuit);
		CHECK(layered_ntk.depth() == 4u);
		CHECK(layered_ntk.num_layers() == 5u);
		CHECK(layered_ntk.layer(0).size() == 4u);
		CHECK(layered_ntk.layer(1).size() == 1u);
		CHECK(layered_ntk.layer(2).size() == 1u);
		CHECK(layered_ntk.layer(3).size() == 1u);
		CHECK(layered_ntk.layer(4).size() == 1u);
	}
}