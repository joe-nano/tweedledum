/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/placement/line_placement.h"

#include "../test_circuits.h"
#include "tweedledum/algorithms/verification/placement_verify.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>
#include <vector>

using namespace tweedledum;

TEST_CASE("Test for line intial placement heuristic", "[line_placement][mapping]")
{
	CircuitDAG circuit(nullptr);
	SECTION("Empty circuit")
	{
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 00")
	{
		circuit = test_circuit_00();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 01")
	{
		circuit = test_circuit_01();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 02")
	{
		circuit = test_circuit_02();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 03")
	{
		circuit = test_circuit_03();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 04")
	{
		circuit = test_circuit_04();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 05")
	{
		circuit = test_circuit_05();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 06")
	{
		circuit = test_circuit_06();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 07")
	{
		circuit = test_circuit_07();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::line_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
}
