/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/mapping/jit_map.h"

#include "test_circuits.h"
#include "tweedledum/algorithms/verification/map_verify.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_TEST_CASE(
    "Test for Just-in-time mapper", "[jit_map][mapping]", (CircuitDAG))
{
	SECTION("Test circuit 00")
	{
		TestType original = test_circuit_00<TestType>();
		Device device = Device::path(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 01")
	{
		TestType original = test_circuit_01<TestType>();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 02")
	{
		TestType original = test_circuit_02<TestType>();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 03")
	{
		TestType original = test_circuit_03<TestType>();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 04")
	{
		TestType original = test_circuit_04<TestType>();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 05")
	{
		TestType original = test_circuit_05<TestType>();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 06")
	{
		TestType original = test_circuit_06<TestType>();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
	SECTION("Test circuit 07")
	{
		TestType original = test_circuit_07<TestType>();
		Device device = Device::ring(original.num_qubits());
		MappedDAG mapped = jit_map(original, device);
		CHECK(map_verify(original, mapped));
	}
}
