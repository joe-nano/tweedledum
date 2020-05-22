/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../ir/Circuit.h"

#include <fstream>
#include <string>
#include <vector>

namespace tweedledum {

// TODO: make this accept a module
void write_qasm(Circuit const circuit, std::ostream& os = std::cout)
{
	// Header
	os << fmt::format("OPENQASM 2.0;\n");
	os << fmt::format("include \"qelib1.inc\";\n");
	os << fmt::format("qreg q[{}];\n", circuit.num_qubits());
	if (circuit.num_cbits()) {
		os << fmt::format("creg c[{}];\n", circuit.num_cbits());
	}
	// Body
	using op_type = typename Circuit::op_type;
	circuit.foreach_op([&](op_type const& op) {
		switch (op.id()) {
		default:
			return false;

		case gate_ids::i:
			os << fmt::format("id q[{}];\n", op.target());
			break;

		case gate_ids::h:
			os << fmt::format("h q[{}];\n", op.target());
			break;

		case gate_ids::x:
			os << fmt::format("x q[{}];\n", op.target());
			break;

		case gate_ids::y:
			os << fmt::format("y q[{}];\n", op.target());
			break;

		case gate_ids::z:
			os << fmt::format("z q[{}];\n", op.target());
			break;

		case gate_ids::s:
			os << fmt::format("s q[{}];\n", op.target());
			break;

		case gate_ids::sdg:
			os << fmt::format("sdg q[{}];\n", op.target());
			break;

		case gate_ids::t:
			os << fmt::format("t q[{}];\n", op.target());
			break;

		case gate_ids::tdg:
			os << fmt::format("tdg q[{}];\n", op.target());
			break;

		case gate_ids::cx: {
			wire::Id c = op.control();
			if (c.is_complemented()) {
				os << fmt::format("x q[{}];\n", c);
			}
			os << fmt::format(
			    "cx q[{}], q[{}];\n", c, op.target());
			if (c.is_complemented()) {
				os << fmt::format("x q[{}];\n", c);
			}
		} break;

		case gate_ids::cy:
			os << fmt::format(
			    "cy q[{}], q[{}];\n", op.control(), op.target());
			break;

		case gate_ids::cz:
			os << fmt::format(
			    "cz q[{}], q[{}];\n", op.control(), op.target());
			break;

		case gate_ids::swap:
			os << fmt::format("swap q[{}], q[{}];\n", op.target(0u),
			    op.target(1u));
			break;

		case gate_ids::ncx: {
			// FIXME: I wish OpenQASM had support for negative controls
			wire::Id c0 = op.control(0u);
			wire::Id c1 = op.control(1u);
			if (c0.is_complemented()) {
				os << fmt::format("x q[{}];\n", c0);
			}
			if (c1.is_complemented()) {
				os << fmt::format("x q[{}];\n", c1);
			}
			os << fmt::format("ccx q[{}], q[{}], q[{}];\n",
			    c0, c1, op.target());
			if (c0.is_complemented()) {
				os << fmt::format("x q[{}];\n", c0);
			}
			if (c1.is_complemented()) {
				os << fmt::format("x q[{}];\n", c1);
			}
		} break;
		// Parameterisable gates
		case gate_ids::r1:
			os << fmt::format("u1({}) q[{}];\n",
			    op.rotation_angle(), op.target());
			break;

		case gate_ids::rx:
			os << fmt::format("rx({}) q[{}];\n",
			    op.rotation_angle(), op.target());
			break;

		case gate_ids::ry:
			os << fmt::format("ry({}) q[{}];\n",
			    op.rotation_angle(), op.target());
			break;

		case gate_ids::rz:
			os << fmt::format("rz({}) q[{}];\n",
			    op.rotation_angle(), op.target());
			break;
		}
		return true;
	});
}

void write_qasm(Circuit const circuit, std::string_view filename)
{
	std::ofstream os(filename, std::ofstream::out);
	write_qasm(circuit, os);
}

} // namespace tweedledum