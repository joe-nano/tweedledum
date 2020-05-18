/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Circuit.h"
#include "../../ir/MappedDAG.h"
#include "../../ir/Wire.h"

#include <algorithm>
#include <limits>
#include <vector>

namespace tweedledum {

#pragma region Implementation detail
namespace detail {
using sum_type = std::vector<uint32_t>;

// Fake pathsums can be employed to verify mappings.  Assuming that the mapping
// do not screw-up adding single qubit gates, we can safely ignore them and
// verify if the set of output path sums of the original circuit mataches the
// set of output path sums of the mapped circuit.
//
// The user need to pass the _initial_ virtual->physical mapping so that the
// path literals can be placed correctly.
template<typename Circuit>
std::vector<sum_type> fake_pathsums(
    Circuit const circuit, std::vector<wire::Id> const& init)
{
	assert(init.size() == circuit.num_qubits());
	using node_type = typename Circuit::node_type;
	using op_type = typename Circuit::op_type;
	constexpr uint32_t qid_max = std::numeric_limits<uint32_t>::max();

	std::vector<uint32_t> wire_to_qid(circuit.num_wires(), qid_max);
	std::vector<sum_type> fake_pathsum;

	circuit.foreach_input([&](node_type const& node) {
		wire::Id w_id = node.op.target();
		if (!w_id.is_qubit()) {
			return;
		};
		wire_to_qid.at(w_id) = fake_pathsum.size();
		fake_pathsum.emplace_back(1u, init.at(fake_pathsum.size()));
	});
	circuit.foreach_op([&](op_type const& op) {
		if (!op.is_two_qubit()) {
			return;
		}
		uint32_t t_qid = wire_to_qid.at(op.target());
		if (op.is(gate_ids::swap)) {
			uint32_t t1_qid = wire_to_qid.at(op.target(1u));
			std::swap(
			    fake_pathsum.at(t_qid), fake_pathsum.at(t1_qid));
			return;
		}
		uint32_t c_qid = wire_to_qid.at(op.control());
		std::vector<uint32_t> result;
		std::set_symmetric_difference(fake_pathsum.at(c_qid).begin(),
		    fake_pathsum.at(c_qid).end(),
		    fake_pathsum.at(t_qid).begin(),
		    fake_pathsum.at(t_qid).end(), std::back_inserter(result));
		fake_pathsum.at(t_qid) = result;
	});
	return fake_pathsum;
}
} // namespace detail
#pragma endregion

/*! \brief Verify if a circuit was correctly mapped (under assumptions, see
 * details).
 *
 * This method uses a trick to verify if a circuit has been correctly mapped. It
 * will consider all two-qubit gates that are not a SWAP to be CX and ignore
 * one-qubit gates, meaning that the circuits will be treated as reversible
 * circuits composed of CX and SWAP gates.  The algorithm basically checks if
 * the outputs are equal up to a permutation.
 *
 * NOTE: as it ignores one-qubit gates, this verification assumes that those
 * gates were correctly mapped!
 *
 * \param[in] original the unmapped circuit.
 * \param[in] mapped the mapped version of the original circuit.
 * \returns true if the circuit is correctly mapped under the assumptions.
 */
inline bool map_verify(Circuit const original, MappedDAG const& mapped)
{
	std::vector<wire::Id> init_original(
	    original.num_qubits(), wire::invalid_id);
	for (uint32_t i = 0u; i < init_original.size(); ++i) {
		init_original.at(i) = wire::make_qubit(i);
	}
	auto original_pathsums = detail::fake_pathsums(original, init_original);
	auto mapped_pathsums
	    = detail::fake_pathsums(mapped, mapped.init_phy_to_v());
	return std::is_permutation(original_pathsums.begin(),
	    original_pathsums.end(), mapped_pathsums.begin());
}

} // namespace tweedledum
