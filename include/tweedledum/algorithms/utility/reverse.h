/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../utility/shallow_duplicate.h"

#include <type_traits>

namespace tweedledum {

/*! \defgroup reverse This is a hack for documentation :(
 *  \{
 */
/*! \brief Reverse a circuit.
 *
 * __NOTE__: This function requires a template parameter that cannot be
 * inferred.  This is useful when reversing and creating a different circuit
 * representation, e.g. `op_graph` <-> `Netlist`
 *
 * __NOTE__: Operation type __must__ be the same.
 *
 * \tparam Circuit the original circuit type.
 * \tparam NewCircuit the resulting circuit type.
 * \param[in] original the original quantum circuit (__will not be modified__).
 * \returns a __new__ reversed circuit.
 */
template<class NewCircuit, class Circuit>
NewCircuit reverse(Circuit const original)
{
	static_assert(std::is_same_v<typename Circuit::op_type,
	                  typename NewCircuit::op_type>,
	    "Operation type _must_ be the same");

	using op_type = typename Circuit::op_type;
	NewCircuit result = shallow_duplicate<NewCircuit>(original);
	original.foreach_rop([&](op_type const& op) {
		result.emplace_op(op);
	});
	return result;
}

/*! \brief Reverse a circuit.
 *
 * __NOTE__: the input and output circuits are of the same type.
 *
 * \tparam Circuit the circuit type.
 * \param[in] original the original quantum circuit (__will not be modified__).
 * \return a __new__ reversed circuit.
 */
template<class Circuit>
Circuit reverse(Circuit const original)
{
	return reverse<Circuit, Circuit>(original);
}
/*! \} */

} // namespace tweedledum
