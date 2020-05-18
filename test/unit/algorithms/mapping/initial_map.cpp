/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/initial_map.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_TEST_CASE("Test for SAT intial map heuristic",
    "[initial_map][mapping]", (Netlist, Circuit))
{
	TestType network;
	initial_map_params params;
	params.method = initial_map_params::methods::greedy_sat;

	SECTION("Empty network")
	{
		Device device = Device::path(network.num_qubits());
		std::vector<wire::Id> map = initial_map(network, device);
		CHECK(map.size() == 0u);
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
		std::vector<wire::Id> map
		    = initial_map(network, device, params);
		CHECK(map.size() == 3u);
	}
}

TEMPLATE_TEST_CASE("Test for random intial map heuristic",
    "[initial_map][mapping]", (Netlist, Circuit))
{
	TestType network;
	initial_map_params params;
	params.method = initial_map_params::methods::random;

	SECTION("Empty network")
	{
		Device device = Device::path(network.num_qubits());
		std::vector<wire::Id> map = initial_map(network, device);
		CHECK(map.size() == 0u);
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
		std::vector<wire::Id> map
		    = initial_map(network, device, params);
		CHECK(map.size() == 3u);
	}
}
