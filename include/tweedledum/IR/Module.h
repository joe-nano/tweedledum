/*-------------------------------------------------------------------------------------------------
| Part of the tweedledum project.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "Circuit.h"

namespace tweedledum {

/*! \brief 
 *
 */
class Module {
public:
	explicit Module() : circuit_()
	{}

	Circuit circuit_;
};

} // namespace tweedledum
