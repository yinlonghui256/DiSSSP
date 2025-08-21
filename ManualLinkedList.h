#pragma once


#include "types.h"


/**
 * @brief ManualLinkedListBase holds the memory base of the linked lists.
 * In this problem, blocks are maintained as linked lists, and blocks are disjoint.
 * Therefore, we can integrate them together by two vectors: prev and next.
 * And more importantly, we need to support the removal of vertices by their value,
 * which it is not very convenient to do with a std::list.
 * 
 * 0 ~ n-1 are for the vertices, from above n are for the blocks.
 * blocks are assigned to dynamic id but larger than n.
 * The head array stores the head of the block each vertex belongs to.
 */

class ManualLinkedListBase : public std::enable_shared_from_this<ManualLinkedListBase> {
    friend class ManualLinkedList;

    // stores the previous vertex in the linked list.
    // for block heads, stores the size of the block.
    std::vector<VertexIndex> prev;

    // stores the next vertex in the linked list.
    // for block tails, equals to INVALID_VERTEX to mark itself the end of the block.
    std::vector<VertexIndex> next;

    // stores the head of the block each vertex belongs to.
    // if head[v] == INVALID_VERTEX, then v is not in any block.
    // for BLOCK heads, stores the TAIL of the block.
    std::vector<VertexIndex> head;

    VertexIndex blockPool; // A special linked list to keep track of the recycled blocks.
    // Maintained as a forward list.

    // Remove a vertex from its current linked list.
    // Return the VertexIndex at the next position.
    // caller responsible to check whether the vertex is in some block.
    VertexIndex erase(VertexIndex v);

    // Recycle a linked list block.
    // This is called when a ManualLinkedList is destructed.
    void recycleList(VertexIndex id);

    public:
        ManualLinkedListBase(size_t s)
            : prev(s, NULL_VERTEX), next(s, NULL_VERTEX), head(s, NULL_VERTEX), blockPool(NULL_VERTEX) {
            DEBUG_MLL_LOG("Constructing ManualLinkedListBase of size: " << s);
        }

    // Create a new linked list.
    ManualLinkedList newList();
    
    void debugPrint() const;
};

/**
 * @brief ManualLinkedList is a linked list of vertices.
 * It does not own the memory, so it can be copied and moved at a low cost.
 * Each ManualLinkedList serves for a block in FrontierManager.
 */
class ManualLinkedList {
    friend class ManualLinkedListBase;
    std::weak_ptr<ManualLinkedListBase> wpListBase;
    VertexIndex id;
    // The size of a block is stored in prev[id], as id is always the head of the block.

    // Flush the head of all vertices in the block to the current id.
    void flushHead();

    // ManualLinkedList should only be created by ManualLinkedListBase::newList().
    ManualLinkedList(std::shared_ptr<ManualLinkedListBase> base, VertexIndex id)
        : wpListBase(base), id(id) {
        base->prev[id] = 0; base->next[id] = base->head[id] = NULL_VERTEX;
        DEBUG_MLL_LOG("Constructing ManualLinkedList of id: " << id);
		base->debugPrint();
    }

    public:
    
    // ManualLinkedList is so small that copying it is cheap.
    ManualLinkedList(ManualLinkedList&& other) = default;
    ManualLinkedList& operator=(ManualLinkedList&& other) = default;
    ManualLinkedList(const ManualLinkedList& other) = default;
    ManualLinkedList& operator=(const ManualLinkedList& other) = default;

    ~ManualLinkedList();

    class Iterator {
        const ManualLinkedList& list;
        VertexIndex current;
    public:
        constexpr Iterator(const ManualLinkedList& lst, VertexIndex start) : list(lst), current(start) {}
        Iterator(Iterator&& other) = delete;
        Iterator& operator=(ManualLinkedList::Iterator &&) = delete;
        Iterator(const Iterator& other) = default;
        Iterator& operator=(const ManualLinkedList::Iterator &) = default;
        VertexIndex operator*() const { return current; }
        Iterator& operator++() { current = list.wpListBase.lock()->next[current]; return *this; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

	VertexIndex getId() const { return id; }

    bool empty() const { return wpListBase.lock()->prev[id] == 0; }

    Iterator begin() const { return Iterator{*this, wpListBase.lock()->next[id]}; }

    constexpr Iterator end() const { return Iterator{*this, NULL_VERTEX}; }

    size_t size() const { return wpListBase.lock()->prev[id]; }

    // Prepare the block to be inserted into FrontierManager.
    void archive() { flushHead(); }

    // Add a vertex to this linked list.
    // If the vertex is already in another block, it will be removed from the old block.
    // If the vertex is already in this block, it will be ignored.
    // return if the vertex 
    void add(VertexIndex v);

    // Remove a vertex from its current linked list.
    // Return the VertexIndex at the next position.
    // caller responsible to check whether the vertex is in some block.
    void erase(VertexIndex v) { wpListBase.lock()->erase(v); }

    VertexIndex next(VertexIndex v) const { return wpListBase.lock()->next[v]; }
    
    // Merge another linked list into this linked list.
    // The other linked list will be empty after the merge.
    // The complexity is linear to the size of the other linked list.
    void merge(ManualLinkedList& other);

    void debugPrint() const;
};