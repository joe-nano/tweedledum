/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/Wire.h"

#include <string>

namespace tweedledum {

/*! \brief Creates a new circuit with same wires as the original.
 *
 * \param[in] original the original quantum circuit (__will not be modified__).
 * \param[in] name the name of the new circuit (default: same as the original).
 * \returns a __new__ circuit without gates.
 */
Circuit shallow_duplicate(Circuit const original, std::string_view name = {})
{
	Circuit duplicate(original.module(), name.empty() ? original.name() : name);
	original.foreach_wire([&](wire::Id wire, std::string_view name) {
		if (wire.is_qubit()) {
			duplicate.create_qubit(name, original.wire_mode(wire));
		} else {
			duplicate.create_cbit(name);
		}
	});
	return duplicate;
}

} // namespace tweedledum
