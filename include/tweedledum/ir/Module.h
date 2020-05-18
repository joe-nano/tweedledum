/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "CircuitDAG.h"
#include "Gate.h"
#include "GateLib.h"

namespace tweedledum {

// TODO
class Module {
public:
	explicit Module() : circuit_(this)
	{
		register_gates(gate_lib_);
	}

	CircuitDAG circuit_;
	GateLib2 gate_lib_;
	phmap::node_hash_set<Gate> gates_;
};

} // namespace tweedledum
