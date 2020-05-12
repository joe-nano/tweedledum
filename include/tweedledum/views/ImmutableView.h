/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../ir/Gate.h"
#include "../ir/Node.h"
#include "../ir/Wire.h"

namespace tweedledum {

/*! \brief Deletes all methods that can change the circuit.
 *
 * This view deletes all methods that can change the circuit structure
 * This view is convenient to use as a base class for other views that make some
 * computations based on the structure when being constructed.
 */
template<typename Circuit>
class ImmutableView : public Circuit {
public:
	using op_type = typename Circuit::op_type;
	using node_type = typename Circuit::node_type;
	using dstrg_type = typename Circuit::dstrg_type;

	/*! \brief Default constructor.
	 *
	 * Constructs immutable view on a circuit.
	 */
	ImmutableView(Circuit const circuit) : Circuit(circuit) {}

	wire::Id create_qubit(std::string_view name, wire::Mode const mode
	                                             = wire::Mode::inout)
	    = delete;
	wire::Id create_qubit(wire::Mode const mode = wire::Mode::inout)
	    = delete;
	wire::Id create_cbit(std::string_view name, wire::Mode const mode
	                                            = wire::Mode::inout)
	    = delete;
	wire::Id create_cbit(wire::Mode const mode = wire::Mode::inout)
	    = delete;
	void wire_name(wire::Id const w_id, std::string_view new_name,
	    bool const rename = true)
	    = delete;
	void wire_mode(wire::Id const w_id, wire::Mode const new_mode) = delete;

	template<typename Op>
	node::Id emplace_op(Op&& op) = delete;
	node::Id create_op(Gate const& g, wire::Id const t) = delete;
	node::Id create_op(Gate const& g, wire::Id const w0, wire::Id const w1)
	    = delete;
	node::Id create_op(Gate const& g, wire::Id const c0, wire::Id const c1,
	    wire::Id const t)
	    = delete;
	node::Id create_op(Gate const& g, std::vector<wire::Id> const& controls,
	    std::vector<wire::Id> const& targets)
	    = delete;
};

} // namespace tweedledum
