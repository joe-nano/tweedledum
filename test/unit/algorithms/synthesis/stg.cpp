/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/stg.h"

#include "tweedledum/algorithms/simulation/simulate_classically.h"
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Module.h"
#include "tweedledum/ir/Operation.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <sstream>

namespace tweedledum::detail {

inline auto circuit_and_map(Module& module, uint32_t num_qubits)
{
	std::vector<wire::Id> qubits;
	for (uint32_t i = 0u; i < num_qubits; ++i) {
		qubits.emplace_back(module.circuit_.create_qubit());
	}
	return qubits;
}

} // namespace tweedledum::detail

using namespace tweedledum;
TEST_CASE("Single-target gate synthesis", "[stg]")
{
	Module module;
	auto map = detail::circuit_and_map(module, 6u);
	CircuitDAG& circuit = module.circuit_;
	SECTION("Synthesize using stg_from_pkrm")
	{
		kitty::dynamic_truth_table tt(5);
		kitty::create_from_hex_string(tt, "DA657041");
		stg_from_pkrm()(circuit, map, tt);
		for (auto i = 0ull; i < tt.num_bits(); ++i) {
			auto expected_output = i;
			if (kitty::get_bit(tt, i)) {
				expected_output |= (1ull << tt.num_vars());
			}
			CHECK(simulate_classically(circuit, i)
			      == expected_output);
		}
	}
	SECTION("Synthesize using stg_from_pprm")
	{
		kitty::dynamic_truth_table tt(5);
		kitty::create_from_hex_string(tt, "DA657041");
		stg_from_pprm()(circuit, map, tt);
		for (auto i = 0ull; i < tt.num_bits(); ++i) {
			auto expected_output = i;
			if (kitty::get_bit(tt, i)) {
				expected_output |= (1ull << tt.num_vars());
			}
			CHECK(simulate_classically(circuit, i)
			      == expected_output);
		}
	}
	SECTION("Synthesize stg_from_spectrum")
	{
		kitty::dynamic_truth_table tt(5);
		kitty::create_from_hex_string(tt, "DA657041");
		stg_from_spectrum()(circuit, map, tt);
		CHECK(circuit.num_operations() == 108u);
		CHECK(circuit.num_qubits() == 6u);
	}
}