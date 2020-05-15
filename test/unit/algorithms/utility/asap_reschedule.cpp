/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/utility/asap_reschedule.h"

#include "tweedledum/algorithms/analysis/check_layered.h"
#include "tweedledum/algorithms/verification/unitary_verify.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Node.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/views/LayersView.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_TEST_CASE(
    "ASAP reschedule", "[asap_reschedule][transformations]", (CircuitDAG))
{
	TestType network;
	wire::Id const q0 = network.create_qubit();
	wire::Id const q1 = network.create_qubit();
	wire::Id const q2 = network.create_qubit();

	network.create_op(GateLib::h, q0);
	network.create_op(GateLib::cz, q1, q0);
	network.create_op(GateLib::h, q0);
	network.create_op(GateLib::h, q2);
	CHECK_FALSE(check_layered(network));

	TestType rescheduled = asap_reschedule(network);
	CHECK(rescheduled.node(node::Id(4u)).op.is(gate_ids::h));
	CHECK(check_layered(rescheduled));
	CHECK(unitary_verify(network, rescheduled));
}