/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

/* Taken from mockturtle (: */

#include "../ir/Node.h"

#include <cassert>
#include <memory>
#include <vector>

namespace tweedledum {

/*! \brief Associative container for circuit nodes
 *
 * This container helps to store values associated to nodes in a circuit.  The
 * container is initialized with a circuit to derive the size according to the
 * number of nodes.  The container can be accessed via nodes, or indirectly via
 * `node::id`, from which the corresponding node is derived.
 *
 * The implementation uses a vector as underlying data structure
 */
template<class T, class Circuit>
class NodeMap {
public:
	using node_type = typename Circuit::node_type;
	using reference = typename std::vector<T>::reference;
	using const_reference = typename std::vector<T>::const_reference;

public:
	/*! \brief Default constructor. */
	explicit NodeMap(Circuit const circuit)
	    : circuit_(circuit),
	      data_(std::make_shared<std::vector<T>>(circuit.size()))
	{}

	/*! \brief Constructor with default value.
	 *
	 * Initializes all values in the container to `init_value`.
	 */
	NodeMap(Circuit const circuit, T const& init_value)
	    : circuit_(circuit),
	      data_(std::make_shared<std::vector<T>>(circuit.size(), init_value))
	{}

#pragma region Access using node reference
	/*! \brief Mutable access to value by node. */
	reference operator[](node_type const& node)
	{
		return (*data_)[circuit_.id(node)];
	}

	/*! \brief Constant access to value by node. */
	const_reference operator[](node_type const& node) const
	{
		return (*data_)[circuit_.id(node)];
	}

	/*! \brief Mutable access to value by node. */
	reference at(node_type const& node)
	{
		assert(
		    circuit_.id(node) < data_->size() && "index out of bounds");
		return (*data_)[circuit_.id(node)];
	}

	/*! \brief Constant access to value by node. */
	const_reference at(node_type const& node) const
	{
		assert(
		    circuit_.id(node) < data_->size() && "index out of bounds");
		return (*data_)[circuit_.id(node)];
	}
#pragma endregion

#pragma region Access using node identifier
	/*! \brief Mutable access to value by `node::id`. */
	reference operator[](node::Id const nid)
	{
		return (*data_)[nid];
	}

	/*! \brief Constant access to value by `node::id`. */
	const_reference operator[](node::Id const nid) const
	{
		return (*data_)[nid];
	}

	/*! \brief Mutable access to value by `node::id`. */
	reference at(node::Id const nid)
	{
		assert(nid < data_->size() && "index out of bounds");
		return (*data_)[nid];
	}

	/*! \brief Constant access to value by `node::id`. */
	const_reference at(node::Id const nid) const
	{
		assert(nid < data_->size() && "index out of bounds");
		return (*data_)[nid];
	}
#pragma endregion

	/*! \brief Resets the size of the map.
	 *
	 * This function should be called, if the circuit changed in size.  Then
	 * the map is cleared and resized to the current circuit's size.  All
	 * values are initialized with `init_value`.
	 *
	 * \param init_value Initialization value after resize
	 */
	void reset(T const& init_value = {})
	{
		data_->clear();
		data_->resize(circuit_.size(), init_value);
	}

	/*! \brief Resizes the map.
	 *
	 * This function should be called, if the NodeMap's size needs to be
	 * changed without clearing its data.
	 *
	 * \param init_value Initialization value after resize
	 */
	void resize(T const& init_value = {})
	{
		if (circuit_.size() > data_->size()) {
			data_->resize(circuit_.size(), init_value);
		}
	}

private:
	Circuit const circuit_;
	std::shared_ptr<std::vector<T>> data_;
};

} // namespace tweedledum