/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/Gate.h"

#include "tweedledum/support/Angle.h"

#include <catch.hpp>
#include <vector>

TEST_CASE("Check correcteness meta gates", "[gate][meta]")
{
	using namespace tweedledum;
	SECTION("Input gate")
	{
		CHECK(GateLib::input.id() == gate_ids::input);
		CHECK(GateLib::input.is(gate_ids::input));
		CHECK_FALSE(GateLib::input.is_one_qubit());
		CHECK(GateLib::input.is_meta());
		CHECK_FALSE(GateLib::input.is_two_qubit());
		CHECK_FALSE(GateLib::input.is_r1());
		CHECK_FALSE(GateLib::input.is_measurement());
	}
}

TEST_CASE("Check correcteness non-parameterasible gates", "[gate][non_param]")
{
	using namespace tweedledum;
	SECTION("Identity gate")
	{
		CHECK(GateLib::i.id() == gate_ids::i);
		CHECK(GateLib::i.is(gate_ids::i));
		CHECK(GateLib::i.axis() == rot_axis::na);
		CHECK(GateLib::i.is_one_qubit());
		CHECK_FALSE(GateLib::i.is_meta());
		CHECK_FALSE(GateLib::i.is_two_qubit());
		CHECK_FALSE(GateLib::i.is_r1());
		CHECK_FALSE(GateLib::i.is_measurement());
	}
	SECTION("Hadamard gate")
	{
		CHECK(GateLib::h.id() == gate_ids::h);
		CHECK(GateLib::h.is(gate_ids::h));
		CHECK(GateLib::h.axis() == rot_axis::xy);
		CHECK(GateLib::h.is_one_qubit());
		CHECK_FALSE(GateLib::h.is_meta());
		CHECK_FALSE(GateLib::h.is_two_qubit());
		CHECK_FALSE(GateLib::h.is_r1());
		CHECK_FALSE(GateLib::h.is_measurement());
	}
	SECTION("X (NOT) gate")
	{
		CHECK(GateLib::x.id() == gate_ids::x);
		CHECK(GateLib::x.is(gate_ids::x));
		CHECK(GateLib::x.axis() == rot_axis::x);
		CHECK(GateLib::x.is_one_qubit());
		CHECK_FALSE(GateLib::x.is_meta());
		CHECK_FALSE(GateLib::x.is_two_qubit());
		CHECK_FALSE(GateLib::x.is_r1());
		CHECK_FALSE(GateLib::x.is_measurement());
		CHECK(GateLib::x.rotation_angle() == sym_angle::pi);
	}
	SECTION("Y gate")
	{
		CHECK(GateLib::y.id() == gate_ids::y);
		CHECK(GateLib::y.is(gate_ids::y));
		CHECK(GateLib::y.axis() == rot_axis::y);
		CHECK(GateLib::y.is_one_qubit());
		CHECK_FALSE(GateLib::y.is_meta());
		CHECK_FALSE(GateLib::y.is_two_qubit());
		CHECK_FALSE(GateLib::y.is_r1());
		CHECK_FALSE(GateLib::y.is_measurement());
		CHECK(GateLib::y.rotation_angle() == sym_angle::pi);
	}
	SECTION("Z gate")
	{
		CHECK(GateLib::z.id() == gate_ids::z);
		CHECK(GateLib::z.is(gate_ids::z));
		CHECK(GateLib::z.axis() == rot_axis::z);
		CHECK(GateLib::z.is_one_qubit());
		CHECK_FALSE(GateLib::z.is_meta());
		CHECK_FALSE(GateLib::z.is_two_qubit());
		CHECK(GateLib::z.is_r1());
		CHECK_FALSE(GateLib::z.is_measurement());
		CHECK(GateLib::z.rotation_angle() == sym_angle::pi);
	}
	SECTION("Phase (S) gate")
	{
		CHECK(GateLib::s.id() == gate_ids::s);
		CHECK(GateLib::s.is(gate_ids::s));
		CHECK(GateLib::s.axis() == rot_axis::z);
		CHECK(GateLib::s.is_one_qubit());
		CHECK_FALSE(GateLib::s.is_meta());
		CHECK_FALSE(GateLib::s.is_two_qubit());
		CHECK(GateLib::s.is_r1());
		CHECK_FALSE(GateLib::s.is_measurement());
		CHECK(GateLib::s.rotation_angle() == sym_angle::pi_half);
	}
	SECTION("Phase (S) adjoint (daggert) gate")
	{
		CHECK(GateLib::sdg.id() == gate_ids::sdg);
		CHECK(GateLib::sdg.is(gate_ids::sdg));
		CHECK(GateLib::sdg.axis() == rot_axis::z);
		CHECK(GateLib::sdg.is_one_qubit());
		CHECK_FALSE(GateLib::sdg.is_meta());
		CHECK_FALSE(GateLib::sdg.is_two_qubit());
		CHECK(GateLib::sdg.is_r1());
		CHECK_FALSE(GateLib::sdg.is_measurement());
		CHECK(GateLib::sdg.rotation_angle() == -sym_angle::pi_half);
	}
	SECTION("T gate")
	{
		CHECK(GateLib::t.id() == gate_ids::t);
		CHECK(GateLib::t.is(gate_ids::t));
		CHECK(GateLib::t.axis() == rot_axis::z);
		CHECK(GateLib::t.is_one_qubit());
		CHECK_FALSE(GateLib::t.is_meta());
		CHECK_FALSE(GateLib::t.is_two_qubit());
		CHECK(GateLib::t.is_r1());
		CHECK_FALSE(GateLib::t.is_measurement());
		CHECK(GateLib::t.rotation_angle() == sym_angle::pi_quarter);
	}
	SECTION("T adjoint (daggert) gate")
	{
		CHECK(GateLib::tdg.id() == gate_ids::tdg);
		CHECK(GateLib::tdg.is(gate_ids::tdg));
		CHECK(GateLib::tdg.axis() == rot_axis::z);
		CHECK(GateLib::tdg.is_one_qubit());
		CHECK_FALSE(GateLib::tdg.is_meta());
		CHECK_FALSE(GateLib::tdg.is_two_qubit());
		CHECK(GateLib::tdg.is_r1());
		CHECK_FALSE(GateLib::tdg.is_measurement());
		CHECK(GateLib::tdg.rotation_angle() == -sym_angle::pi_quarter);
	}
	SECTION("CX (CNOT) gate")
	{
		CHECK(GateLib::cx.id() == gate_ids::cx);
		CHECK(GateLib::cx.is(gate_ids::cx));
		CHECK(GateLib::cx.axis() == rot_axis::x);
		CHECK_FALSE(GateLib::cx.is_one_qubit());
		CHECK_FALSE(GateLib::cx.is_meta());
		CHECK(GateLib::cx.is_two_qubit());
		CHECK_FALSE(GateLib::cx.is_r1());
		CHECK_FALSE(GateLib::cx.is_measurement());
		CHECK(GateLib::cx.rotation_angle() == sym_angle::pi);
	}
	SECTION("CY gate")
	{
		CHECK(GateLib::cy.id() == gate_ids::cy);
		CHECK(GateLib::cy.is(gate_ids::cy));
		CHECK(GateLib::cy.axis() == rot_axis::y);
		CHECK_FALSE(GateLib::cy.is_one_qubit());
		CHECK_FALSE(GateLib::cy.is_meta());
		CHECK(GateLib::cy.is_two_qubit());
		CHECK_FALSE(GateLib::cy.is_r1());
		CHECK_FALSE(GateLib::cy.is_measurement());
		CHECK(GateLib::cy.rotation_angle() == sym_angle::pi);
	}
	SECTION("CZ gate")
	{
		CHECK(GateLib::cz.id() == gate_ids::cz);
		CHECK(GateLib::cz.is(gate_ids::cz));
		CHECK(GateLib::cz.axis() == rot_axis::z);
		CHECK_FALSE(GateLib::cz.is_one_qubit());
		CHECK_FALSE(GateLib::cz.is_meta());
		CHECK(GateLib::cz.is_two_qubit());
		CHECK(GateLib::cz.is_r1());
		CHECK_FALSE(GateLib::cz.is_measurement());
		CHECK(GateLib::cz.rotation_angle() == sym_angle::pi);
	}
	SECTION("NCX (Toffoli) gate")
	{
		CHECK(GateLib::ncx.id() == gate_ids::ncx);
		CHECK(GateLib::ncx.is(gate_ids::ncx));
		CHECK(GateLib::ncx.axis() == rot_axis::x);
		CHECK_FALSE(GateLib::ncx.is_one_qubit());
		CHECK_FALSE(GateLib::ncx.is_meta());
		CHECK_FALSE(GateLib::ncx.is_two_qubit());
		CHECK_FALSE(GateLib::ncx.is_r1());
		CHECK_FALSE(GateLib::ncx.is_measurement());
		CHECK(GateLib::ncx.rotation_angle() == sym_angle::pi);
	}
	SECTION("NCY gate")
	{
		CHECK(GateLib::ncy.id() == gate_ids::ncy);
		CHECK(GateLib::ncy.is(gate_ids::ncy));
		CHECK(GateLib::ncy.axis() == rot_axis::y);
		CHECK_FALSE(GateLib::ncy.is_one_qubit());
		CHECK_FALSE(GateLib::ncy.is_meta());
		CHECK_FALSE(GateLib::ncy.is_two_qubit());
		CHECK_FALSE(GateLib::ncy.is_r1());
		CHECK_FALSE(GateLib::ncy.is_measurement());
		CHECK(GateLib::ncy.rotation_angle() == sym_angle::pi);
	}
	SECTION("NCZ gate")
	{
		CHECK(GateLib::ncz.id() == gate_ids::ncz);
		CHECK(GateLib::ncz.is(gate_ids::ncz));
		CHECK(GateLib::ncz.axis() == rot_axis::z);
		CHECK_FALSE(GateLib::ncz.is_one_qubit());
		CHECK_FALSE(GateLib::ncz.is_meta());
		CHECK_FALSE(GateLib::ncz.is_two_qubit());
		CHECK(GateLib::ncz.is_r1());
		CHECK_FALSE(GateLib::ncz.is_measurement());
		CHECK(GateLib::ncz.rotation_angle() == sym_angle::pi);
	}
	SECTION("SWAP gate")
	{
		CHECK(GateLib::swap.id() == gate_ids::swap);
		CHECK(GateLib::swap.is(gate_ids::swap));
		CHECK(GateLib::swap.axis() == rot_axis::na);
		CHECK_FALSE(GateLib::swap.is_one_qubit());
		CHECK_FALSE(GateLib::swap.is_meta());
		CHECK(GateLib::swap.is_two_qubit());
		CHECK_FALSE(GateLib::swap.is_r1());
		CHECK_FALSE(GateLib::swap.is_measurement());
	}
}

