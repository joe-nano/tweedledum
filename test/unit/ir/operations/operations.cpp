/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/ir/operations/w2_op.h"
#include "tweedledum/support/Angle.h"

#include <catch.hpp>

using namespace tweedledum;

TEMPLATE_TEST_CASE(
    "Check correct instantiation of meta ops", "[ops][inst]", w2_op, Operation)
{
	wire::Id const qubit = wire::make_qubit(0);
	wire::Id const cbit = wire::make_cbit(1);

	TestType q_input(GateLib::input, qubit);
	TestType c_input(GateLib::input, cbit);
}

template<class Op>
void check_one_wire(Gate const& g, wire::Id const& t)
{
	Op const o(g, t);
	// Non-vector constructor
	CHECK(o.num_controls() == 0u);
	CHECK(o.num_targets() == 1u);
	CHECK(o.target() == t);

	// Vector constructor
	Op const o_vec(g, std::vector<wire::Id>({}), std::vector<wire::Id>({t}));
	CHECK(o_vec.num_controls() == 0u);
	CHECK(o_vec.num_targets() == 1u);
	CHECK(o_vec.target() == t);

	CHECK(o == o_vec);
}

template<class Op>
void check_two_wire(Gate const& g, wire::Id const& c, wire::Id const& t)
{
	Op const o(g, c, t);
	// Non-vector constructor
	CHECK(o.num_controls() == 1u);
	CHECK(o.num_targets() == 1u);
	CHECK(o.control() == c);
	CHECK(o.target() == t);

	// Vector constructor
	Op const o_vec(
	    g, std::vector<wire::Id>({c}), std::vector<wire::Id>({t}));
	CHECK(o_vec.num_controls() == 1u);
	CHECK(o_vec.num_targets() == 1u);
	CHECK(o_vec.control() == c);
	CHECK(o_vec.target() == t);

	CHECK(o == o_vec);
}

TEMPLATE_TEST_CASE("Check correct instantiation of non-parameterasible 1 and 2 "
                   "wires operations",
    "[ops][inst]", w2_op, Operation)
{
	wire::Id const w0 = wire::make_qubit(0);
	wire::Id const w1 = wire::make_qubit(15);
	std::vector<Gate> one_wire
	    = {GateLib::i, GateLib::h, GateLib::x, GateLib::y, GateLib::z,
	        GateLib::s, GateLib::t, GateLib::sdg, GateLib::tdg};
	std::vector<Gate> two_wire = {GateLib::cx, GateLib::cy, GateLib::cz};

	for (Gate const& g : one_wire) {
		check_one_wire<TestType>(g, w1);
	}

	for (Gate const& g : two_wire) {
		check_two_wire<TestType>(g, w0, w1);
	}

	TestType swap(GateLib::swap, w0, w1);
	CHECK(swap.num_controls() == 0u);
	CHECK(swap.num_targets() == 2u);
	CHECK(swap.target(0) == w0);
	CHECK(swap.target(1) == w1);

	TestType swap_10(GateLib::swap, w1, w0);
	CHECK(swap == swap_10);

	TestType swap_v(GateLib::swap, std::vector<wire::Id>({}),
	    std::vector<wire::Id>({w0, w1}));
	CHECK(swap_v.num_controls() == 0u);
	CHECK(swap_v.num_targets() == 2u);
	CHECK(swap_v.target(0) == w0);
	CHECK(swap_v.target(1) == w1);

	TestType swap_v_10(GateLib::swap, std::vector<wire::Id>({}),
	    std::vector<wire::Id>({w1, w0}));
	CHECK(swap_v == swap_v_10);
}

TEMPLATE_TEST_CASE("Check correct instantiation of parameterasible 1 and 2 "
                   "wires operations",
    "[ops][inst]", w2_op, Operation)
{
	wire::Id const c = wire::make_qubit(0);
	wire::Id const t = wire::make_qubit(2);
	std::vector<Angle> commmon_angles = {sym_angle::zero, sym_angle::pi,
	    sym_angle::pi_half, sym_angle::pi_quarter};
	for (Angle const& a : commmon_angles) {
		check_one_wire<TestType>(GateLib::r1(a), t);
		check_one_wire<TestType>(GateLib::rx(a), t);
		check_one_wire<TestType>(GateLib::ry(a), t);
		check_one_wire<TestType>(GateLib::rz(a), t);
		check_two_wire<TestType>(GateLib::crx(a), c, t);
		check_two_wire<TestType>(GateLib::cry(a), c, t);
		check_two_wire<TestType>(GateLib::crz(a), c, t);
	}
}

