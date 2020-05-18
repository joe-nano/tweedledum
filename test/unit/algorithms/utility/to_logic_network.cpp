/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/utility/to_logic_network.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/static_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/xag.hpp>

using namespace mockturtle;
using namespace tweedledum;

TEST_CASE("Conver simple quantum circuit to XAG", "[to_logic_network]")
{
	Circuit circuit(nullptr);
	wire::Id q0 = circuit.create_qubit("__i_0", wire::Mode::in);
	wire::Id q1 = circuit.create_qubit("__i_1", wire::Mode::in);
	wire::Id q2 = circuit.create_qubit("__o_0", wire::Mode::out);

	circuit.create_op(GateLib::ncx, std::vector<wire::Id>({q0, q1}),
	    std::vector<wire::Id>({q2}));

	const auto logic_ntk = to_logic_network<xag_network>(circuit);

	kitty::dynamic_truth_table function(2u);
	kitty::create_from_binary_string(function, "1000");
	default_simulator<kitty::dynamic_truth_table> sim(function.num_vars());
	CHECK(simulate<kitty::dynamic_truth_table>(logic_ntk, sim)[0]
	      == function);
}
