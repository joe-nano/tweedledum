/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "CircuitDAG.h"
#include "operations/w3_op.h"

namespace tweedledum {

// TODO
class Module {
public:
	explicit Module() : circuit_() {}

	CircuitDAG<w3_op> circuit_;
};

} // namespace tweedledum
