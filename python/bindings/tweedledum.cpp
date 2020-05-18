/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#include <mockturtle/networks/xag.hpp>
#include <pybind11/pybind11.h>
#include <sstream>
#include <string>
#include <tweedledum/algorithms/synthesis/oracles/xag_synth.h>
#include <tweedledum/ir/Circuit.h>
#include <tweedledum/ir/Module.h>
#include <tweedledum/writers/write_qasm.h>

namespace tweedledum {

std::string synthesize_xag(mockturtle::xag_network const& ntk)
{
	Circuit circuit = xag_synth(ntk);
	std::ostringstream os;
	write_qasm(circuit, os);
	return os.str();
}

} // namespace tweedledum

void init_tweedledum(pybind11::module& m)
{
	using namespace tweedledum;
	namespace py = pybind11;

	m.def("synthesize_xag", &synthesize_xag, "Synthesize XAG");
}
