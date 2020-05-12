/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Gate.h"
#include "../../ir/Wire.h"
#include "../../support/Angle.h"
#include "../synthesis/diagonal_synth.h"
#include "../utility/shallow_duplicate.h"
#include "barenco.h"
#include "ccz.h"

#include <array>
#include <iostream>
#include <vector>

namespace tweedledum {

/*! \brief Parameters for `decomposition`. */
struct decomp_params {
	uint64_t gate_set = gate_set::clifford_t;
	uint32_t barenco_controls_threshold = 6u;
	bool allow_ancilla = true;
	bool use_t_par = false;
	bool use_relative_phase = false;
};

#pragma region Decomposition circuit builder(detail)
namespace detail {

template<typename Circuit>
class decomp_builder : public Circuit {
public:
	using op_type = typename Circuit::op_type;
	using node_type = typename Circuit::node_type;
	using dstrg_type = typename Circuit::dstrg_type;

	explicit decomp_builder(Circuit& circuit, decomp_params const& params)
	    : Circuit(circuit), params_(params)
	{
		barenco_params_.controls_threshold
		    = params_.barenco_controls_threshold;
		barenco_params_.use_ncrx = params_.use_relative_phase;
	}

public:
#pragma region Creating operations(using wire ids)
	void create_op(Gate const& g, wire::Id const t)
	{
		if (params_.gate_set & (1ull << static_cast<uint32_t>(g.id())))
		{
			this->emplace_op(op_type(g, t));
			return;
		}
		switch (g.id()) {
		// Non-parameterisable gates
		case gate_ids::i:
			create_op(GateLib::u3(sym_angle::zero, sym_angle::zero,
			              sym_angle::zero),
			    t);
			return;

		case gate_ids::h:
			create_op(GateLib::u3(sym_angle::pi_half,
			              sym_angle::zero, sym_angle::pi),
			    t);
			return;

		case gate_ids::x:
			create_op(GateLib::u3(sym_angle::pi, sym_angle::zero,
			              sym_angle::pi),
			    t);
			return;

		case gate_ids::y:
			create_op(GateLib::u3(sym_angle::pi, sym_angle::pi_half,
			              sym_angle::pi_half),
			    t);
			return;

		case gate_ids::z:
			create_op(GateLib::r1(sym_angle::pi), t);
			return;

		case gate_ids::s:
			create_op(GateLib::r1(sym_angle::pi_half), t);
			return;

		case gate_ids::sdg:
			create_op(GateLib::r1(-sym_angle::pi_half), t);
			return;

		case gate_ids::t:
			create_op(GateLib::r1(sym_angle::pi_quarter), t);
			return;

		case gate_ids::tdg:
			create_op(GateLib::r1(-sym_angle::pi_quarter), t);
			return;

		// Parameterisable gates
		case gate_ids::r1:
			create_op(GateLib::u3(sym_angle::zero, sym_angle::zero,
			              g.rotation_angle()),
			    t);
			return;

		case gate_ids::rx:
			create_op(GateLib::u3(g.rotation_angle(),
			              -sym_angle::pi_half, sym_angle::pi_half),
			    t);
			return;

		case gate_ids::ry:
			create_op(GateLib::u3(g.rotation_angle(),
			              sym_angle::zero, sym_angle::zero),
			    t);
			return;

		default:
			break;
		}
		this->emplace_op(op_type(g, t));
	}

