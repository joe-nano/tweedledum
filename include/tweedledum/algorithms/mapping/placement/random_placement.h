/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../../ir/Wire.h"
#include "../../../target/Device.h"

#include <random>
#include <vector>

namespace tweedledum::detail {

/*! \brief Yet to be written.
 */
inline std::vector<wire::Id> random_placement(
    Device const& device, uint32_t seed = 17u)
{
	std::vector<wire::Id> placement;
	for (uint32_t i = 0u; i < device.num_qubits(); ++i) {
		placement.emplace_back(i, /* is_qubit */ true);
	}
	std::mt19937 rnd(seed);
	std::shuffle(placement.begin(), placement.end(), rnd);
	return placement;
}

} // namespace tweedledum::detail
