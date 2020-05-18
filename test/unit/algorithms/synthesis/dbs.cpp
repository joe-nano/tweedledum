/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/dbs.h"

#include "tweedledum/algorithms/simulation/simulate_classically.h"
#include "tweedledum/algorithms/synthesis/stg.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"

#include <catch.hpp>

using namespace tweedledum;
TEST_CASE("Decomposition based synthesis", "[dbs]")
{
	std::vector<uint32_t> permutation = {0, 2, 3, 5, 7, 1, 4, 6};
	Module module;
	SECTION("Synthesize PRIME(3) - PPRM")
	{
		dbs(module, permutation, stg_from_pprm());
		CircuitDAG circuit = module.circuit_;
		for (auto i = 0u; i < permutation.size(); ++i) {
			CHECK(
			    simulate_classically(circuit, i) == permutation[i]);
		}
	}
	SECTION("Synthesize PRIME(3) - spectrum")
	{
		dbs(module, permutation, stg_from_spectrum());
		CircuitDAG circuit = module.circuit_;
		CHECK(circuit.num_operations() == 52u);
		CHECK(circuit.num_qubits() == 3u);
	}
}