TEST_CASE("Check correcteness parameterasible gates", "[gate][param]")
{
	using namespace tweedledum;
	std::vector<Angle> commmon_angles = {sym_angle::zero, sym_angle::pi,
	    sym_angle::pi_half, sym_angle::pi_quarter};

	SECTION("R1")
	{
		for (Angle const& a : commmon_angles) {
			Gate r1 = GateLib::r1(a);
			CHECK(r1.id() == gate_ids::r1);
			CHECK(r1.is(gate_ids::r1));
			CHECK(r1.axis() == rot_axis::z);
			CHECK(r1.is_one_qubit());
			CHECK_FALSE(r1.is_meta());
			CHECK_FALSE(r1.is_two_qubit());
			CHECK(r1.is_r1());
			CHECK_FALSE(r1.is_measurement());
			CHECK(r1.rotation_angle() == a);
		}
	}
	SECTION("Rx")
	{
		for (Angle const& a : commmon_angles) {
			Gate rx = GateLib::rx(a);
			CHECK(rx.id() == gate_ids::rx);
			CHECK(rx.is(gate_ids::rx));
			CHECK(rx.axis() == rot_axis::x);
			CHECK(rx.is_one_qubit());
			CHECK_FALSE(rx.is_meta());
			CHECK_FALSE(rx.is_two_qubit());
			CHECK_FALSE(rx.is_r1());
			CHECK_FALSE(rx.is_measurement());
			CHECK(rx.rotation_angle() == a);
		}
	}
	SECTION("Ry")
	{
		for (Angle const& a : commmon_angles) {
			Gate ry = GateLib::ry(a);
			CHECK(ry.id() == gate_ids::ry);
			CHECK(ry.is(gate_ids::ry));
			CHECK(ry.axis() == rot_axis::y);
			CHECK(ry.is_one_qubit());
			CHECK_FALSE(ry.is_meta());
			CHECK_FALSE(ry.is_two_qubit());
			CHECK_FALSE(ry.is_r1());
			CHECK_FALSE(ry.is_measurement());
			CHECK(ry.rotation_angle() == a);
		}
	}
	SECTION("Rz")
	{
		for (Angle const& a : commmon_angles) {
			Gate rz = GateLib::rz(a);
			CHECK(rz.id() == gate_ids::rz);
			CHECK(rz.is(gate_ids::rz));
			CHECK(rz.axis() == rot_axis::z);
			CHECK(rz.is_one_qubit());
			CHECK_FALSE(rz.is_meta());
			CHECK_FALSE(rz.is_two_qubit());
			CHECK_FALSE(rz.is_r1());
			CHECK_FALSE(rz.is_measurement());
			CHECK(rz.rotation_angle() == a);
		}
	}
	SECTION("CRx")
	{
		for (Angle const& a : commmon_angles) {
			Gate crx = GateLib::crx(a);
			CHECK(crx.id() == gate_ids::crx);
			CHECK(crx.is(gate_ids::crx));
			CHECK(crx.axis() == rot_axis::x);
			CHECK_FALSE(crx.is_one_qubit());
			CHECK_FALSE(crx.is_meta());
			CHECK(crx.is_two_qubit());
			CHECK_FALSE(crx.is_r1());
			CHECK_FALSE(crx.is_measurement());
			CHECK(crx.rotation_angle() == a);
		}
	}
	SECTION("CRy")
	{
		for (Angle const& a : commmon_angles) {
			Gate cry = GateLib::cry(a);
			CHECK(cry.id() == gate_ids::cry);
			CHECK(cry.is(gate_ids::cry));
			CHECK(cry.axis() == rot_axis::y);
			CHECK_FALSE(cry.is_one_qubit());
			CHECK_FALSE(cry.is_meta());
			CHECK(cry.is_two_qubit());
			CHECK_FALSE(cry.is_r1());
			CHECK_FALSE(cry.is_measurement());
			CHECK(cry.rotation_angle() == a);
		}
	}
	SECTION("CRz")
	{
		for (Angle const& a : commmon_angles) {
			Gate crz = GateLib::crz(a);
			CHECK(crz.id() == gate_ids::crz);
			CHECK(crz.is(gate_ids::crz));
			CHECK(crz.axis() == rot_axis::z);
			CHECK_FALSE(crz.is_one_qubit());
			CHECK_FALSE(crz.is_meta());
			CHECK(crz.is_two_qubit());
			CHECK_FALSE(crz.is_r1());
			CHECK_FALSE(crz.is_measurement());
			CHECK(crz.rotation_angle() == a);
		}
	}
	SECTION("NCRx")
	{
		for (Angle const& a : commmon_angles) {
			Gate ncrx = GateLib::ncrx(a);
			CHECK(ncrx.id() == gate_ids::ncrx);
			CHECK(ncrx.is(gate_ids::ncrx));
			CHECK(ncrx.axis() == rot_axis::x);
			CHECK_FALSE(ncrx.is_one_qubit());
			CHECK_FALSE(ncrx.is_meta());
			CHECK_FALSE(ncrx.is_two_qubit());
			CHECK_FALSE(ncrx.is_r1());
			CHECK_FALSE(ncrx.is_measurement());
			CHECK(ncrx.rotation_angle() == a);
		}
	}
	SECTION("NCRy")
	{
		for (Angle const& a : commmon_angles) {
			Gate ncry = GateLib::ncry(a);
			CHECK(ncry.id() == gate_ids::ncry);
			CHECK(ncry.is(gate_ids::ncry));
			CHECK(ncry.axis() == rot_axis::y);
			CHECK_FALSE(ncry.is_one_qubit());
			CHECK_FALSE(ncry.is_meta());
			CHECK_FALSE(ncry.is_two_qubit());
			CHECK_FALSE(ncry.is_r1());
			CHECK_FALSE(ncry.is_measurement());
			CHECK(ncry.rotation_angle() == a);
		}
	}
	SECTION("NCRz")
	{
		for (Angle const& a : commmon_angles) {
			Gate ncrz = GateLib::ncrz(a);
			CHECK(ncrz.id() == gate_ids::ncrz);
			CHECK(ncrz.is(gate_ids::ncrz));
			CHECK(ncrz.axis() == rot_axis::z);
			CHECK_FALSE(ncrz.is_one_qubit());
			CHECK_FALSE(ncrz.is_meta());
			CHECK_FALSE(ncrz.is_two_qubit());
			CHECK_FALSE(ncrz.is_r1());
			CHECK_FALSE(ncrz.is_measurement());
			CHECK(ncrz.rotation_angle() == a);
		}
	}
}

