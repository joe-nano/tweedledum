/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/decomposition/barenco.h"

#include "tweedledum/algorithms/simulation/simulate_classically.h"
#include "tweedledum/algorithms/verification/unitary_verify.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"

#include <algorithm>
#include <catch.hpp>
#include <vector>

using namespace tweedledum;

TEST_CASE("Barenco decompostion", "[decomp][barenco]")
{
	Module module;
	SECTION("Without relative phase")
	{
		for (uint32_t i = 4u; i <= 8; ++i) {
			CircuitDAG original(&module);
			std::vector<wire::Id> qubits(i, wire::invalid_id);
			std::generate(qubits.begin(), qubits.end(), [&]() {
				return original.create_qubit();
			});

			original.create_op(GateLib::ncx,
			    std::vector(qubits.begin() + 1, qubits.end() - 1),
			    std::vector({qubits.at(0)}));

			barenco_params params;
			params.use_ncrx = false;
			CircuitDAG decomposed
			    = barenco_decomposition(original, params);
			CHECK(unitary_verify(original, decomposed));
		}
	}
	SECTION("With relative phase")
	{
		for (uint32_t i = 4u; i <= 8; ++i) {
			CircuitDAG original(&module);
			std::vector<wire::Id> qubits(i, wire::invalid_id);
			std::generate(qubits.begin(), qubits.end(), [&]() {
				return original.create_qubit();
			});

			original.create_op(GateLib::ncx,
			    std::vector(qubits.begin() + 1, qubits.end() - 1),
			    std::vector({qubits.at(0)}));
			CircuitDAG decomposed = barenco_decomposition(original);
			CHECK(unitary_verify(original, decomposed));
		}
	}
}