template<class Op>
void check_three_wire(
    Gate const& g, wire::Id const c0, wire::Id const c1, wire::Id const t)
{
	Op const o(g, c0, c1, t);
	// Non-vector constructor
	CHECK(o.num_controls() == 2u);
	CHECK(o.num_targets() == 1u);
	CHECK(o.control(0) == c0);
	CHECK(o.control(1) == c1);
	CHECK(o.target() == t);

	Op const o_norm(g, c1, c0, t);
	CHECK(o == o_norm);

	// Vector constructor
	Op const o_vec(
	    g, std::vector<wire::Id>({c0, c1}), std::vector<wire::Id>({t}));
	CHECK(o_vec.num_controls() == 2u);
	CHECK(o_vec.num_targets() == 1u);
	CHECK(o_vec.control(0) == c0);
	CHECK(o_vec.control(1) == c1);
	CHECK(o_vec.target() == t);

	CHECK(o == o_vec);

	Op o_vec_norm(
	    g, std::vector<wire::Id>({c1, c0}), std::vector<wire::Id>({t}));
	CHECK(o_vec == o_vec_norm);
}

TEMPLATE_TEST_CASE("Check correct instantiation of non-parameterasible 3 wires "
                   "operations",
    "[ops][inst]", Operation)
{
	wire::Id const c0 = wire::make_qubit(0);
	wire::Id const c1 = wire::make_qubit(8);
	wire::Id const t = wire::make_qubit(15);
	std::vector<Gate> three_wire
	    = {GateLib::ncx, GateLib::ncy, GateLib::ncz};

	for (Gate const& g : three_wire) {
		check_three_wire<TestType>(g, c0, c1, t);
	}
}

TEMPLATE_TEST_CASE("Check correct instantiation of parameterasible 3 wires "
                   "operations",
    "[ops][inst]", Operation)
{
	wire::Id const c0 = wire::make_qubit(0);
	wire::Id const c1 = wire::make_qubit(1);
	wire::Id const t = wire::make_qubit(2);
	std::vector<Angle> commmon_angles = {sym_angle::zero, sym_angle::pi,
	    sym_angle::pi_half, sym_angle::pi_quarter};
	for (Angle const& a : commmon_angles) {
		check_three_wire<TestType>(GateLib::ncrx(a), c0, c1, t);
		check_three_wire<TestType>(GateLib::ncry(a), c0, c1, t);
		check_three_wire<TestType>(GateLib::ncrz(a), c0, c1, t);
	}
}

