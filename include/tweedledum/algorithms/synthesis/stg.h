/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Wire.h"
#include "../../support/Angle.h"
#include "../../support/ParityMap.h"
#include "gray_synth.h"
#include "linear_synth.h"

#include <cstdint>
#include <easy/esop/constructors.hpp>
#include <easy/esop/esop_from_pkrm.hpp>
#include <easy/esop/esop_from_pprm.hpp>
#include <iostream>
#include <kitty/kitty.hpp>
#include <vector>

namespace tweedledum {

/*! \brief Parameters for `stg_from_exact_esop`. */
struct stg_from_esop_params {
};

/*! \brief Synthesize a quantum circuit from a function by computing exact ESOP
 * representation
 */
struct stg_from_exact_esop {
	/*! \brief Synthesize into a _existing_ quantum circuit
	 *
	 * \param circuit  A quantum circuit
	 * \param qubits   The subset of qubits the gate acts upon.
	 * \param function
	 */
	template<class Circuit>
	void operator()(Circuit& circuit, std::vector<wire::Id> const& qubits,
	    kitty::dynamic_truth_table const& function) const
	{
		using exact_synthesizer
		    = easy::esop::esop_from_tt<kitty::dynamic_truth_table,
		        easy::sat2::maxsat_rc2, easy::esop::helliwell_maxsat>;
		const auto num_controls = function.num_vars();
		assert(qubits.size()
		       == static_cast<std::size_t>(num_controls) + 1u);

		easy::esop::helliwell_maxsat_statistics stats;
		easy::esop::helliwell_maxsat_params ps;
		exact_synthesizer synthesizer(stats, ps);

		std::vector<wire::Id> target = {qubits.back()};
		const auto cubes = synthesizer.synthesize(function);
		for (auto const& cube : cubes) {
			std::vector<wire::Id> controls;
			std::vector<wire::Id> negations;
			auto bits = cube._bits;
			auto mask = cube._mask;
			for (uint32_t v = 0u; v < num_controls; ++v) {
				if (mask & 1) {
					controls.emplace_back((bits & 1) ?
					                          qubits[v] :
					                          !qubits[v]);
				}
				bits >>= 1;
				mask >>= 1;
			}
			switch (controls.size()) {
			case 0:
				circuit.create_op(GateLib::x, controls, target);
				break;

			case 1:
				circuit.create_op(
				    GateLib::cx, controls, target);
				break;

			default:
				circuit.create_op(
				    GateLib::ncx, controls, target);
				break;
			}
		}
	}
};

/*! \brief Synthesize a quantum circuit from a function by computing PKRM
 * representation
 *
 * PKRM: Pseudo-Kronecker Read-Muller expression---a special case of an ESOP
 * form.
 */
struct stg_from_pkrm {
	/*! \brief Synthesize into a _existing_ quantum circuit
	 *
	 * \param circuit  A quantum circuit
	 * \param qubits   The subset of qubits the gate acts upon.
	 * \param function
	 */
	template<class Circuit>
	void operator()(Circuit& circuit, std::vector<wire::Id> const& qubits,
	    kitty::dynamic_truth_table const& function) const
	{
		const auto num_controls = function.num_vars();
		assert(qubits.size()
		       >= static_cast<std::size_t>(num_controls) + 1u);

		std::vector<wire::Id> target = {qubits.back()};
		for (auto const& cube :
		    easy::esop::esop_from_optimum_pkrm(function)) {
			std::vector<wire::Id> controls;
			std::vector<wire::Id> negations;
			auto bits = cube._bits;
			auto mask = cube._mask;
			for (uint32_t v = 0u; v < num_controls; ++v) {
				if (mask & 1) {
					controls.emplace_back((bits & 1) ?
					                          qubits[v] :
					                          !qubits[v]);
				}
				bits >>= 1;
				mask >>= 1;
			}
			switch (controls.size()) {
			case 0:
				circuit.create_op(GateLib::x, controls, target);
				break;

			case 1:
				circuit.create_op(
				    GateLib::cx, controls, target);
				break;

			default:
				circuit.create_op(
				    GateLib::ncx, controls, target);
				break;
			}
		}
	}
};

/*! \brief Synthesize a quantum circuit from a function by computing PPRM
 * representation
 *
 * PPRM: The positive polarity Reed-Muller form is an ESOP, where each variable
 * has positive polarity (not complemented form). PPRM is a canonical
 * expression, so further minimization is not possible.
 */
struct stg_from_pprm {
	/*! \brief Synthesize into a _existing_ quantum circuit
	 *
	 * \param circuit  A quantum circuit
	 * \param qubits   The subset of qubits the gate acts upon.
	 * \param function
	 */
	template<class Circuit>
	void operator()(Circuit& circuit, std::vector<wire::Id> const& qubits,
	    kitty::dynamic_truth_table const& function) const
	{
		const auto num_controls = function.num_vars();
		assert(qubits.size()
		       >= static_cast<std::size_t>(num_controls) + 1u);

		std::vector<wire::Id> target = {qubits.back()};
		for (auto const& cube : easy::esop::esop_from_pprm(function)) {
			assert(cube._bits == cube._mask); /* PPRM property */
			std::vector<wire::Id> controls;
			auto bits = cube._bits;
			for (uint32_t v = 0u; v < num_controls; ++v) {
				if (bits & 1) {
					controls.push_back(qubits[v]);
				}
				bits >>= 1;
			}
			switch (controls.size()) {
			case 0:
				circuit.create_op(GateLib::x, controls, target);
				break;

			case 1:
				circuit.create_op(
				    GateLib::cx, controls, target);
				break;

			default:
				circuit.create_op(
				    GateLib::ncx, controls, target);
				break;
			}
		}
	}
};

/*! \brief Parameters for `stg_from_spectrum`. */
struct stg_from_spectrum_params {
	enum class behavior : uint8_t {
		use_linear_synth,
		complete_spectra,
	} behavior = behavior::complete_spectra;

