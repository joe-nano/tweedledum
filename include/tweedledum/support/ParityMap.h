/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "Angle.h"
#include "hash.h"

#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace tweedledum {

/*! \brief Associative container for mapping a parity term to a angle. */
template<typename Term = uint32_t>
class ParityMap {
public:
#pragma region Types and constructors
	ParityMap() = default;
#pragma endregion

#pragma region Properties
	auto num_terms() const
	{
		return term_to_angle_.size();
	}
#pragma endregion

#pragma region Iterators
	auto begin() const
	{
		return term_to_angle_.cbegin();
	}

	auto end() const
	{
		return term_to_angle_.cend();
	}
#pragma endregion

#pragma region Modifiers
	/*! \brief Add parity term.
	 *
	 * If the term already exist it increments the rotation angle
	 */
	void add_term(Term term, Angle rotation_angle)
	{
		assert(rotation_angle != sym_angle::zero);
		auto search = term_to_angle_.find(term);
		if (search != term_to_angle_.end()) {
			search->second += rotation_angle;
		} else {
			term_to_angle_.emplace(term, rotation_angle);
		}
	}

	/*! \brief Extract parity term. */
	Angle extract_term(Term term)
	{
		auto search = term_to_angle_.find(term);
		if (search != term_to_angle_.end()) {
			Angle const result = search->second;
			term_to_angle_.erase(search);
			return result;
		} else {
			return sym_angle::zero;
		}
	}
#pragma endregion

private:
	std::unordered_map<Term, Angle, hash<Term>> term_to_angle_;
};

} // namespace tweedledum
