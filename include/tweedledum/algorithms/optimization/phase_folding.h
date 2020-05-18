/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/Gate.h"
#include "../../ir/Wire.h"
#include "../../support/Angle.h"
#include "../../support/ParityMap.h"

#include <cstdint>
#include <limits>
#include <vector>

namespace tweedledum {

/*! \brief TODO
 *
 * This algorithm merges phase gates that are applied to the same computational
 * paths.
 */
inline Circuit phase_folding(Circuit const original)
{
	using op_type = typename Circuit::op_type;
	using sum_type = std::vector<uint32_t>;
	constexpr uint32_t qid_max = std::numeric_limits<uint32_t>::max();

	Circuit optimized(original.module());
	uint32_t num_path_vars = 1u;
	std::vector<uint32_t> wire_to_qid(original.num_wires(), qid_max);
	std::vector<sum_type> qubit_pathsum;

	original.foreach_wire([&](wire::Id w_id, std::string_view name) {
		if (!w_id.is_qubit()) {
			optimized.create_cbit(name);
			return;
		};
		optimized.create_qubit(name);
		wire_to_qid.at(w_id) = qubit_pathsum.size();
		qubit_pathsum.emplace_back(1u, (num_path_vars++ << 1));
	});

	ParityMap<sum_type> parities;
	original.foreach_op([&](op_type const& op) {
		uint32_t t_qid = wire_to_qid.at(op.target());
		if (op.axis() == rot_axis::z) {
			parities.add_term(
			    qubit_pathsum.at(t_qid), op.rotation_angle());
		}
		if (op.id() == gate_ids::x) {
			if (qubit_pathsum.at(t_qid).at(0) == 1u) {
				qubit_pathsum.at(t_qid).erase(
				    qubit_pathsum.at(t_qid).begin());
				return;
			}
			qubit_pathsum.at(t_qid).insert(
			    qubit_pathsum.at(t_qid).begin(), 1u);
			return;
		}
		if (op.id() == gate_ids::cx) {
			uint32_t c_qid = wire_to_qid.at(op.control());
			std::vector<uint32_t> optimized;
			std::set_symmetric_difference(
			    qubit_pathsum.at(c_qid).begin(),
			    qubit_pathsum.at(c_qid).end(),
			    qubit_pathsum.at(t_qid).begin(),
			    qubit_pathsum.at(t_qid).end(),
			    std::back_inserter(optimized));
			qubit_pathsum.at(t_qid) = optimized;
			return;
		}
		if (op.id() == gate_ids::swap) {
			uint32_t t1_qid = wire_to_qid.at(op.target(1u));
			std::swap(
			    qubit_pathsum.at(t_qid), qubit_pathsum.at(t1_qid));
			return;
		}
		qubit_pathsum.at(t_qid).clear();
		qubit_pathsum.at(t_qid).emplace_back((num_path_vars++ << 1));
	});

	original.foreach_op([&](op_type const& op) {
		if (op.axis() == rot_axis::z) {
			return;
		}
		optimized.emplace_op(op);

		// Could do better that recalculate this
		uint32_t t_qid = wire_to_qid.at(op.target());
		if (op.id() == gate_ids::x) {
			if (qubit_pathsum.at(t_qid).at(0) == 1u) {
				qubit_pathsum.at(t_qid).erase(
				    qubit_pathsum.at(t_qid).begin());
			} else {
				qubit_pathsum.at(t_qid).insert(
				    qubit_pathsum.at(t_qid).begin(), 1u);
			}
		} else if (op.id() == gate_ids::cx) {
			uint32_t c_qid = wire_to_qid.at(op.control());
			std::vector<uint32_t> optimized;
			std::set_symmetric_difference(
			    qubit_pathsum.at(c_qid).begin(),
			    qubit_pathsum.at(c_qid).end(),
			    qubit_pathsum.at(t_qid).begin(),
			    qubit_pathsum.at(t_qid).end(),
			    std::back_inserter(optimized));
			qubit_pathsum.at(t_qid) = optimized;
		} else if (op.id() == gate_ids::swap) {
			uint32_t t1_qid = wire_to_qid.at(op.target(1u));
			std::swap(
			    qubit_pathsum.at(t_qid), qubit_pathsum.at(t1_qid));
			return; // No need to add an angle, would already have
			        // done it!
		} else {
			qubit_pathsum.at(t_qid).clear();
			qubit_pathsum.at(t_qid).emplace_back(
			    (num_path_vars++ << 1));
		}

		// Check if I need to add a rotation Z
		Angle rot_angle
		    = parities.extract_term(qubit_pathsum.at(t_qid));
		if (rot_angle == sym_angle::zero) {
			return;
		}
		optimized.create_op(GateLib::identified_r1(rot_angle), op.target());
	});
	return optimized;
}

} // namespace tweedledum
