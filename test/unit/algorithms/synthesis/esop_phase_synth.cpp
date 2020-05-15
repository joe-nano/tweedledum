/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/algorithms/synthesis/esop_phase_synth.h"

#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Netlist.h"
#include "tweedledum/ir/Operation.h"

#include <catch.hpp>
#include <kitty/constructors.hpp>
#include <kitty/cube.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operators.hpp>

using namespace tweedledum;

TEMPLATE_TEST_CASE("Synthesize phase circuits",
    "[esop_phase_synth][template]", CircuitDAG, Netlist)
{
	using op_type = typename TestType::op_type;
	for (uint32_t n = 2u; n <= 10u; ++n) {
		for (uint32_t t = 0u; t <= 50u; ++t) {
			kitty::dynamic_truth_table func(5u);
			kitty::create_random(func);
			const auto network = esop_phase_synth<TestType>(func);
			std::vector<kitty::cube> cubes;
			network.foreach_op([&](op_type const& op) {
				// CHECK(op.is(gate_ids::ncz));
				uint32_t bits = 0u;
				op.foreach_control([&](auto qubit) {
					bits |= 1 << qubit;
				});
				op.foreach_target([&](auto qubit) {
					bits |= 1 << qubit;
				});
				cubes.emplace_back(bits, bits);
			});

			auto func2 = func.construct();
			kitty::create_from_cubes(func2, cubes, true);
			CHECK(
			    func == (kitty::get_bit(func, 0) ? ~func2 : func2));
		}
	}
}
