/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/Unitary.h"

namespace tweedledum {

/*! \brief Check if two networks are equivalent using unitaries.
 *
 * Needless to say that this method is not scalable at all! But its good to
 * equivalent check small examples and test cases.
 *
 * \param[in] circuit0, circuit1 the circuits that will be check for equivalence.
 * \param[in] rtol Relative tolerance.
 * \param[in] atol Absolute tolerance.
 * \returns true if the circuits are equivalent.
 */
inline bool unitary_verify(Circuit const& circuit0, Circuit const& circuit1,
    double const rtol = 1e-05, double const atol = 1e-08)
{
	Unitary u0(circuit0);
	Unitary u1(circuit1);
	return u0.is_apprx_equal(u1, rtol, atol);
}

} // namespace tweedledum
