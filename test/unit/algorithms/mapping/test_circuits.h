/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include "tweedledum/ir/CircuitDAG.h"
#include "tweedledum/ir/Gate.h"
#include "tweedledum/ir/Wire.h"

using namespace tweedledum;

inline CircuitDAG test_circuit_00()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();

	circuit.create_op(GateLib::cx, q1, q0);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q2, q0);
	return circuit;
}

// Extend ZDD mapper paper example
inline CircuitDAG test_circuit_01()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q3 = circuit.create_qubit();
	circuit.create_cbit();

	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q3);
	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q3);
	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q3, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q3);

	return circuit;
}

// Extend ZDD mapper paper example #2
inline CircuitDAG test_circuit_02()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q3 = circuit.create_qubit();
	circuit.create_cbit();

	circuit.create_qubit();
	auto q5 = circuit.create_qubit();

	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q5);
	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);

	return circuit;
}

// Extend ZDD mapper paper example #3
inline CircuitDAG test_circuit_03()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q3 = circuit.create_qubit();
	circuit.create_cbit();

	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q3);
	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q3);
	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q3, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q3);
	circuit.create_op(GateLib::cx, q3, q2);
	circuit.create_op(GateLib::cx, q3, q1);
	circuit.create_op(GateLib::cx, q3, q0);

	return circuit;
}

// Extend ZDD mapper paper example #3.5
inline CircuitDAG test_circuit_04()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q3 = circuit.create_qubit();
	circuit.create_cbit();

	wire::Id q4 = circuit.create_qubit();
	wire::Id q5 = circuit.create_qubit();
	wire::Id q6 = circuit.create_qubit();
	wire::Id q7 = circuit.create_qubit();
	wire::Id q8 = circuit.create_qubit();
	wire::Id q9 = circuit.create_qubit();

	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q5);
	circuit.create_op(GateLib::cx, q9, q8);
	circuit.create_op(GateLib::cx, q1, q5);
	circuit.create_op(GateLib::cx, q4, q3);
	circuit.create_op(GateLib::cx, q8, q7);
	circuit.create_op(GateLib::cx, q6, q8);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q2, q5);
	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);

	return circuit;
}

// Extend ZDD mapper paper #4
inline CircuitDAG test_circuit_05()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q3 = circuit.create_qubit();
	circuit.create_cbit();

	wire::Id q4 = circuit.create_qubit();
	wire::Id q5 = circuit.create_qubit();
	wire::Id q6 = circuit.create_qubit();
	wire::Id q7 = circuit.create_qubit();

	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q1, q3);
	circuit.create_op(GateLib::cx, q4, q5);
	circuit.create_op(GateLib::cx, q5, q6);
	circuit.create_op(GateLib::cx, q5, q7);

	return circuit;
}

inline CircuitDAG test_circuit_06()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q3 = circuit.create_qubit();
	circuit.create_cbit();

	wire::Id q4 = circuit.create_qubit();
	wire::Id q5 = circuit.create_qubit();

	circuit.create_op(GateLib::cx, q0, q2);
	circuit.create_op(GateLib::cx, q2, q1);
	circuit.create_op(GateLib::cx, q0, q4);
	circuit.create_op(GateLib::cx, q3, q0);
	circuit.create_op(GateLib::cx, q0, q5);

	return circuit;
}

inline CircuitDAG test_circuit_07()
{
	CircuitDAG circuit(nullptr);
	wire::Id q0 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q1 = circuit.create_qubit();
	wire::Id q2 = circuit.create_qubit();
	circuit.create_cbit();
	wire::Id q3 = circuit.create_qubit();
	circuit.create_cbit();

	wire::Id q4 = circuit.create_qubit();

	circuit.create_op(GateLib::cx, q0, q1);
	circuit.create_op(GateLib::cx, q1, q2);
	circuit.create_op(GateLib::cx, q2, q3);
	circuit.create_op(GateLib::cx, q3, q4);
	circuit.create_op(GateLib::cx, q0, q4);

	return circuit;
}