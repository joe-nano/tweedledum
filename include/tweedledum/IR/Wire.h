/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace tweedledum::wire {

/* \brief Simple class to hold a wire identifier ``wire::Id``
 *
 *  In tweedledum, a wire can be either a quantum or classical.  A quantum wire
 * holds the state of a qubit, and it is represented by a line in quantum
 * circuit diagrams.  In tweedledum, a quantum wire is equivalent to a qubit.
 * Similarly, a classical wire holds the state of a cbit, and it is represented
 * by a double line in quantum circuit diagrams.
 *
 *  An ``wire::Id`` serves three purposes:
 *  1) Uniquely identifying a wire within a circuit
 *  2) Informing if a wire is a qubit or a cbit
 *  3) When used withing controlled gates, informing if the use of the wire is
 * complemented or not e.g., a CX can be negatively controlled by adding a NOT
 * before and after the control
 *
 * Limits: a circuit can only have 2^30 - 1 wires! (the last one I use to
 * identify a invalid wire)
 */
class Id {
public:
#pragma region Types and constructors
	constexpr Id(uint32_t const uid, bool const is_qubit)
	    : uid_(uid), is_qubit_(static_cast<uint32_t>(is_qubit)),
	      is_complemented_(0)
	{
		assert(uid <= (std::numeric_limits<uint32_t>::max() >> 2));
	}

	constexpr Id(
	    uint32_t const uid, bool const is_qubit, bool const is_complemented)
	    : uid_(uid), is_qubit_(static_cast<uint32_t>(is_qubit)),
	      is_complemented_(static_cast<uint32_t>(is_complemented))
	{
		assert(uid <= (std::numeric_limits<uint32_t>::max() >> 2));
	}
#pragma endregion

#pragma region Properties
	uint32_t uid() const
	{
		return uid_;
	}

	bool is_complemented() const
	{
		return static_cast<bool>(is_complemented_);
	}

	bool is_qubit() const
	{
		return static_cast<bool>(is_qubit_);
	}

	/*! \brief Guarantee the return of a not complemented ``wire::Id`` */
	Id wire() const
	{
		return Id(uid(), is_qubit());
	}
#pragma endregion

#pragma region Modifiers
	void complement()
	{
		is_complemented_ ^= 1u;
	}
#pragma endregion

#pragma region Overloads
	operator uint32_t() const
	{
		return uid_;
	}

	Id operator!() const
	{
		Id complemented(*this);
		complemented.complement();
		return complemented;
	}

	bool operator<(Id other) const
	{
		return data_ < other.data_;
	}

	bool operator==(Id other) const
	{
		return data_ == other.data_;
	}

	bool operator!=(Id other) const
	{
		return data_ != other.data_;
	}
#pragma endregion

private:
	union {
		uint32_t data_;
		struct {
			uint32_t uid_ : 30;
			uint32_t is_qubit_ : 1;
			uint32_t is_complemented_ : 1;
		};
	};
};

constexpr Id make_qubit(uint32_t const uid, bool is_complemented = false)
{
	return Id(uid, /* is_qubit */ true, is_complemented);
}

constexpr Id make_cbit(uint32_t const uid, bool is_complemented = false)
{
	return Id(uid, /* is_qubit */ false, is_complemented);
}

constexpr Id invalid_id
    = Id(std::numeric_limits<uint32_t>::max() >> 2, true, true);

enum class Mode : uint8_t {
	in,
	out,
	inout,
	ancilla,
};

/* \brief Class used for storing wire information in a circuit
 */
class Storage {
	struct WireInfo {
		Id wire_id;
		Mode mode;
		std::string name;

		WireInfo(
		    Id const wire_id, Mode const mode, std::string_view name)
		    : wire_id(wire_id), mode(mode), name(name)
		{}
	};

public:
	Storage() : num_qubits_(0u) {}

	uint32_t num_wires() const
	{
		return wires_.size();
	}

	uint32_t num_qubits() const
	{
		return num_qubits_;
	}

	uint32_t num_cbits() const
	{
		return num_wires() - num_qubits();
	}

	Id create_qubit(std::string_view name, Mode const mode)
	{
		Id const qubit_id = make_qubit(wires_.size());
		wires_.emplace_back(qubit_id, mode, name);
		name_to_wire_.emplace(name, qubit_id);
		++num_qubits_;
		return qubit_id;
	}

	Id create_cbit(std::string_view name, Mode const mode)
	{
		Id const cbit_id = make_cbit(wires_.size());
		wires_.emplace_back(cbit_id, mode, name);
		name_to_wire_.emplace(name, cbit_id);
		return cbit_id;
	}

	Id wire(std::string_view name) const
	{
		return name_to_wire_.at(std::string(name));
	}

	std::string wire_name(Id const wire_id) const
	{
		return wires_.at(wire_id).name;
	}

	/* \brief Add a new name to identify a wire.
	 *
	 * \param rename If true, this flag indicates that `new_name` must
	 * substitute the previous name. (default: `true`)
	 */
	void wire_name(
	    Id const wire_id, std::string_view new_name, bool const rename)
	{
		if (rename) {
			name_to_wire_.erase(wires_.at(wire_id).name);
			wires_.at(wire_id).name = new_name;
		}
		name_to_wire_.emplace(new_name, wire_id);
	}

	Mode wire_mode(Id const wire_id) const
	{
		return wires_.at(wire_id).mode;
	}

	void wire_mode(Id const wire_id, Mode const new_mode)
	{
		wires_.at(wire_id).mode = new_mode;
	}

	template<typename Fn>
	void foreach_wire(Fn&& fn) const
	{
		// clang-format off
		static_assert(std::is_invocable_r_v<void, Fn, Id> ||
			      std::is_invocable_r_v<void, Fn, Id, std::string_view>);
		// clang-format on
		for (uint32_t i = 0u; i < wires_.size(); ++i) {
			if constexpr (std::is_invocable_r_v<void, Fn, Id>) {
				fn(wires_.at(i).wire_id);
			} else {
				fn(wires_.at(i).wire_id, wires_.at(i).name);
			}
		}
	}

private:
	uint32_t num_qubits_ = 0u;
	std::vector<WireInfo> wires_;
	std::unordered_map<std::string, Id> name_to_wire_;
};

} // namespace tweedledum::wire
