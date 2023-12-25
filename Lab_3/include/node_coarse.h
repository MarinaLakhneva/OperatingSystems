#pragma once

class NodeCoarse {
  public:
    NodeCoarse(int val, NodeCoarse *prev_node = nullptr, NodeCoarse *next_node = nullptr) {
        node_val = val;
        next = next_node;
        prev = prev_node;
        if (next_node)
            next_node->prev = this;
        if (prev_node)
            prev_node->next = this;
    }

    inline int val() const { return node_val; }

    NodeCoarse *next, *prev;

  private:
    int node_val;
};