TEMPLATE_TEST_CASE(
    "Check 1 and 2 wires operations adjointness", "[ops][adj]", w2_op)
{
	wire::Id const q0 = wire::make_qubit(0);
	wire::Id const q1 = wire::make_qubit(9);
	wire::Id const q2 = wire::make_qubit(19);

	std::vector<Gate> gs
	    = {GateLib::i, GateLib::h, GateLib::x, GateLib::y, GateLib::z,
	        GateLib::s, GateLib::t, GateLib::cx, GateLib::cy, GateLib::cz};
	std::vector<Gate> gs_adjoint = {GateLib::i, GateLib::h, GateLib::x,
	    GateLib::y, GateLib::z, GateLib::sdg, GateLib::tdg, GateLib::cx,
	    GateLib::cy, GateLib::cz};
	std::vector<Angle> commmon_angles = {sym_angle::zero, sym_angle::pi,
	    sym_angle::pi_half, sym_angle::pi_quarter};
	for (Angle const& a : commmon_angles) {
		gs.emplace_back(GateLib::r1(a));
		gs.emplace_back(GateLib::rx(a));
		gs.emplace_back(GateLib::ry(a));
		gs.emplace_back(GateLib::rz(a));
		gs.emplace_back(GateLib::crx(a));
		gs.emplace_back(GateLib::cry(a));
		gs.emplace_back(GateLib::crz(a));
		gs_adjoint.emplace_back(GateLib::r1(-a));
		gs_adjoint.emplace_back(GateLib::rx(-a));
		gs_adjoint.emplace_back(GateLib::ry(-a));
		gs_adjoint.emplace_back(GateLib::rz(-a));
		gs_adjoint.emplace_back(GateLib::crx(-a));
		gs_adjoint.emplace_back(GateLib::cry(-a));
		gs_adjoint.emplace_back(GateLib::crz(-a));
	}

	std::vector<TestType> ops;
	std::vector<TestType> ops_adjoint;
	std::vector<TestType> ops_not_adjoint;
	for (uint32_t i = 0; i < gs.size(); ++i) {
		if (gs.at(i).is_one_qubit()) {
			ops.emplace_back(gs.at(i), q0);
			ops_adjoint.emplace_back(gs_adjoint.at(i), q0);
			ops_not_adjoint.emplace_back(gs.at(i), q1);
			ops_not_adjoint.emplace_back(gs_adjoint.at(i), q1);
			continue;
		}
		ops.emplace_back(gs.at(i), q0, q1);
		ops_adjoint.emplace_back(gs_adjoint.at(i), q0, q1);
		ops_not_adjoint.emplace_back(gs.at(i), q0, q2);
		ops_not_adjoint.emplace_back(gs.at(i), q1, q0);
		ops_not_adjoint.emplace_back(gs_adjoint.at(i), q1, q0);
	}
	for (uint32_t i = 0; i < ops.size(); ++i) {
		CHECK(ops.at(i).is_adjoint(ops_adjoint.at(i)));
		CHECK(ops_adjoint.at(i).is_adjoint(ops.at(i)));
		for (uint32_t j = 0; j < ops.size(); ++j) {
			if (i == j) {
				continue;
			}
			CHECK_FALSE(ops.at(i).is_adjoint(ops.at(j)));
			CHECK_FALSE(ops.at(j).is_adjoint(ops.at(i)));
		}
		for (uint32_t j = 0; j < ops_not_adjoint.size(); ++j) {
			CHECK_FALSE(ops.at(i).is_adjoint(ops_not_adjoint.at(j)));
			CHECK_FALSE(ops_not_adjoint.at(j).is_adjoint(ops.at(i)));
		}
	}
	TestType swap_0(GateLib::swap, q0, q1);
	TestType swap_1(GateLib::swap, q1, q0);
	TestType swap_2(GateLib::swap, q0, q2);
	TestType swap_3(GateLib::swap, q1, q2);
	CHECK(swap_0.is_adjoint(swap_1));
	CHECK(swap_1.is_adjoint(swap_0));
	CHECK_FALSE(swap_0.is_adjoint(swap_2));
	CHECK_FALSE(swap_0.is_adjoint(swap_3));
	CHECK_FALSE(swap_2.is_adjoint(swap_3));
}