	linear_synth_params ls_params;
	gray_synth_params gs_params;
};

/*! \brief Synthesize a single target gate from a function by computing
 * Rademacher-Walsh spectrum */
struct stg_from_spectrum {
	stg_from_spectrum(stg_from_spectrum_params const& params_ = {})
	    : params(params_)
	{}

	/*! \brief Synthesize a single target gate into a _existing_ quantum
	 * circuit
	 *
	 * \param circuit  A quantum circuit
	 * \param qubits   The subset of qubits the gate acts upon.
	 * \param function
	 */
	template<class Circuit>
	void operator()(Circuit& circuit, std::vector<wire::Id> const& qubits,
	    kitty::dynamic_truth_table const& function) const
	{
		const auto num_controls = function.num_vars();
		assert((num_controls + 1u) <= 32u);
		assert(qubits.size() >= num_controls + 1u);

		auto gate_function
		    = kitty::extend_to(function, num_controls + 1);
		auto xt = gate_function.construct();
		kitty::create_nth_var(xt, num_controls);
		gate_function &= xt;

		ParityMap<uint32_t> parities;
		// An angle type create like this is implicitly multiplied by pi
		const Angle nom(1, (1 << gate_function.num_vars()));
		const auto spectrum
		    = kitty::rademacher_walsh_spectrum(gate_function);
		for (auto i = 1u; i < spectrum.size(); ++i) {
			if (spectrum[i] == 0) {
				continue;
			}
			parities.add_term(i, nom * spectrum[i]);
		}

		circuit.create_op(GateLib::h, qubits.back());
		if (params.behavior
		    == stg_from_spectrum_params::behavior::use_linear_synth) {
			linear_synth(
			    circuit, qubits, parities, params.ls_params);
		} else if (parities.num_terms() == spectrum.size() - 1) {
			linear_synth(
			    circuit, qubits, parities, params.ls_params);
		} else {
			gray_synth(circuit, qubits, parities, params.gs_params);
		}
		circuit.create_op(GateLib::h, qubits.back());
	}

	stg_from_spectrum_params params;
};

} /* namespace tweedledum */
