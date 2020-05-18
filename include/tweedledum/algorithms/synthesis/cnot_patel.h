/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/Gate.h"
#include "../../ir/Module.h"
#include "../../ir/Wire.h"
#include "../../support/BitMatrixRM.h"
#include "../../support/DynamicBitset.h"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <unordered_map>
#include <vector>

namespace tweedledum {
namespace detail {

template<class Matrix>
class CnotPatel {
	using matrix_type = Matrix;
	using qubit_pair_type = std::pair<uint32_t, uint32_t>;

public:
	CnotPatel(Circuit& circuit, std::vector<wire::Id> const& qubits,
	    matrix_type const& matrix, uint32_t partition_size)
	    : network_(circuit), qubits_(qubits), matrix_(matrix),
	      partition_size_(partition_size)
	{}

	auto synthesize(bool add_gates = true)
	{
		// synthesize lower triangular part
		auto gates_lower = lower_cnot_synthesis();
		matrix_.transpose();
		// synthesize upper triangular part
		auto gates_upper = lower_cnot_synthesis();

		if (add_gates == false) {
			return (gates_upper.size() + gates_lower.size());
		}

		for (const auto [control, target] : gates_upper) {
			// switch control/target of CX gates in gates_upper;
			network_.create_op(
			    GateLib::cx, qubits_[target], qubits_[control]);
		}

		std::reverse(gates_lower.begin(), gates_lower.end());
		for (const auto [control, target] : gates_lower) {
			network_.create_op(
			    GateLib::cx, qubits_[control], qubits_[target]);
		}
		return (gates_upper.size() + gates_lower.size());
	}

private:
	auto lower_cnot_synthesis()
	{
		std::vector<qubit_pair_type> gates;
		const auto num_columns = matrix_.num_columns();
		const auto num_sections
		    = (num_columns - 1u) / partition_size_ + 1u;

		for (auto section = 0ul; section < num_sections; ++section) {
			const auto start = section * partition_size_;
			const auto end = std::min(
			    start + partition_size_, matrix_.num_columns());
			std::unordered_map<uint32_t, uint32_t> patterns_table;

			// For each section we use row operations to eliminate
			// sub-row patterns that repeat in that section.
			for (auto row = start; row < matrix_.num_rows(); ++row)
			{
				auto pattern = 0u;
				auto subrow_column = 0u;
				for (auto column = start; column < end;
				     ++column) {
					pattern |= (matrix_.at(row, column)
					            << subrow_column);
					subrow_column += 1;
				}
				if (pattern == 0) {
					continue;
				}
				if (patterns_table.find(pattern)
				    != patterns_table.end()) {
					matrix_.row(row) ^= matrix_.row(
					    patterns_table[pattern]);
					gates.emplace_back(
					    patterns_table[pattern], row);
				} else {
					patterns_table[pattern] = row;
				}
			}

			// Gaussian elimination
			for (auto column = start; column < end; ++column) {
				auto is_diagonal_one
				    = (matrix_.at(column, column) == 1);
				for (auto row = column + 1;
				     row < matrix_.num_rows(); ++row) {
					if (matrix_.at(row, column) == 0) {
						continue;
					}
					if (is_diagonal_one == 0) {
						is_diagonal_one = 1;
						matrix_.row(column)
						    ^= matrix_.row(row);
						gates.emplace_back(row, column);
					}
					matrix_.row(row) ^= matrix_.row(column);
					gates.emplace_back(column, row);
				}
			}
		}
		return gates;
	}

private:
	Circuit& network_;
	std::vector<wire::Id> qubits_;
	matrix_type matrix_;
	uint32_t partition_size_;
};

} // namespace detail

/*! \brief Parameters for `cnot_patel`. */
struct cnot_patel_params {
	/*! \brief Search for the best parition size. */
	bool best_partition_size = false;
	/*! \brief Partition size */
	uint32_t partition_size = 1u;
};

// TODO:
// /*! \brief */
// template<class Circuit, class Matrix>
// void cnot_patel_rewire(Circuit& circuit, std::vector<wire::Id> const& qubits,
// Matrix const& matrix,
//                 cnot_patel_params params = {})
// {
// 	assert(circuit.num_qubits() >= qubits.size());
// 	assert(matrix.is_square());
// 	assert(qubits.size() == matrix.num_rows());
// 	assert(params.best_partition_size
// 	       || (params.partition_size < 32u && params.partition_size <=
// matrix.num_rows()));

// 	// Abbreviations:
// 	//   - ps : partition size
// 	if (!params.allow_rewiring && !params.best_partition_size) {
// 		detail::CnotPatel synthesizer(circuit, qubits, matrix,
// params.partition_size); 		synthesizer.synthesize(); 		return;
// 	}

// 	auto const min_ps = params.best_partition_size ? 1u :
// params.partition_size; 	auto const max_ps = params.best_partition_size ?
// matrix.num_rows() : params.partition_size; 	auto const old_num_gates =
// circuit.num_operations(); 	(void)old_num_gates; /* var not used in Release
// mode */ 	auto best_num_gates = std::numeric_limits<uint32_t>::max();

// 	auto best_ps = min_ps;
// 	std::vector<uint32_t> best_permutation(qubits.size());
// 	std::iota(best_permutation.begin(), best_permutation.end(), 0u);

// 	auto permutation = best_permutation;
// 	do {
// 		auto permuted_matrix = matrix.permute_rows(permutation);
// 		auto ps = min_ps;
// 		do {
// 			detail::CnotPatel synthesizer(circuit, qubits,
// permuted_matrix, ps); 			const auto num_gates = synthesizer.synthesize(false);
// 			if (num_gates < best_num_gates) {
// 				best_num_gates = num_gates;
// 				best_ps = ps;
// 				best_permutation = permutation;
// 			}
// 			++ps;
// 			assert(circuit.num_operations() == old_num_gates);
// 		} while (ps < max_ps);
// 	} while (std::next_permutation(permutation.begin(), permutation.end()));

// 	auto permuted_matrix = matrix.permute_rows(best_permutation);
// 	detail::CnotPatel synthesizer(circuit, qubits, permuted_matrix,
// best_ps); 	synthesizer.synthesize();

// 	auto transpositions = permutation_to_transpositions(best_permutation);
// 	for (auto&& [i, j] : transpositions) {
// 		i = qubits[i];
// 		j = qubits[j];
// 	}
// 	circuit.rewire(transpositions);
// }

/*! \brief CNOT Patel synthesis for linear circuits
 *
 * This is the in-place variant of ``cnot_patel``, in which the circuit is
 * passed as a parameter and can potentially already contain some gates. The
 * parameter ``qubits`` provides a qubit mapping to the existing qubits in the
 * circuit.
 *
 * \param circuit A quantum circuit
 * \param qubits  The subset of qubits the linear reversible circuit acts upon.
 * \param matrix  The square matrix representing a linear reversible circuit.
 * \param params  The parameters that configure the synthesis process.
 *                See `cnot_patel_params` for details.
 */
template<class Matrix>
void cnot_patel(Circuit& circuit, std::vector<wire::Id> const& qubits,
    Matrix const& matrix, cnot_patel_params params = {})
{
	assert(circuit.num_qubits() >= qubits.size());
	assert(matrix.is_square());
	assert(qubits.size() == matrix.num_rows());
	assert(params.best_partition_size
	       || (params.partition_size < 32u
	           && params.partition_size <= matrix.num_rows()));

	// Abbreviations:
	//   - ps : partition size
	if (!params.best_partition_size) {
		detail::CnotPatel synthesizer(
		    circuit, qubits, matrix, params.partition_size);
		synthesizer.synthesize();
		return;
	}

	auto const min_ps
	    = params.best_partition_size ? 1u : params.partition_size;
	auto const max_ps = params.best_partition_size ? matrix.num_rows() :
	                                                 params.partition_size;
	auto const old_num_gates = circuit.num_operations();
	(void) old_num_gates; /* var not used in Release mode */
	auto best_num_gates = std::numeric_limits<uint32_t>::max();

	auto best_ps = min_ps;
	auto ps = min_ps;
	do {
		detail::CnotPatel synthesizer(circuit, qubits, matrix, ps);
		const auto num_gates = synthesizer.synthesize(false);
		if (num_gates < best_num_gates) {
			best_num_gates = num_gates;
			best_ps = ps;
		}
		++ps;
		assert(circuit.num_operations() == old_num_gates);
	} while (ps < max_ps);
	detail::CnotPatel synthesizer(circuit, qubits, matrix, best_ps);
	synthesizer.synthesize();
}

/*! \brief CNOT Patel synthesis for linear circuits
 *
   \verbatim embed:rst
   This algorithm is based on the work in :cite:`PMH08`.

   The following code shows how to apply the algorithm to the example in the
 original paper.

   .. code-block:: c++

      std::vector<uint32_t> rows = {0b000011,
                                    0b011001,
                                    0b010010,
                                    0b111111,
                                    0b111011,
                                    0b011100};
      BitMatrixRM matrix(6, rows);
      cnot_patel_params parameters;
      parameters.allow_rewiring = false;
      parameters.best_partition_size = false;
      parameters.partition_size = 2u;
      auto circuit = cnot_patel(matrix, parameters);

   \endverbatim
 *
 * \param matrix The square matrix representing a linear reversible circuit.
 * \param params The parameters that configure the synthesis process.
 *               See `cnot_patel_params` for details.
 *
 * \algtype synthesis
 * \algexpects Linear matrix
 * \algreturns {CNOT} circuit
 */
template<class Matrix>
void cnot_patel(Module* module, Matrix const& matrix, cnot_patel_params params = {})
{
	assert(matrix.is_square());
	assert(params.best_partition_size
	       || (params.partition_size >= 1 && params.partition_size <= 32));

	const uint32_t num_qubits = matrix.num_rows();
	std::vector<wire::Id> qubits;
	for (uint32_t i = 0u; i < num_qubits; ++i) {
		qubits.emplace_back(module->circuit_.create_qubit());
	}
	cnot_patel(module->circuit_, qubits, matrix, params);
}

} // namespace tweedledum
