/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../support/Angle.h"
#include "Circuit.h"
#include "Gate.h"
#include "Wire.h"

#include <cstdint>
#include <parallel_hashmap/phmap.h>
#include <string>

namespace tweedledum {

class AbstractGate {
public:
	std::string_view name;

	bool (&is)(Gate const&);

	void (&create_op)(Circuit&, std::vector<wire::Id> const&,
	    std::vector<Angle> const&);

	template<typename OpClass>
	static AbstractGate get()
	{
		return AbstractGate(
		    OpClass::name(), OpClass::is, OpClass::create_op);
	}

private:
	AbstractGate(std::string_view name, bool (&is)(Gate const&),
	    void (&create_op)(Circuit&, std::vector<wire::Id> const&,
	        std::vector<Angle> const&))
	    : name(name), is(is), create_op(create_op)
	{}
};

class GateLib2 {
public:
	GateLib2() = default;

	AbstractGate const* lookup(std::string_view name) const
	{
		auto it = registered_gates_.find(name);
		if (it != registered_gates_.end()) {
			return &it->second;
		}
		return nullptr;
	}

	template<typename... Args>
	void add_gate()
	{
		GateAdder<Args...>::add(*this);
	}

private:
	template<typename First, typename... Rest>
	class GateAdder {
	public:
		static void add(GateLib2& set)
		{
			set.add_gate(AbstractGate::get<First>());
			GateAdder<Rest...>::add(set);
		}
	};

	template<typename First>
	class GateAdder<First> {
	public:
		static void add(GateLib2& set)
		{
			set.add_gate(AbstractGate::get<First>());
		}
	};

	void add_gate(AbstractGate gate)
	{
		registered_gates_.emplace(gate.name, gate);
	}

	GateLib2(const GateLib2&) = delete;
	void operator=(GateLib2&) = delete;

	phmap::node_hash_map<std::string, AbstractGate> registered_gates_;
};

// Meta gates
// constexpr Gate undefined(
//     sym_angle::zero, sym_angle::zero, sym_angle::zero, gate_ids::undefined);
// constexpr Gate opaque(
//     sym_angle::zero, sym_angle::zero, sym_angle::zero, gate_ids::opaque);
// constexpr Gate input(
//     sym_angle::zero, sym_angle::zero, sym_angle::zero, gate_ids::input);

class H : public Gate {
public:
	static std::string_view name()
	{
		return "h";
	}

	static bool is(Gate const& gate)
	{
		(void) gate;
		return true;
	}

	static void create_op(Circuit& circuit,
	    std::vector<wire::Id> const& qubits,
	    std::vector<Angle> const& parameters)
	{
		(void) parameters;
		circuit.create_op(GateLib::h, qubits.at(0));
	}
};

class X : public Gate {
public:
	static std::string_view name()
	{
		return "x";
	}

	static bool is(Gate const& gate)
	{
		(void) gate;
		return true;
	}

	static void create_op(Circuit& circuit,
	    std::vector<wire::Id> const& qubits,
	    std::vector<Angle> const& parameters)
	{
		(void) parameters;
		circuit.create_op(GateLib::x, qubits.at(0));
	}
};

class Y : public Gate {
public:
};

class Z : public Gate {
public:
};

class S : public Gate {
public:
};

class Sdg : public Gate {
public:
};

class T : public Gate {
public:
};

class Tdg : public Gate {
public:
};

class R1 : public Gate {
public:
};

class Rx : public Gate {
public:
};

class Ry : public Gate {
public:
};

class Rz : public Gate {
public:
};

class U3 : public Gate {
public:
};

class Swap : public Gate {
public:
};

inline void register_gates(GateLib2& gate_lib)
{
	gate_lib.add_gate<X>();
}

} // namespace tweedledum
