/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/MappedDAG.h"
#include "../../ir/Wire.h"
#include "../../target/Device.h"
#include "token_swap/a_star_swap.h"
#include "token_swap/parameters.h"
#include "token_swap/sat_swap.h"

#include <utility>
#include <vector>

namespace tweedledum {

void swap_network(MappedDAG& circuit, Device& topology,
    std::vector<wire::Id> const& phy_to_v, swap_network_params params = {})
{
	std::vector<wire::Id> init = circuit.phy_to_v();
	std::vector<std::pair<uint32_t, uint32_t>> swaps;

	std::vector<uint32_t> initial(init.begin(), init.end());
	std::vector<uint32_t> final(phy_to_v.begin(), phy_to_v.end());

	switch (params.method) {
	case swap_network_params::methods::admissible:
	case swap_network_params::methods::non_admissible:
		swaps = detail::a_star_swap(topology, initial, final, params);
		break;

	case swap_network_params::methods::sat:
		swaps = detail::sat_swap(topology, initial, final, params);
		break;
	}
	for (auto [x, y] : swaps) {
		circuit.create_op(
		    GateLib::swap, wire::make_qubit(x), wire::make_qubit(y));
		std::swap(init.at(x), init.at(y));
	}
	std::vector<wire::Id> v_to_phy(topology.num_qubits(), wire::invalid_id);
	for (uint32_t i = 0u; i < phy_to_v.size(); ++i) {
		v_to_phy.at(phy_to_v.at(i)) = wire::make_qubit(i);
	}
	circuit.v_to_phy(v_to_phy);
}

} // namespace tweedledum