	void create_op(Gate const& g, wire::Id const w0, wire::Id const w1)
	{
		if (params_.gate_set & (1ull << static_cast<uint32_t>(g.id())))
		{
			this->emplace_op(op_type(g, w0, w1));
			return;
		}
		switch (g.id()) {
		// Non-parameterisable gates
		case gate_ids::cx:
			if (params_.gate_set
			    & (1ull << static_cast<uint32_t>(gate_ids::cz))) {
				create_op(GateLib::h, w1);
				create_op(GateLib::cz, w0, w1);
				create_op(GateLib::h, w1);
				return;
			}
			break;

		case gate_ids::cy:
			create_op(GateLib::sdg, w1);
			create_op(GateLib::cx, w0, w1);
			create_op(GateLib::s, w1);
			return;

		case gate_ids::cz:
			create_op(GateLib::h, w1);
			create_op(GateLib::cx, w0, w1);
			create_op(GateLib::h, w1);
			return;

		case gate_ids::swap:
			create_op(GateLib::cx, w0, w1);
			create_op(GateLib::cx, w1, w0);
			create_op(GateLib::cx, w0, w1);
			return;

		// Parameterisable gates
		case gate_ids::crx:
			create_op(GateLib::r1(g.rotation_angle() / 2), w1);
			create_op(GateLib::cx, w0, w1);
			create_op(GateLib::ry(-g.rotation_angle() / 2), w1);
			create_op(GateLib::cx, w0, w1);
			create_op(GateLib::u3(g.rotation_angle() / 2,
			              -sym_angle::pi_half, sym_angle::zero),
			    w1);
			return;

		case gate_ids::cry:
			create_op(GateLib::ry(g.rotation_angle() / 2), w1);
			create_op(GateLib::cx, w0, w1);
			create_op(GateLib::ry(-g.rotation_angle() / 2), w1);
			create_op(GateLib::cx, w0, w1);
			return;

		case gate_ids::crz:
			create_op(GateLib::r1(g.rotation_angle() / 2), w1);
			create_op(GateLib::cx, w0, w1);
			create_op(GateLib::r1(-g.rotation_angle() / 2), w1);
			create_op(GateLib::cx, w0, w1);
			return;

		default:
			break;
		}
		this->emplace_op(op_type(g, w0, w1));
	}

	void create_op(Gate const& g, wire::Id const c0, wire::Id const c1,
	    wire::Id const t)
	{
		if (params_.gate_set & (1ull << static_cast<uint32_t>(g.id())))
		{
			this->emplace_op(op_type(g, c0, c1, t));
			return;
		}
		switch (g.id()) {
		// Non-parameterisable gates
		case gate_ids::ncx:
			create_op(GateLib::h, t);
			if (params_.use_t_par) {
				ccz_tpar(*this, c0, c1, t);
			} else {
				ccz(*this, c0, c1, t);
			}
			create_op(GateLib::h, t);
			return;

		case gate_ids::ncy:
			create_op(GateLib::sdg, t);
			create_op(GateLib::h, t);
			if (params_.use_t_par) {
				ccz_tpar(*this, c0, c1, t);
			} else {
				ccz(*this, c0, c1, t);
			}
			create_op(GateLib::h, t);
			create_op(GateLib::s, t);
			break;

		case gate_ids::ncz:
			if (params_.use_t_par) {
				ccz_tpar(*this, c0, c1, t);
			} else {
				ccz(*this, c0, c1, t);
			}
			return;

		// Parameterisable gates
		case gate_ids::ncrx:
		case gate_ids::ncry:
			// create_op(GateLib::ry(g.rotation_angle() / 2), w1);
			// create_op(GateLib::ncx, c0, c1, t);
			// create_op(GateLib::ry(-g.rotation_angle() / 2), w1);
			// create_op(GateLib::ncx, c0, c1, t);
			break;

		case gate_ids::ncrz:
		default:
			break;
		}
		this->emplace_op(op_type(g, c0, c1, t));
	}

