/*-------------------------------------------------------------------------------------------------
| Part of the tweedledum project.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "../IR/Module.h"

#include <iostream>
#include <string>
#include <vector>

namespace tweedledum {

namespace detail {

class string_builder {
public:
	explicit string_builder() : max_name_length_(0u)
	{}

	void add_qubit(std::string_view name)
	{
		if (name.length() > max_name_length_) {
			max_name_length_ = name.length();
		}
		wires_.emplace_back(name);
		circuit_lines_.emplace_back("    ");
		circuit_lines_.emplace_back("────");
		circuit_lines_.emplace_back("    ");
	}

	void add_cbit(std::string_view name)
	{
		if (name.length() > max_name_length_) {
			max_name_length_ = name.length();
		}
		wires_.emplace_back(name);
		circuit_lines_.emplace_back("    ");
		circuit_lines_.emplace_back("════");
		circuit_lines_.emplace_back("    ");
	}

	std::string str()
	{
		std::string result;
		for (uint32_t i = 0u; i < circuit_lines_.size(); i += 3u) {
			result += fmt::format("{0:>{1}}   {2}\n{3:>{1}} : {4}\n{0:>{1}}   {5}\n",
			                      "", max_name_length_, circuit_lines_.at(i),
			                      wires_.at(i / 3), circuit_lines_.at(i + 1),
			                      circuit_lines_.at(i + 2));
		}
		return result;
	}

private:
	std::vector<std::string> wires_;
	std::vector<std::string> circuit_lines_;
	uint32_t max_name_length_;
};

}

/*! \brief 
 */
void write_utf8(Module const* module, std::ostream& os = std::cout)
{
	std::string utf8_str;
	detail::string_builder builder;
	module->circuit_.foreach_wire([&](wire::Id wire, std::string_view name) {
		if (wire.is_qubit()) {
			builder.add_qubit(name);
			return;
		}
		builder.add_cbit(name);
	});
	os << builder.str();
}

} // namespace tweedledum