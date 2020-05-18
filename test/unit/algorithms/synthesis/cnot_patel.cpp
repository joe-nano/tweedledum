/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/cnot_patel.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/support/BitMatrixRM.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("CNOT patel synthesis", "[cnot_patel]")
{
	using op_type = typename Circuit::op_type;
	std::vector<uint32_t> rows
	    = {0b000011, 0b011001, 0b010010, 0b111111, 0b111011, 0b011100};
	BitMatrixRM matrix(6, rows);
	Module module;
	SECTION("Check example from paper")
	{
		cnot_patel_params parameters;
		parameters.best_partition_size = false;
		parameters.partition_size = 2u;
		cnot_patel(&module, matrix, parameters);

		// Simulate CNOTs in network
		BitMatrixRM id_matrix(6, 6);
		id_matrix.foreach_row([](auto& row, const auto row_index) {
			row[row_index] = 1;
		});

		Circuit& circuit = module.circuit_;
		circuit.foreach_op([&](op_type const& op) {
			if (!op.is(gate_ids::cx)) {
				return;
			}
			id_matrix.row(op.target())
			    ^= id_matrix.row(op.control());
		});
		// Check if network realizes original matrix
		for (auto row_index = 0u; row_index < matrix.num_rows();
		     ++row_index) {
			CHECK(matrix.row(row_index) == id_matrix.row(row_index));
		}
	}
	// SECTION("Find best permutation for the example from paper")
	// {
	// 	cnot_patel_params parameters;
	// 	parameters.allow_rewiring = true;
	// 	parameters.best_partition_size = true;
	// 	auto network = cnot_patel<TestType>(matrix, parameters);

	// 	// Simulate CNOTs in network
	// 	BitMatrixRM id_matrix(6, 6);
	// 	id_matrix.foreach_row([](auto& row, const auto row_index) {
	// row[row_index] = 1; });

	// 	network.foreach_op([&](auto const& node) {
	// 		if (!node.operation.gate.is(gate_ids::cx)) {
	// 			return;
	// 		}
	// 		id_matrix.row(node.operation.target()) ^=
	// id_matrix.row(node.operation.control());
	// 	});
	// 	std::vector<uint32_t> rows_permutation;
	// 	for (wire_id wire : network.wiring_map()) {
	// 		rows_permutation.push_back(wire);
	// 	}
	// 	matrix.permute_rows(rows_permutation);

	// 	// Check if network realizes original matrix
	// 	for (auto row_index = 0u; row_index < matrix.num_rows();
	// ++row_index) { 		CHECK(matrix.row(row_index) ==
	// id_matrix.row(row_index));
	// 	}
	// }
}
