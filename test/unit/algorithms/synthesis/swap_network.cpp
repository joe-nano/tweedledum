/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/swap_network.h"

#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/MappedDAG.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/target/Device.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Synthesis of swapping networks using A*", "[swap_network][synth]")
{
	Device arch = Device::path(3u);
	MappedDAG swap_mapped(arch);

	std::vector<wire::Id> final
	    = {wire::make_qubit(2), wire::make_qubit(1), wire::make_qubit(0)};
	swap_network(swap_mapped, arch, final);
	CHECK(swap_mapped.phy_to_v() == final);
}

TEST_CASE("Synthesis of swapping networks using SAT", "[swap_network][synth]")
{
	Device arch = Device::path(3u);
	MappedDAG swap_mapped(arch);

	std::vector<wire::Id> final
	    = {wire::make_qubit(2), wire::make_qubit(1), wire::make_qubit(0)};

	swap_network_params params;
	params.method = swap_network_params::methods::sat;
	swap_network(swap_mapped, arch, final, params);
	CHECK(swap_mapped.phy_to_v() == final);
}
