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
#include "placement/hsat_placement.h"
#include "routing/sabre_router.h"

#include <vector>

namespace tweedledum {

/*! \brief SABRE-base mappper
 *
   \verbatim embed:rst

   Mapper based on the SABRE algorithm :cite:`SABRE19`.

   \endverbatim
 */
inline MappedDAG sabre_map(Circuit const original, Device const& device,
    sabre_config const& config = {})
{
	detail::sabre_router router(device, config);
	std::vector<wire::Id> placement
	    = detail::hsat_placement(original, device);
	MappedDAG mapped = router.route(original, placement);
	mapped = router.route(reverse(original), mapped.v_to_phy());
	return mapped;
}

} // namespace tweedledum
