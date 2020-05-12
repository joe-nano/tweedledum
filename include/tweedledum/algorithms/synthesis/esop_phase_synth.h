/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Gate.h"
#include "../../ir/Wire.h"

#include <easy/esop/esop_from_pprm.hpp>
#include <vector>

namespace tweedledum {

/*! \brief ESOP-phase synthesis.
 *
 * This is the in-place variant of ``esop_phase_synth``, in which the circuit is
 * passed as a parameter and can potentially already contain some gates. The
 * parameter ``qubits`` provides a qubit mapping to the existing qubits in the
 * circuit.
 *
 * \param circuit A quantum circuit
 * \param qubits A qubit mapping
 * \param function A Boolean function
 */
template<typename Circuit>
void esop_phase_synth(Circuit& circuit, std::vector<wire::Id> const& qubits,
    kitty::dynamic_truth_table const& function)
{
	for (const auto& cube : easy::esop::esop_from_pprm(function)) {
		std::vector<wire::Id> controls;
		std::vector<wire::Id> targets;
		for (uint32_t i = 0u; i < function.num_vars(); ++i) {
			if (!cube.get_mask(i)) {
				continue;
			}
			assert(cube.get_bit(i));
			if (targets.empty()) {
				targets.emplace_back(qubits[i]);
			} else {
				controls.emplace_back(qubits[i]);
			}
		}
		if (!targets.empty()) {
			circuit.create_op(GateLib::ncz, controls, targets);
		}
	}
}

/*! \brief ESOP-phase synthesis.
 *
 * Finds a quantum circuit using multiple-controlled Z gates that computes
 * a phase into a quantum state based on the Boolean function.  Note that
 * the circuit is the same for the function and its inverse.
 *
 * In order to find the multiple-controlled Z gates, the algorithm computes
 * the function's PPRM representation.
 *
 * \param function A Boolean function
 *
 * \algtype synthesis
 * \algexpects Boolean function
 * \algreturns Quantum circuit
 */
template<class Circuit>
Circuit esop_phase_synth(kitty::dynamic_truth_table const& function)
{
	Circuit circuit;
	uint32_t const num_qubits = function.num_vars();
	std::vector<wire::Id> qubits;
	for (uint32_t i = 0u; i < num_qubits; ++i) {
		qubits.emplace_back(circuit.create_qubit());
	}
	esop_phase_synth(circuit, qubits, function);
	return circuit;
}

} // namespace tweedledum
