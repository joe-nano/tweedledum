/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/Wire.h"
#include "../../support/Angle.h"
#include "../../support/ParityMap.h"
#include "gray_synth.h"
#include "linear_synth.h"

#include <algorithm>
#include <vector>

namespace tweedledum {

namespace detail {

inline void fast_hadamard_transform(std::vector<Angle>& angles)
{
	uint32_t k = 0u;
	for (uint32_t m = 1u; m < angles.size(); m <<= 1u) {
		for (uint32_t i = 0u; i < angles.size(); i += (m << 1u)) {
			for (uint32_t j = i, p = k = i + m; j < p; ++j, ++k) {
				Angle t = angles.at(j);
				angles.at(j) += angles.at(k);
				angles.at(k) = t - angles.at(k);
			}
		}
	}
}

} // namespace detail

/*! \brief Synthesis of diagonal unitary matrices.
 *
 * This is the in-place variant of ``diagonal_synth``, in which the circuit is
 * passed as a parameter and can potentially already contain some gates.  The
 * parameter ``qubits`` provides a qubit mapping to the existing qubits in the
 * circuit.
 *
 * \param circuit A quantum circuit
 * \param qubits The subset of qubits the linear reversible circuit acts upon
 * \param angles Angles for diagonal matrix elements
 */
inline void diagonal_synth(Circuit& circuit, std::vector<wire::Id> qubits,
    std::vector<Angle> const& angles)
{
	// Number of angles + 1 needs to be a power of two!
	assert(!angles.empty() && !(angles.size() & (angles.size() - 1)));
	assert(!qubits.empty() && qubits.size() <= 32);
	assert((1u << qubits.size()) == angles.size());

	// Normalize input angles
	std::vector<Angle> norm_angles;
	std::transform(angles.begin(), angles.end(),
	    std::back_inserter(norm_angles), [](Angle const& a) {
		    return -a;
	    });

	// Normalize qubits polarity
	uint32_t const num_angles_2 = (angles.size() / 2);
	uint32_t index = 0u;
	for (uint32_t i = qubits.size(); i-- > 0;) {
		if (!qubits.at(index).is_complemented()) {
			index++;
			continue;
		}
		qubits.at(index++).complement();
		for (uint32_t j = 0u; j < num_angles_2;
		     j += (num_angles_2 >> i)) {
			for (uint32_t k = (j << 1);
			     k < ((num_angles_2 >> i) + (j << 1)); k += 1) {
				std::swap(norm_angles.at(k),
				    norm_angles.at((num_angles_2 >> i) + k));
			}
		}
	}

	detail::fast_hadamard_transform(norm_angles);
	uint32_t factor = (1 << (qubits.size() - 1));
	ParityMap<uint32_t> parities;
	for (uint32_t i = 0u; i < norm_angles.size(); ++i) {
		if (norm_angles.at(i) == sym_angle::zero) {
			continue;
		}
		parities.add_term(i, norm_angles.at(i) / factor);
	}
	// cnot_rz(circuit, qubits, parities);
	if (parities.num_terms() == norm_angles.size()) {
		linear_synth(circuit, qubits, parities);
	} else {
		gray_synth(circuit, qubits, parities);
	}
}

/*! \brief Synthesis of diagonal unitary matrices.
 *
   \verbatim embed:rst

   This algorithm is based on the work in :cite:`SS03`.  It takes as input
 :math:`2^n - 1` real angles :math:`\theta_i` for :math:`i = 1, \dots, 2^{n-1}`
 and returns a quantum circuit that realizes the :math:`2^n \times 2^n` unitary
 operation

   :math:`U = \mathrm{diag}(1, e^{i\theta_1}, e^{i\theta_2}, \dots,
 e^{i\theta_{2^{n-1}}}).`

   It uses a fast Hadamard transformation to compute angles for parity terms
 that are passed to the Gray synthesis algorithm ``gray_synth``.

   \endverbatim
 * \param angles Angles for diagonal matrix elements
 * \return {CNOT, Rz} circuit
 *
 * \algtype synthesis
 * \algexpects List of angles in diagonal unitary matrix
 * \algreturns {CNOT, Rz} circuit
 */
inline void diagonal_synth(Module& module, std::vector<Angle> const& angles)
{
	// Number of angles + 1 needs to be a power of two!
	assert(!angles.empty() && !(angles.size() & (angles.size() - 1)));
	uint32_t num_qubits = __builtin_ctz(angles.size());
	assert(num_qubits <= 32u);

	std::vector<wire::Id> qubits;
	for (uint32_t i = 0u; i < num_qubits; ++i) {
		qubits.emplace_back(module.circuit_.create_qubit());
	}
	diagonal_synth(module.circuit_, qubits, angles);
}

} // namespace tweedledum