TEST_CASE("Check gates adjointness", "[gate]")
{
	using namespace tweedledum;
	std::vector<Gate> self_adjoint = {GateLib::i, GateLib::h, GateLib::x,
	    GateLib::y, GateLib::z, GateLib::cx, GateLib::cy, GateLib::cz,
	    GateLib::ncx, GateLib::ncy, GateLib::ncz, GateLib::swap};
	// nsa = not self-adjoint
	std::vector<Gate> nsa = {GateLib::s, GateLib::t};
	std::vector<Gate> nsa_dagger = {GateLib::sdg, GateLib::tdg};

	// add parameterisable gates
	std::vector<Angle> commmon_angles = {sym_angle::zero, sym_angle::pi,
	    sym_angle::pi_half, sym_angle::pi_quarter};
	for (Angle const& a : commmon_angles) {
		nsa.emplace_back(GateLib::r1(a));
		nsa_dagger.emplace_back(GateLib::r1(-a));
		nsa.emplace_back(GateLib::rx(a));
		nsa_dagger.emplace_back(GateLib::rx(-a));
		nsa.emplace_back(GateLib::ry(a));
		nsa_dagger.emplace_back(GateLib::ry(-a));
		nsa.emplace_back(GateLib::rz(a));
		nsa_dagger.emplace_back(GateLib::rz(-a));

		nsa.emplace_back(GateLib::crx(a));
		nsa_dagger.emplace_back(GateLib::crx(-a));
		nsa.emplace_back(GateLib::cry(a));
		nsa_dagger.emplace_back(GateLib::cry(-a));
		nsa.emplace_back(GateLib::crz(a));
		nsa_dagger.emplace_back(GateLib::crz(-a));

		nsa.emplace_back(GateLib::ncrx(a));
		nsa_dagger.emplace_back(GateLib::ncrx(-a));
		nsa.emplace_back(GateLib::ncry(a));
		nsa_dagger.emplace_back(GateLib::ncry(-a));
		nsa.emplace_back(GateLib::ncrz(a));
		nsa_dagger.emplace_back(GateLib::ncrz(-a));
	}
	REQUIRE(nsa.size() == nsa_dagger.size());

	for (uint32_t i = 0; i < self_adjoint.size(); ++i) {
		for (uint32_t j = 0; j < self_adjoint.size(); ++j) {
			if (i == j) {
				CHECK(self_adjoint[i].is_adjoint(
				    self_adjoint[j]));
			} else {
				CHECK_FALSE(self_adjoint[i].is_adjoint(
				    self_adjoint[j]));
			}
		}
		for (uint32_t j = 0; j < nsa.size(); ++j) {
			CHECK_FALSE(self_adjoint[i].is_adjoint(nsa[j]));
			CHECK_FALSE(nsa[j].is_adjoint(self_adjoint[i]));
			CHECK_FALSE(self_adjoint[i].is_adjoint(nsa_dagger[j]));
			CHECK_FALSE(nsa_dagger[j].is_adjoint(self_adjoint[i]));
		}
	}
	for (uint32_t i = 0; i < nsa.size(); ++i) {
		for (uint32_t j = 0; j < nsa_dagger.size(); ++j) {
			if (i == j) {
				CHECK(nsa[i].is_adjoint(nsa_dagger[j]));
				CHECK(nsa_dagger[j].is_adjoint(nsa[i]));
			} else {
				CHECK_FALSE(nsa[i].is_adjoint(nsa_dagger[j]));
				CHECK_FALSE(nsa_dagger[j].is_adjoint(nsa[i]));
			}
		}
	}
}