/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/jit_map.h"

#include "test_circuits.h"
#include "tweedledum/algorithms/verification/map_verify.h"
#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Test for Just-in-time mapper", "[jit_map][mapping]")
{
	SECTION("Test circuit 00")
	{
		Circuit original = test_circuit_00();
		Device device = Device::path(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 01")
	{
		Circuit original = test_circuit_01();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 02")
	{
		Circuit original = test_circuit_02();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 03")
	{
		Circuit original = test_circuit_03();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 04")
	{
		Circuit original = test_circuit_04();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 05")
	{
		Circuit original = test_circuit_05();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 06")
	{
		Circuit original = test_circuit_06();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 07")
	{
		Circuit original = test_circuit_07();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
}
