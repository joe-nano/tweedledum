/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../target/Device.h"
#include "Gate.h"
#include "Node.h"
#include "Storage.h"
#include "Wire.h"
#include "operations/w2_op.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fmt/format.h>
#include <limits>
#include <random>
#include <string>

namespace tweedledum {

/*! \brief
 */
class MappedDAG {
#pragma region Types and constructors
	struct mstrg_type {
		// Maps for original wire -> new wire
		std::vector<wire::Id> init_v_to_phy;
		std::vector<wire::Id> v_to_phy;
		Device device;

		mstrg_type(class Device const& d) : device(d) {}
	};

public:
	using base_type = MappedDAG;
	using op_type = w2_op;
	using node_type = node::Regular<w2_op>;
	using dstrg_type = Storage<node_type>;
	using wstrg_type = wire::Storage;

	MappedDAG(Device const& device)
	    : data_(std::make_shared<dstrg_type>("tweedledum_mapd_network")),
	      wires_(std::make_shared<wstrg_type>()),
	      map_(std::make_shared<mstrg_type>(device))
	{
		for (uint32_t i = 0; i < device.num_qubits(); ++i) {
			create_qubit();
		}
	}

	template<typename Network>
	MappedDAG(Network const& network, Device const& device)
	    : data_(std::make_shared<dstrg_type>("tweedledum_mapd_network")),
	      wires_(std::make_shared<wstrg_type>()),
	      map_(std::make_shared<mstrg_type>(device))
	{
		network.foreach_wire([&](wire::Id wire, std::string_view name) {
			if (wire.is_qubit()) {
				create_qubit(name);
			}
		});
		for (uint32_t i = network.num_qubits(); i < device.num_qubits();
		     ++i) {
			create_qubit();
		}
		network.foreach_wire([&](wire::Id wire, std::string_view name) {
			if (!wire.is_qubit()) {
				create_cbit(name);
			}
		});
	}
#pragma endregion

#pragma region Properties
	std::string_view name() const
	{
		return data_->name;
	}

	uint32_t size() const
	{
		return data_->nodes.size();
	}

	uint32_t capacity() const
	{
		return data_->nodes.capacity();
	}

	void reserve(uint32_t const new_cap)
	{
		data_->nodes.reserve(new_cap);
	}

	uint32_t num_operations() const
	{
		return (data_->nodes.size() - data_->inputs.size());
	}

	bool check_gate_set(uint64_t const allowed_gates) const
	{
		return (data_->gate_set & ~allowed_gates) == 0ull;
	}
#pragma endregion

#pragma region Nodes
	node::Id id(node_type const& n) const
	{
		return node::Id(static_cast<uint32_t>(&n - data_->nodes.data()));
	}

	node_type const& node(node::Id id) const
	{
		return data_->nodes.at(id);
	}
#pragma endregion

#pragma region Node custom values
	void default_value(uint32_t const value) const
	{
		data_->default_value = value;
	}

	void clear_values() const
	{
		std::for_each(data_->nodes.begin(), data_->nodes.end(),
		    [](node_type& node) {
			    node.data = 0u;
		    });
	}

	uint32_t value(node_type const& node) const
	{
		return node.data;
	}

	void value(node_type const& node, uint32_t const value) const
	{
		node.data = value;
	}

	uint32_t incr_value(node_type const& node) const
	{
		return ++node.data;
	}

	uint32_t decr_value(node_type const& node) const
	{
		assert(node.data > 0);
		return --node.data;
	}
#pragma endregion

#pragma region Wires
private:
	void connect_wire(wire::Id const w_id)
	{
		node::Id const n_id(data_->nodes.size());
		op_type const input(GateLib::input, w_id);
		data_->nodes.emplace_back(input, data_->default_value);
		data_->inputs.emplace_back(n_id);
		data_->outputs.emplace_back(n_id);
		map_->init_v_to_phy.push_back(w_id);
		map_->v_to_phy.push_back(w_id);
	}

	wire::Id create_qubit(std::string_view name, wire::Mode const mode
	                                             = wire::Mode::inout)
	{
		wire::Id const w_id = wires_->create_qubit(name, mode);
		connect_wire(w_id);
		return w_id;
	}

	wire::Id create_qubit(wire::Mode const mode = wire::Mode::inout)
	{
		std::string const name = fmt::format("__dum_q{}", num_qubits());
		return create_qubit(name, mode);
	}

public:
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