TEMPLATE_TEST_CASE(
    "Check 1, 2 and 3 wires operations adjointness", "[ops][adj]", Operation)
{
	wire::Id const q0 = wire::make_qubit(0);
	wire::Id const q1 = wire::make_qubit(9);
	wire::Id const q2 = wire::make_qubit(19);

	std::vector<Gate> gs = {GateLib::i, GateLib::h, GateLib::x, GateLib::y,
	    GateLib::z, GateLib::s, GateLib::t, GateLib::cx, GateLib::cy,
	    GateLib::cz, GateLib::ncx, GateLib::ncy, GateLib::ncz};
	std::vector<Gate> gs_adjoint = {GateLib::i, GateLib::h, GateLib::x,
	    GateLib::y, GateLib::z, GateLib::sdg, GateLib::tdg, GateLib::cx,
	    GateLib::cy, GateLib::cz, GateLib::ncx, GateLib::ncy, GateLib::ncz};
	std::vector<Angle> commmon_angles = {sym_angle::zero, sym_angle::pi,
	    sym_angle::pi_half, sym_angle::pi_quarter};
	for (Angle const& a : commmon_angles) {
		gs.emplace_back(GateLib::r1(a));
		gs.emplace_back(GateLib::rx(a));
		gs.emplace_back(GateLib::ry(a));
		gs.emplace_back(GateLib::rz(a));
		gs.emplace_back(GateLib::crx(a));
		gs.emplace_back(GateLib::cry(a));
		gs.emplace_back(GateLib::crz(a));
		gs.emplace_back(GateLib::ncrx(a));
		gs.emplace_back(GateLib::ncry(a));
		gs.emplace_back(GateLib::ncrz(a));
		gs_adjoint.emplace_back(GateLib::r1(-a));
		gs_adjoint.emplace_back(GateLib::rx(-a));
		gs_adjoint.emplace_back(GateLib::ry(-a));
		gs_adjoint.emplace_back(GateLib::rz(-a));
		gs_adjoint.emplace_back(GateLib::crx(-a));
		gs_adjoint.emplace_back(GateLib::cry(-a));
		gs_adjoint.emplace_back(GateLib::crz(-a));
		gs_adjoint.emplace_back(GateLib::ncrx(-a));
		gs_adjoint.emplace_back(GateLib::ncry(-a));
		gs_adjoint.emplace_back(GateLib::ncrz(-a));
	}

	std::vector<TestType> ops;
	std::vector<TestType> ops_adjoint;
	std::vector<TestType> ops_not_adjoint;
	for (uint32_t i = 0; i < gs.size(); ++i) {
		if (gs.at(i).is_one_qubit()) {
			ops.emplace_back(gs.at(i), q0);
			ops_adjoint.emplace_back(gs_adjoint.at(i), q0);
			ops_not_adjoint.emplace_back(gs.at(i), q1);
			ops_not_adjoint.emplace_back(gs_adjoint.at(i), q1);
			continue;
		} else if (gs.at(i).is_two_qubit()) {
			ops.emplace_back(gs.at(i), q0, q1);
			ops_adjoint.emplace_back(gs_adjoint.at(i), q0, q1);
			ops_not_adjoint.emplace_back(gs.at(i), q0, q2);
			ops_not_adjoint.emplace_back(gs.at(i), q1, q0);
			ops_not_adjoint.emplace_back(gs_adjoint.at(i), q1, q0);
			continue;
		}
		ops.emplace_back(gs.at(i), q0, q1, q2);
		ops_adjoint.emplace_back(gs_adjoint.at(i), q0, q1, q2);
		ops_not_adjoint.emplace_back(gs.at(i), q0, q2, q1);
		ops_not_adjoint.emplace_back(gs.at(i), q1, q2, q0);
		ops_not_adjoint.emplace_back(gs_adjoint.at(i), q0, q2, q1);
	}
	for (uint32_t i = 0; i < ops.size(); ++i) {
		CHECK(ops.at(i).is_adjoint(ops_adjoint.at(i)));
		CHECK(ops_adjoint.at(i).is_adjoint(ops.at(i)));
		for (uint32_t j = 0; j < ops.size(); ++j) {
			if (i == j) {
				continue;
			}
			CHECK_FALSE(ops.at(i).is_adjoint(ops.at(j)));
			CHECK_FALSE(ops.at(j).is_adjoint(ops.at(i)));
		}
		for (uint32_t j = 0; j < ops_not_adjoint.size(); ++j) {
			CHECK_FALSE(ops.at(i).is_adjoint(ops_not_adjoint.at(j)));
			CHECK_FALSE(ops_not_adjoint.at(j).is_adjoint(ops.at(i)));
		}
	}
	TestType swap_0(GateLib::swap, q0, q1);
	TestType swap_1(GateLib::swap, q1, q0);
	TestType swap_2(GateLib::swap, q0, q2);
	TestType swap_3(GateLib::swap, q1, q2);
	CHECK(swap_0.is_adjoint(swap_1));
	CHECK(swap_1.is_adjoint(swap_0));
	CHECK_FALSE(swap_0.is_adjoint(swap_2));
	CHECK_FALSE(swap_0.is_adjoint(swap_3));
	CHECK_FALSE(swap_2.is_adjoint(swap_3));
}

