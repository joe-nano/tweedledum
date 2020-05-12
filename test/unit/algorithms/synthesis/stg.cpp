/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/stg.h"

#include "tweedledum/algorithms/simulation/simulate_classically.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Wire.h"
#include "tweedledum/ir/operations/wn32_op.h"

#include <catch.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <sstream>

namespace tweedledum::detail {

template<class Network>
inline auto circuit_and_map(uint32_t num_qubits)
{
	Network network;
	std::vector<wire::Id> qubits;
	for (uint32_t i = 0u; i < num_qubits; ++i) {
		qubits.emplace_back(network.create_qubit());
	}
	return std::make_pair(network, qubits);
}

} // namespace tweedledum::detail

using namespace tweedledum;
TEMPLATE_PRODUCT_TEST_CASE("Single-target gate synthesis", "[stg][template]",
    (CircuitDAG, Netlist), (wn32_op))
{
	SECTION("Synthesize using stg_from_pkrm")
	{
		kitty::dynamic_truth_table tt(5);
		kitty::create_from_hex_string(tt, "DA657041");
		auto [network, map] = detail::circuit_and_map<TestType>(6u);
		stg_from_pkrm()(network, map, tt);
		for (auto i = 0ull; i < tt.num_bits(); ++i) {
			auto expected_output = i;
			if (kitty::get_bit(tt, i)) {
				expected_output |= (1ull << tt.num_vars());
			}
			CHECK(simulate_classically(network, i)
			      == expected_output);
		}
	}
	SECTION("Synthesize using stg_from_pprm")
	{
		kitty::dynamic_truth_table tt(5);
		kitty::create_from_hex_string(tt, "DA657041");
		auto [network, map] = detail::circuit_and_map<TestType>(6u);
		stg_from_pprm()(network, map, tt);
		for (auto i = 0ull; i < tt.num_bits(); ++i) {
			auto expected_output = i;
			if (kitty::get_bit(tt, i)) {
				expected_output |= (1ull << tt.num_vars());
			}
			CHECK(simulate_classically(network, i)
			      == expected_output);
		}
	}
	SECTION("Synthesize stg_from_spectrum")
	{
		kitty::dynamic_truth_table tt(5);
		kitty::create_from_hex_string(tt, "DA657041");
		auto [network, map] = detail::circuit_and_map<TestType>(6u);
		stg_from_spectrum()(network, map, tt);
		CHECK(network.num_operations() == 108u);
		CHECK(network.num_qubits() == 6u);
	}
}