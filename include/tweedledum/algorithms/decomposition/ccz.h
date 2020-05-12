/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Gate.h"
#include "../../ir/Wire.h"

namespace tweedledum::detail {

/*                                                           ┌───┐
 *  ──────────────●───────────────────●─────────●─────────●──┤ T ├
 *                │                   │         │         │  └───┘
 *                │                   │       ┌─┴─┐┌───┐┌─┴─┐┌───┐
 *  ────●─────────┼─────────●─────────┼───────┤ X ├┤ ┴ ├┤ X ├┤ T ├
 *      │         │         │         │       └───┘└───┘└───┘└───┘
 *    ┌─┴─┐┌───┐┌─┴─┐┌───┐┌─┴─┐┌───┐┌─┴─┐                    ┌───┐
 *  ──┤ X ├┤ ┴ ├┤ X ├┤ T ├┤ X ├┤ ┴ ├┤ X ├────────────────────┤ T ├
 *    └───┘└───┘└───┘└───┘└───┘└───┘└───┘                    └───┘
 *
 * NOTE: Important normalization: if only one control is complemented, it must
 * be x. This means that a complemented `y` implies (->) a complemented `x`.
 */
template <typename Network>
void ccz(Network& network, wire::Id x, wire::Id y, wire::Id const z)
{
	assert(!z.is_complemented());
	if (y.is_complemented() && !x.is_complemented()) {
		std::swap(x, y);
	}
	network.create_op(GateLib::cx, y.wire(), z);
	network.create_op(x.is_complemented() ? GateLib::t : GateLib::tdg, z);
	network.create_op(GateLib::cx, x.wire(), z);
	network.create_op(GateLib::t, z);
	network.create_op(GateLib::cx, y.wire(), z);
	network.create_op(y.is_complemented() ? GateLib::t : GateLib::tdg, z);
	network.create_op(GateLib::cx, x.wire(), z);
	network.create_op(x.is_complemented() && !y.is_complemented() ?
	                      GateLib::tdg :
	                      GateLib::t,
	    z);

	network.create_op(GateLib::cx, x.wire(), y.wire());
	network.create_op(GateLib::tdg, y.wire());
	network.create_op(GateLib::cx, x.wire(), y.wire());
	network.create_op(
	    y.is_complemented() ? GateLib::tdg : GateLib::t, x.wire());
	network.create_op(
	    x.is_complemented() ? GateLib::tdg : GateLib::t, y.wire());
}

/* Better T gate parallelization at the expense of an extra CNOT gate.
 *
 *   ┌───┐          ┌───┐┌───┐┌───┐┌───┐     ┌───┐
 * ──┤ T ├──●───────┤ X ├┤ ┴ ├┤ X ├┤ ┴ ├─────┤ X ├──●──
 *   └───┘  │       └─┬─┘└───┘└─┬─┘└───┘     └─┬─┘  │
 *   ┌───┐┌─┴─┐       │  ┌───┐  │              │  ┌─┴─┐
 * ──┤ T ├┤ X ├──●────┼──┤ ┴ ├──●─────────●────┼──┤ X ├
 *   └───┘└───┘  │    │  └───┘            │    │  └───┘
 *   ┌───┐     ┌─┴─┐  │  ┌───┐          ┌─┴─┐  │
 * ──┤ T ├─────┤ X ├──●──┤ T ├──────────┤ X ├──●───────
 *   └───┘     └───┘     └───┘          └───┘
 *
 * NOTE: Important normalization: if only one control is complemented, it must
 * be x. This means that a complemented `y` implies (->) a complemented `x`.
 */
template <typename Network>
void ccz_tpar(Network& network, wire::Id x, wire::Id y, wire::Id const z)
{
	assert(!z.is_complemented());
	if (y.is_complemented() && !x.is_complemented()) {
		std::swap(x, y);
	}
	network.create_op(
	    y.is_complemented() ? GateLib::tdg : GateLib::t, x.wire());
	network.create_op(
	    x.is_complemented() ? GateLib::tdg : GateLib::t, y.wire());
	network.create_op(x.is_complemented() && !y.is_complemented() ?
	                      GateLib::tdg :
	                      GateLib::t,
	    z);

	network.create_op(GateLib::cx, x.wire(), y.wire());
	network.create_op(GateLib::cx, y.wire(), z);
	network.create_op(GateLib::cx, z, x.wire());

	network.create_op(
	    x.is_complemented() ? GateLib::t : GateLib::tdg, x.wire());
	network.create_op(GateLib::tdg, y.wire());
	network.create_op(GateLib::t, z);

	network.create_op(GateLib::cx, y.wire(), x.wire());
	network.create_op(
	    y.is_complemented() ? GateLib::t : GateLib::tdg, x.wire());
	network.create_op(GateLib::cx, y.wire(), z);
	network.create_op(GateLib::cx, z, x.wire());
	network.create_op(GateLib::cx, x.wire(), y.wire());
}

/* Relative phase CCZ
 *
 *   ┌───┐          ┌───┐┌───┐┌───┐┌───┐     ┌───┐
 * ──┤ T ├──●───────┤ X ├┤ ┴ ├┤ X ├┤ ┴ ├─────┤ X ├──●──
 *   └───┘  │       └─┬─┘└───┘└─┬─┘└───┘     └─┬─┘  │
 *   ┌───┐┌─┴─┐       │  ┌───┐  │              │  ┌─┴─┐
 * ──┤ T ├┤ X ├──●────┼──┤ ┴ ├──●─────────●────┼──┤ X ├
 *   └───┘└───┘  │    │  └───┘            │    │  └───┘
 *   ┌───┐     ┌─┴─┐  │  ┌───┐          ┌─┴─┐  │
 * ──┤ T ├─────┤ X ├──●──┤ T ├──────────┤ X ├──●───────
 *   └───┘     └───┘     └───┘          └───┘
 *
 * NOTE: Important normalization: if only one control is complemented, it must
 * be x. This means that a complemented `y` implies (->) a complemented `x`.
 */
template <typename Network>
void ccz_rp(Network& network, wire::Id x, wire::Id y, wire::Id const z)
{
	network.add_gate(GateLib::t, z);
	network.add_gate(GateLib::cx, y, z);
	network.add_gate(GateLib::tdg, z);
	network.add_gate(GateLib::cx, x, z);
	network.add_gate(GateLib::t, z);
	network.add_gate(GateLib::cx, y, z);
	network.add_gate(GateLib::tdg, z);
}

} // namespace tweedledum::detail