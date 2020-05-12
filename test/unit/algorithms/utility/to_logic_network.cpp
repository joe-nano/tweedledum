/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/utility/to_logic_network.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/ir/operations/w3_op.h"
#include "tweedledum/ir/operations/wn32_op.h"

#include <catch.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/static_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/xag.hpp>

using namespace mockturtle;
using namespace tweedledum;

TEMPLATE_PRODUCT_TEST_CASE("Conver simple quantum circuit to XAG",
    "[to_logic_network][template]", (CircuitDAG, Netlist), (w3_op, wn32_op))
{
	TestType quantum_ntk;
	wire::Id q0 = quantum_ntk.create_qubit("__i_0", wire::Mode::in);
	wire::Id q1 = quantum_ntk.create_qubit("__i_1", wire::Mode::in);
	wire::Id q2 = quantum_ntk.create_qubit("__o_0", wire::Mode::out);

	quantum_ntk.create_op(GateLib::ncx, std::vector<wire::Id>({q0, q1}),
	    std::vector<wire::Id>({q2}));

	const auto logic_ntk = to_logic_network<xag_network>(quantum_ntk);

	kitty::dynamic_truth_table function(2u);
	kitty::create_from_binary_string(function, "1000");
	default_simulator<kitty::dynamic_truth_table> sim(function.num_vars());
	CHECK(simulate<kitty::dynamic_truth_table>(logic_ntk, sim)[0]
	      == function);
}
