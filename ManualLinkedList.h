#pragma once

#include <vector>
#include "Graph.h"


/**
 * @brief ManualLinkedListBase holds the memory base of the linked lists.
 * In this problem, blocks are maintained as linked lists, and blocks are disjoint.
 * Therefore, we can integrate them together by two vectors: prev and next.
 * And more importantly, we need to support the removal of vertices by their value,
 * which it is not very convenient to do with a std::list.
 * 
 * 0 ~ n-1 are for the vertices, n maintains a pool of recycled blocks.
 * blocks are assigned to dynamic id but larger than n.
 * The head array stores the head of the block each vertex belongs to.
 */

class ManualLinkedListBase {
    friend class ManualLinkedList;
    // prev and next may reallocate, but head does not.
    std::vector<VertexIndex> prev;
    std::vector<VertexIndex> next;
    std::vector<VertexIndex> head;
    VertexIndex blockPool; // stores ids of recycled blocks. Maintained as a forward linked list.

    public:
    ManualLinkedListBase(size_t s)
    : prev(s+1, INVALID_VERTEX), next(s+1, INVALID_VERTEX), head(s, INVALID_VERTEX), blockPool(s) {}

    // Remove a vertex from its current linked list.
    // Return the VertexIndex at the next position.
    VertexIndex erase(VertexIndex v);

    // Create a new linked list.
    ManualLinkedList newList();

    // Recycle a linked list.
    void recycleList(VertexIndex id) { next[id] = next[blockPool]; next[blockPool] = id; }
    
};

/**
 * @brief ManualLinkedList is a linked list of vertices.
 * It does not own the memory, so it can be copied and moved at a low cost.
 * Each ManualLinkedList serves for a block in FrontierManager.
 */
class ManualLinkedList {
    friend class ManualLinkedListBase;
    ManualLinkedListBase& listBase;
    VertexIndex id;
    // The size of a block is stored in prev[id], as id is always the head of the block.

    // Flush the head of all vertices in the block to the current id.
    void flushHead();

    // ManualLinkedList should only be created by ManualLinkedListBase::newList().
    ManualLinkedList(ManualLinkedListBase& base, VertexIndex id)
    : listBase(base), id(id) { listBase.prev[id] = 0; listBase.next[id] = INVALID_VERTEX; }

public:

    ~ManualLinkedList() { listBase.recycleList(id); }

    VertexIndex begin() const { return listBase.next[id]; }

    constexpr VertexIndex end() const { return INVALID_VERTEX; }

    VertexIndex erase(VertexIndex v) { listBase.erase(v); }

    size_t size() const { return listBase.prev[id]; }

    // Prepare the block to be inserted into FrontierManager.
    void archive() { flushHead(); }

    // Add a vertex to the block.
    // If the vertex is already in another block, it will be removed from the old block.
    // If the vertex is already in this block, it will be ignored.
    void add(VertexIndex v);

    VertexIndex next(VertexIndex v) const { return listBase.next[v]; }
};