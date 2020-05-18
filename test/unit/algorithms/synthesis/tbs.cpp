/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/tbs.h"

#include "tweedledum/algorithms/simulation/simulate_classically.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"

#include <catch.hpp>
#include <cstdint>
#include <vector>

using namespace tweedledum;
TEST_CASE("Transformation based synthesis", "[tbs]")
{
	std::vector<uint32_t> permutation = {0, 2, 3, 5, 7, 1, 4, 6};
	Module module;
	SECTION("Synthesize PRIME(3) - unidirectional TBS")
	{
		tbs(module, permutation);
		CircuitDAG& circuit = module.circuit_;
		for (auto i = 0u; i < permutation.size(); ++i) {
			CHECK(
			    simulate_classically(circuit, i) == permutation[i]);
		}
	}
	SECTION("Synthesize PRIME(3) - bidirectional TBS")
	{
		tbs_params params;
		params.behavior = tbs_params::behavior::bidirectional;
		tbs(module, permutation, params);
		CircuitDAG& circuit = module.circuit_;
		for (auto i = 0u; i < permutation.size(); ++i) {
			CHECK(
			    simulate_classically(circuit, i) == permutation[i]);
		}
	}
	SECTION("Synthesize PRIME(3) - multi-directional TBS")
	{
		tbs_params params;
		params.behavior = tbs_params::behavior::multidirectional;
		tbs(module, permutation, params);
		CircuitDAG& circuit = module.circuit_;
		for (auto i = 0u; i < permutation.size(); ++i) {
			CHECK(
			    simulate_classically(circuit, i) == permutation[i]);
		}
	}
}