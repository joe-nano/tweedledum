/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/Gate.h"
#include "../../ir/Module.h"
#include "../../ir/Wire.h"
#include "../../support/ParityMap.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

namespace tweedledum {
namespace detail {

inline void linear_synth_binary(Circuit& circuit, std::vector<wire::Id> const& qubits,
    ParityMap<uint32_t> parities)
{
	const auto num_qubits = qubits.size();

	// Initialize the parity of each qubit state
	// Applying phase gate to parities that consisting of just one variable
	// i is the index of the target
	std::vector<uint32_t> qubits_states;
	for (auto i = 0u; i < num_qubits; ++i) {
		qubits_states.emplace_back((1u << i));
		auto rotation_angle = parities.extract_term(qubits_states[i]);
		if (rotation_angle != 0.0) {
			circuit.create_op(
			    GateLib::rz(rotation_angle), qubits[i]);
		}
	}

	// Synthesize the circuit
	for (auto i = 1u; i < (1u << num_qubits); i++) {
		if ((i ^ (1 << (i - 1))) == 0) {
			continue;
		}
		uint32_t first_num = std::floor(std::log2(i));
		for (auto j = 0u; j < num_qubits; j++) {
			if ((first_num != j)
			    && ((qubits_states[j] ^ qubits_states[first_num])
			        == i)) {
				qubits_states[first_num] ^= qubits_states[j];
				circuit.create_op(
				    GateLib::cx, qubits[j], qubits[first_num]);
				auto rotation_angle = parities.extract_term(
				    qubits_states[first_num]);
				if (rotation_angle != 0.0) {
					circuit.create_op(
					    GateLib::rz(rotation_angle),
					    qubits[first_num]);
				}
			}
		}
	}

	// Return qubits to initial state
	for (int i = num_qubits - 1; i > 0; i--) {
		circuit.create_op(GateLib::cx, qubits[i - 1], qubits[i]);
	}
}

inline void linear_synth_gray(Circuit& circuit, std::vector<wire::Id> const& qubits,
    ParityMap<uint32_t> parities)
{
	const auto num_qubits = qubits.size();

	// Generate Gray code
	std::vector<uint32_t> gray_code;
	for (auto i = 0u; i < (1u << num_qubits); ++i) {
		gray_code.emplace_back((i >> 1) ^ i);
	}

	// Initialize the parity of each qubit state
	// Applying phase gate to parities that consisting of just one variable
	// i is the index of the target
	std::vector<uint32_t> qubits_states;
	for (auto i = 0u; i < num_qubits; ++i) {
		qubits_states.emplace_back((1u << i));
		auto rotation_angle = parities.extract_term(qubits_states[i]);
		if (rotation_angle != 0.0) {
			circuit.create_op(
			    GateLib::rz(rotation_angle), qubits[i]);
		}
	}

	// Synthesize the circuit
	// i is the index of the target
	for (auto i = num_qubits - 1u; i > 0; --i) {
		for (auto j = (1u << (i + 1)) - 1u; j > (1u << i); --j) {
			const auto temp
			    = std::log2(gray_code[j] ^ gray_code[j - 1u]);
			qubits_states[i] ^= qubits_states[temp];
			circuit.create_op(GateLib::cx, qubits[temp], qubits[i]);
			auto rotation_angle
			    = parities.extract_term(qubits_states[i]);
			if (rotation_angle != 0.0) {
				circuit.create_op(
				    GateLib::rz(rotation_angle), qubits[i]);
			}
		}
		const auto temp = std::log2(
		    gray_code[1 << i] ^ gray_code[(1u << (i + 1)) - 1u]);
		qubits_states[i] ^= qubits_states[temp];
		circuit.create_op(GateLib::cx, qubits[temp], qubits[i]);
		auto rotation_angle = parities.extract_term(qubits_states[i]);
		if (rotation_angle != 0.0) {
			circuit.create_op(
			    GateLib::rz(rotation_angle), qubits[i]);
		}
	}
}

} // namespace detail

/*! \brief Parameters for `linear_synth`. */
struct linear_synth_params {
	enum class strategy : uint8_t {
		binary,
		gray,
	} strategy = strategy::gray;
};

/*! \brief Linear synthesis for small {CNOT, Rz} networks
 *
 * This is the in-place variant of ``linear_synth``, in which the circuit is
 * passed as a parameter and can potentially already contain some gates. The
 * parameter ``qubits`` provides a qubit mapping to the existing qubits in the
 * circuit.
 *
 * \param circuit  A quantum circuit
 * \param qubits   The subset of qubits the linear reversible circuit acts upon
 * \param parities List of parities and rotation angles to synthesize
 * \param params   The parameters that configure the synthesis process.
 *                 See `linear_synth_params` for details.
 */
inline void linear_synth(Circuit& circuit, std::vector<wire::Id> const& qubits,
    ParityMap<uint32_t> const& parities, linear_synth_params params = {})
{
	assert(qubits.size() <= 32);
	switch (params.strategy) {
	case linear_synth_params::strategy::binary:
		detail::linear_synth_binary(circuit, qubits, parities);
		break;
	case linear_synth_params::strategy::gray:
		detail::linear_synth_gray(circuit, qubits, parities);
		break;
	}
}

/*! \brief Linear synthesis for small {CNOT, Rz} networks.
 *
 * Synthesize all linear combinations.
 *
 * \param num_qubits Number of qubits
 * \param parities   List of parities and rotation angles to synthesize
 * \param params     The parameters that configure the synthesis process.
 *                   See `linear_synth_params` for details.
 *
 * \return {CNOT, Rz} circuit
 *
 * \algtype synthesis
 * \algexpects List of parities and rotation angles to synthesize
 * \algreturns {CNOT, Rz} circuit
 */
inline void linear_synth(Module& module, uint32_t num_qubits,
    ParityMap<uint32_t> const& parities, linear_synth_params params = {})
{
	assert(num_qubits <= 32);
	std::vector<wire::Id> qubits;
	for (uint32_t i = 0u; i < num_qubits; ++i) {
		qubits.emplace_back(module.circuit_.create_qubit());
	}
	linear_synth(module.circuit_, qubits, parities, params);
}

} // namespace tweedledum
