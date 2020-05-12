/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>

TEST_CASE("Wire id constructors", "[wire]")
{
	using namespace tweedledum;
	SECTION("Simple qubit id")
	{
		wire::Id id = wire::make_qubit(0);
		CHECK(id.is_qubit());
		CHECK_FALSE(id.is_complemented());
	}
	SECTION("Simple classical bit id")
	{
		wire::Id id = wire::make_cbit(0);
		CHECK_FALSE(id.is_qubit());
		CHECK_FALSE(id.is_complemented());
	}
	SECTION("Complementing ids")
	{
		wire::Id id = wire::make_qubit(10);
		wire::Id cmpl_id = !id;
		CHECK_FALSE(id == cmpl_id);
		CHECK(id == !cmpl_id);
		CHECK(!id == cmpl_id);
		CHECK(cmpl_id.uid() == 10u);
		CHECK(cmpl_id.is_qubit());
		CHECK(cmpl_id.is_complemented());
	}
}

TEST_CASE("Wire storage", "[wire]")
{
	using namespace tweedledum;

	wire::Storage storage;
	SECTION("Using const literal strings")
	{
		wire::Id qubit = storage.create_qubit("q0", wire::Mode::inout);
		wire::Id qubit_found = storage.wire("q0");
		CHECK(qubit == qubit_found);

		wire::Id cbit = storage.create_cbit("c0", wire::Mode::inout);
		wire::Id cbit_found = storage.wire("c0");
		CHECK(cbit == cbit_found);
	}
	SECTION("Using strings")
	{
		std::string qubit_name = "__dum_q0";
		std::string cbit_name = "c0";

		wire::Id qubit
		    = storage.create_qubit(qubit_name, wire::Mode::inout);
		wire::Id cbit
		    = storage.create_cbit(cbit_name, wire::Mode::inout);
		wire::Id qubit_found = storage.wire(qubit_name);
		wire::Id cbit_found = storage.wire(cbit_name);
		CHECK(qubit == qubit_found);
		CHECK(cbit == cbit_found);
	}
}
