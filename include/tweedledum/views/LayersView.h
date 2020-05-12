/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../ir/Node.h"
#include "../support/NodeMap.h"
#include "ImmutableView.h"

#include <vector>

namespace tweedledum {

/*! \brief Implements the circuit interface methods `layer(node)`,
 * `layer(layer_index)` and `depth`.
 *
 * The layers are computed at construction and can be recomputed by calling the
 * `update` method.
 *
 * NOTE: The 0th layer correspond to the input nodes.
 */
template<typename Circuit>
class LayersView : public ImmutableView<Circuit> {
public:
	using base_type = typename Circuit::base_type;
	using op_type = typename Circuit::op_type;
	using node_type = typename Circuit::node_type;

	explicit LayersView(Circuit const circuit)
	    : ImmutableView<Circuit>(circuit), node_layer_(circuit)
	{
		if (this->num_wires()) {
			update();
		}
	}

	// NOTE: the depth of a quantum circuit is the number layers with gates.
	uint32_t depth() const
	{
		return layer_nodes_.empty() ? 0u : layer_nodes_.size() - 1;
	}

	uint32_t num_layers() const
	{
		return layer_nodes_.size();
	}

	uint32_t layer(node::Id const nid) const
	{
		return node_layer_.at(nid);
	}

	uint32_t layer(node_type const& node) const
	{
		return node_layer_.at(node);
	}

	std::vector<node::Id> layer(uint32_t const i) const
	{
		return layer_nodes_[i];
	}

	void update()
	{
		reset();
		compute_layers();
		this->clear_values();
	}

private:
	void reset()
	{
		node_layer_.reset(0);
		layer_nodes_.clear();
		if (this->num_wires() > 0) {
			layer_nodes_.resize(1);
		}
		this->clear_values();
	}

	void compute_layers()
	{
		this->foreach_input([&](node::Id const nid) {
			layer_nodes_.at(0).push_back(nid);
			node_layer_[nid] = 0u;
		});
		this->foreach_node([&](node_type const& node,
		                       node::Id const nid) {
			if (node.op.is_meta()) {
				return;
			}
			uint32_t layer = 0u;
			this->foreach_child(node, [&](node_type const& child) {
				layer = std::max(layer, node_layer_[child]);
			});
			layer += 1;
			if (layer == layer_nodes_.size()) {
				layer_nodes_.emplace_back();
			}
			node_layer_[node] = layer;
			layer_nodes_.at(layer).push_back(nid);
		});
	}

private:
	NodeMap<uint32_t, Circuit> node_layer_;
	std::vector<std::vector<node::Id>> layer_nodes_;
};

} // namespace tweedledum