	wire::Id create_cbit(std::string_view name, wire::Mode const mode
	                                            = wire::Mode::inout)
	{
		wire::Id const w_id = wires_->create_cbit(name, mode);
		connect_wire(w_id);
		return w_id;
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

	std::string wire_name(wire::Id const w_id) const
	{
		return wires_->wire_name(w_id);
	}

	void wire_name(wire::Id const w_id, std::string_view new_name,
	    bool const rename = true)
	{
		wires_->wire_name(w_id, new_name, rename);
	}

	wire::Mode wire_mode(wire::Id const w_id) const
	{
		return wires_->wire_mode(w_id);
	}

	void wire_mode(wire::Id const w_id, wire::Mode const new_mode)
	{
		wires_->wire_mode(w_id, new_mode);
	}
#pragma endregion

#pragma region Creating operations(using wire ids)
private:
	void connect_node(wire::Id const wire, node_type& node)
	{
		assert(data_->outputs.at(wire) != node::invalid_id);
		uint32_t const position = node.op.position(wire);
		node.children.at(position) = data_->outputs.at(wire);
		data_->outputs.at(wire) = id(node);
		return;
	}

public:
	template<typename Op>
	node::Id emplace_op(Op&& op)
	{
		node::Id const id(data_->nodes.size());
		node_type& node = data_->nodes.emplace_back(
		    std::forward<Op>(op), data_->default_value);
		data_->gate_set |= (1 << static_cast<uint32_t>(op.id()));
		node.op.foreach_control([&](wire::Id wire) {
			connect_node(wire, node);
		});
		node.op.foreach_target([&](wire::Id wire) {
			connect_node(wire, node);
		});
		return id;
	}

	node::Id create_op(Gate const& g, wire::Id const phy0)
	{
		return emplace_op(op_type(g, phy0));
	}

	node::Id create_op(
	    Gate const& g, wire::Id const phy0, wire::Id const phy1)
	{
		if (!map_->device.are_connected(phy0, phy1)) {
			return node::invalid_id;
		}
		return emplace_op(op_type(g, phy0, phy1));
	}

	node::Id create_op(Gate const& g, std::vector<wire::Id> const& controls,
	    std::vector<wire::Id> const& targets)
	{
		if (controls.size() + targets.size() > 2u) {
			return node::invalid_id;
		}
		if (controls.empty() && targets.size() == 1) {
			return emplace_op(op_type(g, targets.at(0)));
		}
		if (!map_->device.are_connected(controls.at(0), targets.at(0)))
		{
			return node::invalid_id;
		}
		return emplace_op(op_type(g, controls.at(0), targets.at(0)));
	}
#pragma endregion

#pragma region Iterators
	template<typename Fn>
	void foreach_wire(Fn&& fn) const
	{
		wires_->foreach_wire(std::forward<Fn>(fn));
	}

	template<typename Fn>
	void foreach_input(Fn&& fn) const
	{
		// clang-format off
		static_assert(std::is_invocable_r_v<void, Fn, node::Id const> ||
		              std::is_invocable_r_v<void, Fn, node_type const&> ||
		              std::is_invocable_r_v<void, Fn, node_type const&, node::Id const>);
		// clang-format on
		for (uint32_t i = 0u, i_limit = data_->inputs.size();
		     i < i_limit; ++i) {
			if constexpr (std::is_invocable_r_v<void, Fn,
			                  node::Id const>) {
				fn(data_->inputs.at(i));
			} else if constexpr (std::is_invocable_r_v<void, Fn,
			                         node_type const&>) {
				fn(node(data_->inputs.at(i)));
			} else {
				fn(node(data_->inputs.at(i)),
				    data_->inputs.at(i));
			}
		}
	}

	template<typename Fn>
	void foreach_output(Fn&& fn) const
	{
		// clang-format off
		static_assert(std::is_invocable_r_v<void, Fn, node::Id const> ||
		              std::is_invocable_r_v<void, Fn, node_type const&> ||
		              std::is_invocable_r_v<void, Fn, node_type const&, node::Id const>);
		// clang-format on
		for (uint32_t i = 0u, i_limit = data_->outputs.size();
		     i < i_limit; ++i) {
			if constexpr (std::is_invocable_r_v<void, Fn,
			                  node::Id const>) {
				fn(data_->outputs.at(i));
			} else if constexpr (std::is_invocable_r_v<void, Fn,
			                         node_type const&>) {
				fn(node(data_->outputs.at(i)));
			} else {
				fn(node(data_->outputs.at(i)),
				    data_->outputs.at(i));
			}
		}
	}

	template<typename Fn>
	void foreach_op(Fn&& fn) const
	{
		// clang-format off
		static_assert(std::is_invocable_r_v<void, Fn, op_type const&> ||
		              std::is_invocable_r_v<void, Fn, op_type const&, node_type const&>);
		// clang-format on
		for (uint32_t i = 0u, i_limit = data_->nodes.size();
		     i < i_limit; ++i) {
			node_type const& node = data_->nodes.at(i);
			if (node.op.is_meta()) {
				continue;
			}
			if constexpr (std::is_invocable_r_v<void, Fn,
			                  op_type const&>) {
				fn(node.op);
			} else {
				fn(node.op, node);
			}
		}
	}

	template<typename Fn>
	void foreach_rop(Fn&& fn) const
	{
		// clang-format off
		static_assert(std::is_invocable_r_v<void, Fn, op_type const&> ||
		              std::is_invocable_r_v<void, Fn, op_type const&, node_type const&>);
		// clang-format on
		for (uint32_t i = data_->nodes.size(); i-- > 0u;) {
			node_type const& node = data_->nodes.at(i);
			if (node.op.is_meta()) {
				continue;
			}
			if constexpr (std::is_invocable_r_v<void, Fn,
			                  op_type const&>) {
				fn(node.op);
			} else {
				fn(node.op, node);
			}
		}
	}

	template<typename Fn>
	void foreach_node(Fn&& fn) const
	{
		// clang-format off
		static_assert(std::is_invocable_r_v<void, Fn, node_type const&> ||
		              std::is_invocable_r_v<void, Fn, node_type const&, node::Id const>);
		// clang-format on
		for (uint32_t i = 0u, i_limit = data_->nodes.size();
		     i < i_limit; ++i) {
			if constexpr (std::is_invocable_r_v<void, Fn,
			                  node_type const&>) {
				fn(data_->nodes.at(i));
			} else {
				fn(data_->nodes.at(i), node::Id(i));
			}
		}
	}
#pragma endregion

#pragma region Operation iterators
	template<typename Fn>
	void foreach_child(node_type const& n, Fn&& fn) const
	{
		// clang-format off
		static_assert(std::is_invocable_r_v<void, Fn, node_type const&> ||
		              std::is_invocable_r_v<void, Fn, node_type const&, node::Id const> ||
		              std::is_invocable_r_v<void, Fn, node_type const&, wire::Id const>);
		for (uint32_t position = 0u; position < n.children.size(); ++position) {
			if (n.children[position] == node::invalid_id) {
				continue;
			}
			if constexpr (std::is_invocable_r_v<void, Fn, node_type const&>) {
				fn(node(n.children[position]));
			} else if constexpr (std::is_invocable_r_v<void, Fn, node_type const&, node::Id const>) {
				fn(node(n.children[position]), n.children[position]);
			} else if constexpr (std::is_invocable_r_v<void, Fn, node_type const&, wire::Id const>) {
				fn(node(n.children[position]), n.op.wire(position));
			}
		}
		// clang-format on
	}
#pragma endregion

#pragma region Mapping
	void v_to_phy(std::vector<wire::Id> const& placement)
	{
		assert(placement.size() == num_qubits());
		if (num_operations() == 0) {
			std::copy(placement.begin(), placement.end(),
			    map_->init_v_to_phy.begin());
		}
		for (uint32_t i = 0; i < placement.size(); ++i) {
			if (map_->init_v_to_phy.at(i) == wire::invalid_id) {
				map_->init_v_to_phy.at(i) = placement.at(i);
			}
			map_->v_to_phy.at(i) = placement.at(i);
		}
	}

	std::vector<wire::Id> v_to_phy() const
	{
		return {map_->v_to_phy.begin(),
		    map_->v_to_phy.begin() + num_qubits()};
	}

	std::vector<wire::Id> init_phy_to_v() const
	{
		std::vector<wire::Id> map(num_qubits(), wire::invalid_id);
		for (uint32_t i = 0; i < num_qubits(); ++i) {
			map[map_->init_v_to_phy[i]] = wire::Id(i, true);
		}
		return map;
	}

	std::vector<wire::Id> phy_to_v() const
	{
		std::vector<wire::Id> map(num_qubits(), wire::invalid_id);
		for (uint32_t i = 0; i < num_qubits(); ++i) {
			map[map_->v_to_phy[i]] = wire::Id(i, true);
		}
		return map;
	}
#pragma endregion

private:
	std::shared_ptr<dstrg_type> data_;
	std::shared_ptr<wstrg_type> wires_;
	std::shared_ptr<mstrg_type> map_;
};

} // namespace tweedledum
