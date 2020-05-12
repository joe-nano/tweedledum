/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/gray_synth.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/operations/w3_op.h"
#include "tweedledum/ir/operations/wn32_op.h"
#include "tweedledum/support/Angle.h"
#include "tweedledum/support/ParityMap.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_PRODUCT_TEST_CASE("Gray synthesis", "[gray_synth][template]",
    (CircuitDAG, Netlist), (wn32_op, w3_op))
{
	using op_type = typename TestType::op_type;
	SECTION("Check simple example from Amy paper")
	{
		ParityMap<uint32_t> parities;
		parities.add_term(0b0110, sym_angle::pi_quarter);
		parities.add_term(0b0001, sym_angle::pi_quarter);
		parities.add_term(0b1001, sym_angle::pi_quarter);
		parities.add_term(0b0111, sym_angle::pi_quarter);
		parities.add_term(0b1011, sym_angle::pi_quarter);
		parities.add_term(0b0011, sym_angle::pi_quarter);

		auto network = gray_synth<TestType>(4, parities);
		BitMatrixRM id_matrix(4, 4);
		id_matrix.foreach_row([](auto& row, const auto row_index) {
			row[row_index] = 1;
		});

		network.foreach_op([&](op_type const& op) {
			if (op.is(gate_ids::cx)) {
				id_matrix.row(op.target())
				    ^= id_matrix.row(op.control());
			}
		});
	}

	SECTION("Check with empty parities")
	{
		auto network = gray_synth<TestType>(4, {});
		CHECK(network.num_operations() == 0u);
		CHECK(network.num_qubits() == 4u);
	}
}
