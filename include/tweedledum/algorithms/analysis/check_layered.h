/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../views/LayersView.h"

namespace tweedledum {

/*! \brief Check if a DAG circuit representation is layered.
 *
 * A topological order is defined to be layered if all nodes of the i-th layer
 * appear before all nodes of the (i+1)-th layer.
 *
 * \param[in] circuit the DAG circuit.
 * \returns true if the circuit is layered.
 */
inline bool check_layered(Circuit const circuit)
{
	using node_type = typename Circuit::node_type;
	LayersView layered(circuit);
	uint32_t layer = 0u;
	bool is_layered = true;
	layered.foreach_node([&](node_type const& node) {
		if (layered.layer(node) < layer) {
			is_layered = false;
		}
		layer = std::max(layer, layered.layer(node));
	});
	return is_layered;
}

} // namespace tweedledum
