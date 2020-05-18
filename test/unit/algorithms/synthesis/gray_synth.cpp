/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/gray_synth.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/support/Angle.h"
#include "tweedledum/support/ParityMap.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("Gray synthesis", "[gray_synth]")
{
	using op_type = typename CircuitDAG::op_type;
	Module module;
	SECTION("Check simple example from Amy paper")
	{
		ParityMap<uint32_t> parities;
		parities.add_term(0b0110, sym_angle::pi_quarter);
		parities.add_term(0b0001, sym_angle::pi_quarter);
		parities.add_term(0b1001, sym_angle::pi_quarter);
		parities.add_term(0b0111, sym_angle::pi_quarter);
		parities.add_term(0b1011, sym_angle::pi_quarter);
		parities.add_term(0b0011, sym_angle::pi_quarter);

		gray_synth(module, 4, parities);
		CircuitDAG& circuit = module.circuit_;
		BitMatrixRM id_matrix(4, 4);
		id_matrix.foreach_row([](auto& row, const auto row_index) {
			row[row_index] = 1;
		});

		circuit.foreach_op([&](op_type const& op) {
			if (op.is(gate_ids::cx)) {
				id_matrix.row(op.target())
				    ^= id_matrix.row(op.control());
			}
		});
	}

	SECTION("Check with empty parities")
	{
		gray_synth(module, 4, {});
		CircuitDAG& circuit = module.circuit_;
		CHECK(circuit.num_operations() == 0u);
		CHECK(circuit.num_qubits() == 4u);
	}
}
