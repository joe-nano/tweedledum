/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/MappedDAG.h"
#include "../../ir/Wire.h"
#include "../../target/Device.h"
#include "../utility/reverse.h"
#include "placement/line_placement.h"
#include "routing/jit_router.h"

#include <vector>

namespace tweedledum {

/*! \brief Yet to be written.
 */
inline MappedDAG jit_map(
    Circuit const original, Device const& device, jit_config const& config = {})
{
	detail::jit_router router(device, config);
	auto reversed = reverse(original);
	std::vector<wire::Id> placement
	    = detail::line_placement(reversed, device);
	MappedDAG mapped = router.route(original, placement, false);
	mapped = router.route(reversed, mapped.v_to_phy());
	return mapped;
}

} // namespace tweedledum
