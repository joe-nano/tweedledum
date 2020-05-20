/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "shallow_duplicate.h"

#include <type_traits>

namespace tweedledum {

/*! \brief Generic circuit rewriting function
 *
 * The parameter ``fn`` is any callable that must have the following signature:
 * - ``bool(Circuit&, Operation const&)``
 *
 * If ``fn`` returns ``true`` then it means that the gate was rewritten and
 * nothing else will be done.  In case it returns ``false``, then it means the
 * current gate must be copied to the new circuit.
 *
 * \param[in] original the original quantum circuit (__will not be modified__).
 * \param[in] fn a rewriting function.
 * \param[in] num_ancillae number of ancillae to add to the circuit.
 * \returns a _new_ rewritten circuit.
 */
template<class RewriteFn>
Circuit rewrite_circuit(Circuit const original, RewriteFn&& fn)
{
	static_assert(std::is_invocable_r_v<bool, RewriteFn, Circuit&,
	                  typename Circuit::op_type const&>,
	    "The rewriting function signature _must_ be bool(Circuit&, Operation const&)");

	Circuit result = shallow_duplicate(original);
	using op_type = typename Circuit::op_type;
	original.foreach_op([&](op_type const& op) {
		if (!fn(result, op)) {
			result.emplace_op(op);
		}
	});
	return result;
}

} // namespace tweedledum
