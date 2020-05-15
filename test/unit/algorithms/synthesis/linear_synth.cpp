/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/linear_synth.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/support/Angle.h"
#include "tweedledum/support/ParityMap.h"

#include <catch.hpp>

using namespace tweedledum;
TEMPLATE_TEST_CASE(
    "Linear synthesis", "[linear_synth][template]", CircuitDAG, Netlist)
{
	ParityMap<uint32_t> parities;
	parities.add_term(3u, sym_angle::pi_quarter);

	SECTION("Using binary strategy")
	{
		auto network = linear_synth<TestType>(
		    3u, parities, {linear_synth_params::strategy::binary});
	}

	SECTION("Using gray strategy (default)")
	{
		auto network = linear_synth<TestType>(3u, parities);
	}
}