template<class Op>
Op create_op(
    Gate const& g, wire::Id const c0, wire::Id const c1, wire::Id const t)
{
	if (g.is_one_qubit()) {
		return Op(g, t);
	} else if (g.is_two_qubit()) {
		return Op(g, c0, t);
	}
	return Op(g, c0, c1, t);
}

TEMPLATE_TEST_CASE(
    "Check 1 and 2 wires operations dependency", "[ops][dep]", w2_op)
{
	wire::Id const q0 = wire::make_qubit(0);
	wire::Id const q1 = wire::make_qubit(11);
	wire::Id const q2 = wire::make_qubit(12);
	wire::Id const q3 = wire::make_qubit(23);
	wire::Id const q4 = wire::make_qubit(24);
	wire::Id const q5 = wire::make_qubit(25);
	std::vector<Gate> gs = {GateLib::h, GateLib::x, GateLib::y, GateLib::z,
	    GateLib::s, GateLib::t, GateLib::sdg, GateLib::tdg, GateLib::cx,
	    GateLib::cy, GateLib::cz};

	// All acting on different qubits
	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = i; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q3, q4, q5);
			CHECK_FALSE((op_i.is_dependent(op_j)
			             || op_j.is_dependent(op_i)));
		}
	}

	// Here [q2, q3] are always targets, and [q0, q1] always controls. They
	// don't cover any of the more tricky dependency cases:
	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = i; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q0, q1, q2);
			// Different target
			TestType op_dt
			    = create_op<TestType>(gs.at(j), q0, q1, q3);
			CHECK_FALSE((op_i.is_dependent(op_dt)
			             || op_dt.is_dependent(op_i)));
			if (i == j) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			if (gs.at(i).axis() == gs.at(j).axis()) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			CHECK((op_i.is_dependent(op_j)
			       || op_j.is_dependent(op_i)));
		}
	}

	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = 8; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q2, q1, q0);
			if (gs.at(i).is_one_qubit()
			    && gs.at(i).axis() == rot_axis::z) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			} else if (gs.at(i).axis() == rot_axis::z
			           && gs.at(j).axis() == rot_axis::z) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			CHECK((op_i.is_dependent(op_j)
			       || op_j.is_dependent(op_i)));
		}
	}

	// Two-qubit gates: same target, different controls
	for (uint32_t i = 8; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = i; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q3, q4, q2);
			if (gs.at(i).axis() == gs.at(j).axis()) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			CHECK((op_i.is_dependent(op_j)
			       || op_j.is_dependent(op_i)));
		}
	}

	// The swap
	TestType swap_op0(GateLib::swap, q0, q2);
	TestType swap_op1(GateLib::swap, q1, q3);
	TestType swap_op3(GateLib::swap, q4, q5);
	CHECK_FALSE((swap_op0.is_dependent(swap_op1)
	             || swap_op1.is_dependent(swap_op0)));
	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i0 = create_op<TestType>(gs.at(i), q0, q1, q2);
		CHECK((op_i0.is_dependent(swap_op0)
		       || swap_op0.is_dependent(op_i0)));
		if (gs.at(i).is_one_qubit() || gs.at(i).is_two_qubit()) {
			CHECK_FALSE((op_i0.is_dependent(swap_op1)
			             || swap_op1.is_dependent(op_i0)));
		} else {
			CHECK((op_i0.is_dependent(swap_op1)
			       || swap_op1.is_dependent(op_i0)));
		}
		CHECK_FALSE((op_i0.is_dependent(swap_op3)
		             || swap_op3.is_dependent(op_i0)));
	}
}

