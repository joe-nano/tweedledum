/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/MappedDAG.h"
#include "../../ir/Wire.h"
#include "../../target/Device.h"
#include "placement/sat_placement.h"

namespace tweedledum {

/*! \brief Yet to be written.
 */
inline MappedDAG sat_map(Circuit const original, Device const& device)
{
	using op_type = typename Circuit::op_type;
	MappedDAG mapped(original, device);

	std::vector<wire::Id> v_to_phy = detail::sat_place(original, device);
	if (v_to_phy.empty()) {
		return mapped;
	}

	std::vector<wire::Id> wire_to_v(original.num_wires(), wire::invalid_id);
	original.foreach_wire([&](wire::Id wire, std::string_view name) {
		wire_to_v.at(wire) = mapped.wire(name);
	});

	mapped.v_to_phy(v_to_phy);
	original.foreach_op([&](op_type const& op) {
		wire::Id const phy0 = v_to_phy.at(wire_to_v.at(op.target()));
		if (op.is_one_qubit()) {
			mapped.create_op(op.gate(), phy0);
		} else if (op.is_two_qubit()) {
			wire::Id const phy1
			    = v_to_phy.at(wire_to_v.at(op.control()));
			mapped.create_op(op.gate(), phy1, phy0);
		}
	});
	return mapped;
}

} // namespace tweedledum