	void create_op(Gate const& g, std::vector<wire::Id> const& controls,
	    std::vector<wire::Id> const& targets)
	{
		if (params_.gate_set & (1ull << static_cast<uint32_t>(g.id())))
		{
			if (controls.size()
			    <= params_.barenco_controls_threshold) {
				this->emplace_op(op_type(g, controls, targets));
				return;
			}
		}

		if (controls.size() == 2u) {
			create_op(
			    g, controls.at(0), controls.at(1), targets.at(0));
			return;
		} else if (controls.size() > params_.barenco_controls_threshold)
		{
			barenco_create_op(g, controls, targets.at(0));
			return;
		}
		diagonal_create_op(g, controls, targets.at(0));
	}
#pragma endregion

private:
	void barenco_create_op(Gate const& g,
	    std::vector<wire::Id> const& controls, wire::Id target)
	{
		if (controls.size() + 1u == this->num_qubits()) {
			this->create_qubit(wire::Mode::ancilla);
		}
		if (params_.gate_set & (1ull << static_cast<uint32_t>(g.id())))
		{
			barenco_decomp(
			    *this, g, controls, target, barenco_params_);
			return;
		}
		switch (g.id()) {
		// Non-parameterisable gates
		case gate_ids::ncx:
			barenco_decomp(
			    *this, g, controls, target, barenco_params_);
			return;

		case gate_ids::ncy:
			create_op(GateLib::sdg, target);
			barenco_decomp(*this, GateLib::ncx, controls, target,
			    barenco_params_);
			create_op(GateLib::s, target);
			break;

		case gate_ids::ncz:
			create_op(GateLib::h, target);
			barenco_decomp(*this, GateLib::ncx, controls, target,
			    barenco_params_);
			create_op(GateLib::h, target);
			break;

		// Parameterisable gates
		case gate_ids::ncrx:
			break;

		case gate_ids::ncry:
			break;

		case gate_ids::ncrz:
			break;

		default:
			break;
		}
	}

	void diagonal_create_op(
	    Gate const& g, std::vector<wire::Id> controls, wire::Id target)
	{
		controls.emplace_back(target);
		std::vector<Angle> angles(
		    (1 << controls.size()), sym_angle::zero);

		switch (g.id()) {
		// Non-parameterisable gates
		case gate_ids::ncx:
			angles.back() = sym_angle::pi;
			create_op(GateLib::h, target);
			diagonal_synth(*this, controls, angles);
			create_op(GateLib::h, target);
			return;

		case gate_ids::ncy:
			angles.back() = sym_angle::pi;
			create_op(GateLib::sdg, target);
			create_op(GateLib::h, target);
			diagonal_synth(*this, controls, angles);
			create_op(GateLib::h, target);
			create_op(GateLib::s, target);
			break;

		case gate_ids::ncz:
			angles.back() = sym_angle::pi;
			diagonal_synth(*this, controls, angles);
			break;

		// Parameterisable gates
		case gate_ids::ncrx:
			angles.at(angles.size() - 2u) = -g.rotation_angle();
			angles.at(angles.size() - 1u) = g.rotation_angle();
			create_op(GateLib::h, target);
			diagonal_synth(*this, controls, angles);
			create_op(GateLib::h, target);
			break;

		case gate_ids::ncry:
			break;

		case gate_ids::ncrz:
			angles.at(angles.size() - 2u) = -g.rotation_angle();
			angles.at(angles.size() - 1u) = g.rotation_angle();
			diagonal_synth(*this, controls, angles);
			break;

		default:
			break;
		}
	}

private:
	decomp_params params_;
	barenco_params barenco_params_;
};

} // namespace detail
#pragma endregion

/*! \brief
 *
 * \tparam Circuit the circuit type.
 * \param[in] circuit the original quantum circuit (__will not be modified__).
 * \returns a decomposed circuit.
 */
template<typename Circuit>
Circuit decompose(Circuit const circuit, decomp_params params = {})
{
	using op_type = typename Circuit::op_type;
	Circuit result = shallow_duplicate(circuit);
	detail::decomp_builder decomp(result, params);

	circuit.foreach_op([&](op_type const& op) {
		if (op.is_one_qubit()) {
			decomp.create_op(op, op.target());
		} else if (op.is_two_qubit()) {
			decomp.create_op(op, op.control(), op.target());
		} else {
			std::vector<wire::Id> controls;
			std::vector<wire::Id> targets;
			op.foreach_control([&](wire::Id control) {
				controls.push_back(control);
			});
			op.foreach_target([&](wire::Id target) {
				targets.push_back(target);
			});
			decomp.create_op(op, controls, targets);
		}
	});
	return result;
}

} // namespace tweedledum
