/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../support/Angle.h"
#include "Gate.h"
#include "Wire.h"

#include <algorithm>
#include <vector>

namespace tweedledum {

class Operation {
	void init_one_wire(wire::Id const t)
	{
		assert(t.is_qubit() && t != wire::invalid_id
		       && !t.is_complemented());
		assert(is_one_qubit());
		wires_.at(0) = t;
	}

	// When dealing with controlled gates (e.g.CX, CZ) id0 is the control
	// and id1 the target
	// *) In case of SWAP they are both targets
	// *) In case of MEASUREMENT they are both targets and id1 _must_ be the
	// cbit
	void init_two_wire(wire::Id const w0, wire::Id const w1)
	{
		assert(w0.is_qubit() && w0 != wire::invalid_id);
		assert(w1 != wire::invalid_id);
		assert(w0 != w1 && "The wires must be different");
		// assert(is_two_qubit() || is_measurement());
		// In a measurement gate the second I/O must be a cbit:
		assert(
		    (is_measurement() && !w1.is_qubit()) || !is_measurement());

		wires_.at(0) = w0;
		wires_.at(1) = w1;
		if (is_measurement()) {
			assert(!w0.is_complemented());
			assert(!w1.is_qubit());
			num_controls_ = 0;
			num_targets_ = 2;
			return;
		}

		// If we reach this point, then w1 is guaranteed to be a target
		// qubit, so it cannot be complemented!
		assert(w1.is_qubit() && !w1.is_complemented());
		if (is(gate_ids::swap)) {
			assert(!w0.is_complemented());
			num_controls_ = 0;
			num_targets_ = 2;
			// Normalization step to make SWAP(0, 1) == SWAP(1, 0);
			if (w1.uid() < w0.uid()) {
				std::swap(wires_.at(0), wires_.at(1));
			}
		}
	}

	void init_three_wire(
	    wire::Id const c0, wire::Id const c1, wire::Id const t)
	{
		assert(c0.is_qubit() && c0 != wire::invalid_id);
		assert(c1.is_qubit() && c1 != wire::invalid_id);
		assert(t.is_qubit() && t != wire::invalid_id
		       && !t.is_complemented());
		assert(c0 != c1 && c0 != t && c1 != t);
		assert(!is_meta() && !is_measurement());
		assert(!is_one_qubit() && !is_two_qubit());

		wires_.at(0) = c0;
		wires_.at(1) = c1;
		wires_.at(2) = t;
		// Normalization step to make CCX(0, 1, 2) == CCX(1, 0, 2);
		if (c1.uid() < c0.uid()) {
			std::swap(wires_.at(0), wires_.at(1));
		}
	}

public:
	Operation(Gate const& g, wire::Id const t)
	    : gate_(g), num_controls_(0), num_targets_(1), wires_(1u, t.wire())
	{
		assert(t != wire::invalid_id && !t.is_complemented());
		assert(is_meta() || (is_one_qubit() && t.is_qubit()));
	}

	Operation(Gate const& g, wire::Id const w0, wire::Id const w1)
	    : gate_(g), num_controls_(1u), num_targets_(1u),
	      wires_(2u, wire::invalid_id)
	{
		init_two_wire(w0, w1);
	}

	Operation(Gate const& g, wire::Id const c0, wire::Id const c1,
	    wire::Id const t)
	    : gate_(g), num_controls_(2u), num_targets_(1u),
	      wires_(3u, wire::invalid_id)
	{
		init_three_wire(c0, c1, t);
	}

	Operation(Gate const& g, std::vector<wire::Id> const& cs,
	    std::vector<wire::Id> const& ts)
	    : gate_(g), num_controls_(cs.size()), num_targets_(ts.size()),
	      wires_(num_controls_ + num_targets_, wire::invalid_id)
	{
		assert(ts.size() >= 1u
		       && "The gate must have at least one target");
		assert(
		    ts.size() <= 2u && "The gate must have at most two target");
		switch (cs.size()) {
		case 0u:
			if (ts.size() == 1) {
				init_one_wire(ts.at(0));
			} else if (ts.size() == 2) {
				init_two_wire(ts.at(0), ts.at(1));
			} else {
				std::abort();
			}
			return;

		case 1u:
			init_two_wire(cs.at(0), ts.at(0));
			return;

		case 2u:
			init_three_wire(cs.at(0), cs.at(1), ts.at(0));
			return;

		default:
			std::partial_sort_copy(cs.begin(), cs.end(),
			    wires_.begin(), wires_.end() - 1);
			wires_.at(num_controls()) = ts.at(0);
			break;
		}
	}

	gate_ids id() const
	{
		return gate_.id();
	}

	bool is(gate_ids gid) const
	{
		return id() == gid;
	}

	bool is_adjoint(Gate const& other) const
	{
		return gate_.is_adjoint(other);
	}

	bool is_meta() const
	{
		return gate_.is_meta();
	}

	bool is_one_qubit() const
	{
		return gate_.is_one_qubit();
	}

	bool is_two_qubit() const
	{
		return gate_.is_two_qubit();
	}

