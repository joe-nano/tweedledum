/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/writers/write_qasm.h"

#include "tweedledum/ir/Circuit.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Wire.h"

#include <catch.hpp>
#include <sstream>
#include <vector>

TEST_CASE("QASM Writer", "[qasm]")
{
	using namespace tweedledum;
	std::vector<Gate> one_qubit
	    = {GateLib::i, GateLib::h, GateLib::x, GateLib::y, GateLib::z,
	        GateLib::s, GateLib::t, GateLib::sdg, GateLib::tdg};
	std::vector<Gate> two_qubit
	    = {GateLib::cx, GateLib::cy, GateLib::cz, GateLib::swap};
	std::vector<Gate> three_qubit = {GateLib::ncx};

	Circuit circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();

	for (Gate const& gate : one_qubit) {
		circuit.create_op(gate, q0);
	}
	for (Gate const& gate : two_qubit) {
		circuit.create_op(gate, q0, q1);
	}
	for (Gate const& gate : three_qubit) {
		circuit.create_op(gate, q0, q1, q2);
	}
	std::ostringstream os;
	write_qasm(circuit, os);

	std::string expected_qasm = "OPENQASM 2.0;\n"
	                            "include \"qelib1.inc\";\n"
	                            "qreg q[3];\n"
	                            "id q[0];\n"
	                            "h q[0];\n"
	                            "x q[0];\n"
	                            "y q[0];\n"
	                            "z q[0];\n"
	                            "s q[0];\n"
	                            "t q[0];\n"
	                            "sdg q[0];\n"
	                            "tdg q[0];\n"
	                            "cx q[0], q[1];\n"
	                            "cy q[0], q[1];\n"
	                            "cz q[0], q[1];\n"
	                            "swap q[0], q[1];\n"
	                            "ccx q[0], q[1], q[2];\n";
	CHECK(os.str() == expected_qasm);
}
