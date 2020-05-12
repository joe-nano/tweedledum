/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../utility/shallow_duplicate.h"

#include <type_traits>

namespace tweedledum {

/*! \defgroup remove_marked This is a hack for documentation :(
 *  \{
 */
/*! \brief Remove marked operations.
 *
 * For this function a node is marked when its value, returned by
 * `circuit.value(node)`, is not zero. A __new__ circuit is created and all
 * nodes that are not marked will be copied into it.
 *
 * __NOTE__: This function requires a template parameter that cannot be
 * inferred.  This is useful when removing nodes and creating a different
 * circuit representation, e.g. `CircuitDAG` <-> `Netlist`
 *
 * __NOTE__: Operation type _must_ be the same.
 *
 * \tparam Circuit the circuit type.
 * \tparam NewCircuit the resulting circuit type.
 * \param[in] original the original quantum circuit (__will not be modified__).
 * \returns a __new__ circuit without the marked operations.
 */
template<class NewCircuit, class Circuit>
NewCircuit remove_marked(Circuit const original)
{
	using op_type = typename Circuit::op_type;
	using node_type = typename Circuit::node_type;

	static_assert(std::is_same_v<typename Circuit::op_type,
	                  typename NewCircuit::op_type>,
	    "Operation type _must_ be the same");

	NewCircuit result = shallow_duplicate<NewCircuit>(original);
	original.foreach_op([&](op_type const& op, node_type const& node) {
		if (original.value(node)) {
			return;
		}
		result.emplace_op(op);
	});
	return result;
}

/*! \brief Remove marked operations.
 *
 * For this function a node is marked when its value, returned by
 * `circuit.value(node)`, is not zero. A __new__ circuit is created and all
 * nodes that are not marked will be copied into it.
 *
 * __NOTE__: the input and output circuits are of the same type.
 *
 * \tparam Circuit the circuit type.
 * \param[in] original the original quantum circuit (__will not be modified__).
 * \returns a __new__ circuit without the marked operations.
 */
template<class Circuit>
Circuit remove_marked(Circuit const original)
{
	return remove_marked<Circuit, Circuit>(original);
}
/*! \} */

} // namespace tweedledum