	bool is_r1() const
	{
		return gate_.is_r1();
	}

	bool is_measurement() const
	{
		return gate_.is_measurement();
	}

	rot_axis axis() const
	{
		return gate_.axis();
	}

	Angle rotation_angle() const
	{
		return gate_.rotation_angle();
	}

	Angle theta() const
	{
		return gate_.theta();
	}

	Angle phi() const
	{
		return gate_.phi();
	}

	Angle lambda() const
	{
		return gate_.lambda();
	}

	Gate const& gate() const
	{
		return gate_;
	}

	uint32_t num_wires() const
	{
		return wires_.size();
	}

	uint32_t num_controls() const
	{
		return num_controls_;
	}

	uint32_t num_targets() const
	{
		return num_targets_;
	}

	wire::Id control(uint32_t const i = 0u) const
	{
		assert(i < num_controls());
		return wires_.at(i);
	}

	wire::Id target(uint32_t const i = 0u) const
	{
		assert(i < num_targets());
		return wires_.at(num_controls() + i);
	}

	uint32_t position(wire::Id const w_id) const
	{
		assert(w_id != wire::invalid_id);
		for (uint32_t i = 0u; i < wires_.size(); ++i) {
			if (wires_.at(i).uid() == w_id.uid()) {
				return i;
			}
		}
		std::abort();
	}

	wire::Id wire(uint32_t const position) const
	{
		assert(wires_.at(position) != wire::invalid_id);
		return wires_.at(position);
	}

	bool is_adjoint(Operation const& other) const
	{
		assert(!is_meta() && !other.is_meta());
		assert(!is_measurement() && !other.is_measurement());
		if (gate_.is_adjoint(other.gate_) == false) {
			return false;
		}
		if (num_controls() != other.num_controls()) {
			return false;
		}
		if (num_targets() != other.num_targets()) {
			return false;
		}
		return wires_ == other.wires_;
	}

	bool is_dependent(Operation const& other) const
	{
		// First, we deal with the easy cases:
		// If one of the gates is a meta gate, then we act
		// conservatively and return true.
		if (is_meta() || other.is_meta()) {
			return true;
		}
		// When the gates are equal, they are _not_ dependent.
		if (*this == other) {
			return false;
		}
		// If one of the gates is an identity gate, they are _not_
		// dependent.
		if (is(gate_ids::i) || other.is(gate_ids::i)) {
			return false;
		}
		// If one of the gates is a swap gate, they I need to check if
		// the set qubits intersect. If they do, then the gates are
		// dependent.
		if (is(gate_ids::swap) || other.is(gate_ids::swap)) {
			for (uint32_t i = 0; i < wires_.size(); ++i) {
				for (uint32_t j = 0; j < other.wires_.size();
				     ++j) {
					if (wires_.at(i) == other.wires_.at(0))
					{
						return true;
					}
				}
			}
			return false;
		}
		// If targets are the same, then I just need to worry about the
		// axis
		if (target() == other.target()) {
			return axis() != other.axis();
		}

		// Well, targets are not the same, and both are an one-qubit
		// operation:
		if (is_one_qubit() && other.is_one_qubit()) {
			return false;
		}

		if (axis() == rot_axis::z) {
			if (other.axis() == rot_axis::z) {
				return false;
			}
			bool dependent = false;
			foreach_control([&](wire::Id this_control) {
				other.foreach_target([&](wire::Id other_target) {
					dependent |= (this_control.uid()
					              == other_target);
				});
			});
			return dependent;
		}
		if (other.axis() == rot_axis::z) {
			bool dependent = false;
			other.foreach_control([&](wire::Id other_control) {
				dependent |= (other_control.uid() == target());
			});
			return dependent;
		}

		bool dependent = false;
		foreach_control([&](wire::Id this_control) {
			dependent |= (this_control.uid() == other.target());
		});
		other.foreach_control([&](wire::Id other_control) {
			dependent |= (target() == other_control.uid());
		});
		return dependent;
	}

	template<typename Fn>
	void foreach_control(Fn&& fn) const
	{
		for (uint32_t i = 0; i < num_controls(); ++i) {
			fn(wires_.at(i));
		}
	}

	template<typename Fn>
	void foreach_target(Fn&& fn) const
	{
		for (uint32_t i = num_controls(); i < wires_.size(); ++i) {
			fn(wires_.at(i));
		}
	}

	bool operator==(Operation const& other) const
	{
		if (gate_ != other.gate_) {
			return false;
		}
		if (num_controls() != other.num_controls()) {
			return false;
		}
		if (num_targets() != other.num_targets()) {
			return false;
		}
		return wires_ == other.wires_;
	}

private:
	Gate gate_;
	// TODO: eventually this will have to be a pointer to either a gate or
	//       a gate defined as a subscircuit, unitary matrix, etc...
	// I think the concept of Gate will need to be more abstract
	uint32_t num_controls_;
	uint32_t num_targets_;
	// TODO: Small Vector optimization, symbolic parameters
	// Maybe it should be const
	std::vector<wire::Id> wires_;
};

} // namespace tweedledum
