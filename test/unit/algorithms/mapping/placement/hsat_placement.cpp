/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/placement/hsat_placement.h"

#include "../test_circuits.h"
#include "tweedledum/algorithms/verification/placement_verify.h"
#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Test for SAT intial placement heuristic", "[hsat_placement][mapping]")
{
	SECTION("Empty circuit")
	{
		Circuit circuit(nullptr);
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 00")
	{
		Circuit circuit = test_circuit_00();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 01")
	{
		Circuit circuit = test_circuit_01();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 02")
	{
		Circuit circuit = test_circuit_02();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 03")
	{
		Circuit circuit = test_circuit_03();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 04")
	{
		Circuit circuit = test_circuit_04();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 05")
	{
		Circuit circuit = test_circuit_05();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 06")
	{
		Circuit circuit = test_circuit_06();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
	SECTION("Test circuit 07")
	{
		Circuit circuit = test_circuit_07();
		Device device = Device::path(circuit.num_qubits());
		std::vector<wire::Id> placement
		    = detail::hsat_placement(circuit, device);
		CHECK(placement_verify(device, placement));
	}
}
