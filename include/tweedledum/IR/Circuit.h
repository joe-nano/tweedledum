/*-------------------------------------------------------------------------------------------------
| Part of the tweedledum project.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "Wire.h"

#include <memory>

namespace tweedledum {

class Circuit {
public:
	explicit Circuit() : wires_(std::make_shared<wire::Storage>())
	{}

	uint32_t num_wires() const
	{
		return wires_->num_wires();
	}

	uint32_t num_qubits() const
	{
		return wires_->num_qubits();
	}

	uint32_t num_cbits() const
	{
		return wires_->num_cbits();
	}

	wire::Id create_qubit(std::string_view name, wire::Mode const mode = wire::Mode::inout)
	{
		wire::Id const id = wires_->create_qubit(name, mode);
		return id;
	}

	wire::Id create_qubit(wire::Mode const mode = wire::Mode::inout)
	{
		std::string const name = fmt::format("__dum_q{}", num_qubits());
		return create_qubit(name, mode);
	}

	wire::Id create_cbit(std::string_view name, wire::Mode const mode = wire::Mode::inout)
	{
		wire::Id const id = wires_->create_cbit(name, mode);
		return id;
	}

	wire::Id create_cbit(wire::Mode const mode = wire::Mode::inout)
	{
		std::string const name = fmt::format("__dum_c{}", num_cbits());
		return create_cbit(name, mode);
	}

	wire::Id wire(std::string_view name) const
	{
		return wires_->wire(name);
	}

	std::string wire_name(wire::Id const id) const
	{
		return wires_->wire_name(id);
	}

	void wire_name(wire::Id const id, std::string_view new_name, bool const rename = true)
	{
		wires_->wire_name(id, new_name, rename);
	}

	wire::Mode wire_mode(wire::Id const id) const
	{
		return wires_->wire_mode(id);
	}

	void wire_mode(wire::Id const id, wire::Mode const new_mode)
	{
		wires_->wire_mode(id, new_mode);
	}

	template<typename Fn>
	void foreach_wire(Fn&& fn) const
	{
		wires_->foreach_wire(std::forward<Fn>(fn));
	}

private:
	std::shared_ptr<wire::Storage> wires_;
};

} // namespace tweedledum