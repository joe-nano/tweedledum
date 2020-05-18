/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/diagonal_synth.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/support/Angle.h"

#include <catch.hpp>
#include <sstream>

using namespace tweedledum;
TEST_CASE("Synthesize diagonal unitaries", "[diagonal_synth]")
{
	//TODO: CHECK
	Module module;
	SECTION("One-qubit")
	{
		std::vector<Angle> angles = {sym_angle::zero, sym_angle::pi};
		diagonal_synth(module, angles);
	}
	SECTION("Two-qubit controlled r1(pi)")
	{
		std::vector<Angle> angles = {sym_angle::zero, sym_angle::zero,
		    sym_angle::zero, sym_angle::pi};
		diagonal_synth(module, angles);
	}
	SECTION("Two-qubit controlled rz(pi/2)")
	{
		std::vector<Angle> angles = {sym_angle::zero, sym_angle::zero,
		    -sym_angle::pi_half, sym_angle::pi_half};
		diagonal_synth(module, angles);
	}
	SECTION("Three-qubit Toffoli")
	{
		std::vector<Angle> angles(7, sym_angle::zero);
		angles.push_back(sym_angle::pi_half);
		diagonal_synth(module, angles);
	}
	SECTION("Three-qubit Toffoli")
	{
		std::vector<Angle> angles(6, sym_angle::zero);
		angles.push_back(-sym_angle::pi);
		angles.push_back(sym_angle::pi);
		diagonal_synth(module, angles);
	}
}