TEMPLATE_TEST_CASE("Check 1, 2 and 3 wires dependency", "[ops][dep]", Operation)
{
	wire::Id const q0 = wire::make_qubit(0);
	wire::Id const q1 = wire::make_qubit(11);
	wire::Id const q2 = wire::make_qubit(12);
	wire::Id const q3 = wire::make_qubit(23);
	wire::Id const q4 = wire::make_qubit(24);
	wire::Id const q5 = wire::make_qubit(25);
	std::vector<Gate> gs = {GateLib::h, GateLib::x, GateLib::y, GateLib::z,
	    GateLib::s, GateLib::t, GateLib::sdg, GateLib::tdg, GateLib::cx,
	    GateLib::cy, GateLib::cz, GateLib::ncx, GateLib::ncy, GateLib::ncz};

	// All acting on different qubits
	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = i; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q3, q4, q5);
			CHECK_FALSE((op_i.is_dependent(op_j)
			             || op_j.is_dependent(op_i)));
		}
	}

	// Here [q2, q3] are always targets, and [q0, q1] always controls. They
	// don't cover any of the more tricky dependency cases:
	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = i; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q0, q1, q2);
			// Different target
			TestType op_dt
			    = create_op<TestType>(gs.at(j), q0, q1, q3);
			CHECK_FALSE((op_i.is_dependent(op_dt)
			             || op_dt.is_dependent(op_i)));
			if (i == j) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			if (gs.at(i).axis() == gs.at(j).axis()) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			CHECK((op_i.is_dependent(op_j)
			       || op_j.is_dependent(op_i)));
		}
	}

	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = 8; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q2, q1, q0);
			if (gs.at(i).is_one_qubit()
			    && gs.at(i).axis() == rot_axis::z) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			} else if (gs.at(i).axis() == rot_axis::z
			           && gs.at(j).axis() == rot_axis::z) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			CHECK((op_i.is_dependent(op_j)
			       || op_j.is_dependent(op_i)));
		}
	}

	// Two-qubit and N-qubit gates: same target, different controls
	for (uint32_t i = 8; i < gs.size(); ++i) {
		TestType op_i = create_op<TestType>(gs.at(i), q0, q1, q2);
		for (uint32_t j = i; j < gs.size(); ++j) {
			TestType op_j
			    = create_op<TestType>(gs.at(j), q3, q4, q2);
			if (gs.at(i).axis() == gs.at(j).axis()) {
				CHECK_FALSE((op_i.is_dependent(op_j)
				             || op_j.is_dependent(op_i)));
				continue;
			}
			CHECK((op_i.is_dependent(op_j)
			       || op_j.is_dependent(op_i)));
		}
	}

	// Some edge cases:
	TestType cz_op(GateLib::cz, q0, q1);
	TestType ccx_op(GateLib::ncx, q0, q1, q2);
	TestType ccy_op(GateLib::ncy, q0, q1, q2);
	TestType ccz_op(GateLib::ncz, q0, q1, q2);
	CHECK_FALSE((cz_op.is_dependent(ccx_op) || ccx_op.is_dependent(cz_op)));
	CHECK_FALSE((cz_op.is_dependent(ccy_op) || ccy_op.is_dependent(cz_op)));
	CHECK_FALSE((cz_op.is_dependent(ccz_op) || ccz_op.is_dependent(cz_op)));

	// The swap
	TestType swap_op0(GateLib::swap, q0, q2);
	TestType swap_op1(GateLib::swap, q1, q3);
	TestType swap_op3(GateLib::swap, q4, q5);
	CHECK_FALSE((swap_op0.is_dependent(swap_op1)
	             || swap_op1.is_dependent(swap_op0)));
	for (uint32_t i = 0; i < gs.size(); ++i) {
		TestType op_i0 = create_op<TestType>(gs.at(i), q0, q1, q2);
		CHECK((op_i0.is_dependent(swap_op0)
		       || swap_op0.is_dependent(op_i0)));
		if (gs.at(i).is_one_qubit() || gs.at(i).is_two_qubit()) {
			CHECK_FALSE((op_i0.is_dependent(swap_op1)
			             || swap_op1.is_dependent(op_i0)));
		} else {
			CHECK((op_i0.is_dependent(swap_op1)
			       || swap_op1.is_dependent(op_i0)));
		}
		CHECK_FALSE((op_i0.is_dependent(swap_op3)
		             || swap_op3.is_dependent(op_i0)));
	}
}