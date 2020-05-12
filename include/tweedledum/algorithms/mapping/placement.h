/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Wire.h"
#include "../../target/Device.h"
#include "placement/hsat_placement.h"
#include "placement/line_placement.h"
#include "placement/random_placement.h"

namespace tweedledum {

/*! \brief Parameters for `placment`. */
struct placement_config {
	enum class methods {
		/*! \brief (**default**) */
		greedy,
		/*! \brief */
		greedy_sat,
		/*! \brief */
		line,
		/*! \brief */
		random,
	};
	methods method = methods::greedy_sat;
	uint32_t random_seed = 17u;
};

/*! \brief Yet to be written.
 */
template<typename Circuit>
std::vector<wire::Id> placement(
    Circuit const circuit, device const& device, placement_config params = {})
{
	using methods = placement_config::methods;

	switch (params.method) {
	case methods::greedy:
		return {};

	case methods::greedy_sat:
		return detail::hsat_placement(circuit, device);

	case methods::line:
		return detail::line_placement(circuit, device);

	case methods::random:
		return detail::random_placement(device, params.random_seed);
	}
}

} // namespace tweedledum
