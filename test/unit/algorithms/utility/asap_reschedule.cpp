/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/utility/asap_reschedule.h"

#include "tweedledum/algorithms/analysis/check_layered.h"
#include "tweedledum/algorithms/verification/unitary_verify.h"
#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Node.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/views/LayersView.h"

#include <catch.hpp>

using namespace tweedledum;

TEST_CASE("ASAP reschedule", "[asap_reschedule][utility]")
{
	Circuit circuit(nullptr);
	wire::Id const q0 = circuit.create_qubit();
	wire::Id const q1 = circuit.create_qubit();
	wire::Id const q2 = circuit.create_qubit();

	circuit.create_op(GateLib::h, q0);
	circuit.create_op(GateLib::cz, q1, q0);
	circuit.create_op(GateLib::h, q0);
	circuit.create_op(GateLib::h, q2);
	CHECK_FALSE(check_layered(circuit));

	Circuit rescheduled = asap_reschedule(circuit);
	CHECK(rescheduled.node(node::Id(4u)).op.is(gate_ids::h));
	CHECK(check_layered(rescheduled));
	CHECK(unitary_verify(circuit, rescheduled));
}