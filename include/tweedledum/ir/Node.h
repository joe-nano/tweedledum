/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <vector>

namespace tweedledum::node {

// NOTE: When casted to an `uint32_t` this must return an index to the node
class Id {
public:
	constexpr Id() : uid_(std::numeric_limits<uint32_t>::max()) {}

	constexpr explicit Id(uint32_t const uid) : uid_(uid) {}

	operator uint32_t() const
	{
		return uid_;
	}

private:
	uint32_t uid_;
};

constexpr Id invalid_id = Id(std::numeric_limits<uint32_t>::max());

// NOTE:  This is used to wrap `operations` in the `Netlist` represention of a
// quantum circuit.
template<typename OpType>
struct Wrapper {
	OpType op;
	mutable uint32_t data;

	Wrapper(OpType const& op, uint32_t const data_value)
	    : op(op), data(data_value)
	{}

	bool operator==(Wrapper const& other) const
	{
		return op == other.op;
	}
};

template<typename OpType>
struct Regular {
	OpType op;
	mutable uint32_t data;
	std::array<Id, OpType::max_num_wires> children;

	Regular(OpType const& op, uint32_t const data_value)
	    : op(op), data(data_value)
	{}

	bool operator==(Regular const& other) const
	{
		return op == other.op;
	}
};

template<typename OpType>
struct Irregular {
	OpType op;
	mutable uint32_t data;
	std::vector<Id> children;

	Irregular(OpType const& op, uint32_t const data_value)
	    : op(op), data(data_value), children(op.num_wires())
	{}

	bool operator==(Irregular const& other) const
	{
		return op == other.op;
	}
};

} // namespace tweedledum::node
