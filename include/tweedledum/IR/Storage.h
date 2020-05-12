/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "Node.h"

#include <cstdint>
#include <string>
#include <vector>

namespace tweedledum {

template<typename NodeType>
struct Storage {
	Storage(std::string_view name_ = {}) : gate_set(0), name(name_) {}

	uint32_t default_value;
	uint64_t gate_set;
	std::string name;
	std::vector<node::Id> inputs;
	std::vector<node::Id> outputs;
	std::vector<NodeType> nodes;
};

} // namespace tweedledum
