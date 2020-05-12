/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/dbs.h"

#include "tweedledum/algorithms/simulation/simulate_classically.h"
#include "tweedledum/algorithms/synthesis/stg.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/operations/wn32_op.h"

#include <catch.hpp>

using namespace tweedledum;
TEMPLATE_PRODUCT_TEST_CASE("Decomposition based synthesis", "[dbs][template]",
    (CircuitDAG, Netlist), (wn32_op))
{
	std::vector<uint32_t> permutation = {0, 2, 3, 5, 7, 1, 4, 6};
	SECTION("Synthesize PRIME(3) - PPRM")
	{
		const auto network
		    = dbs<TestType>(permutation, stg_from_pprm());
		for (auto i = 0u; i < permutation.size(); ++i) {
			CHECK(
			    simulate_classically(network, i) == permutation[i]);
		}
	}
	SECTION("Synthesize PRIME(3) - spectrum")
	{
		const auto network
		    = dbs<TestType>(permutation, stg_from_spectrum());
		CHECK(network.num_operations() == 52u);
		CHECK(network.num_qubits() == 3u);
	}
}
