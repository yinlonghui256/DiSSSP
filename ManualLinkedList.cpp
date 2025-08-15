#include "ManualLinkedList.h"


VertexIndex ManualLinkedListBase::erase(VertexIndex v) {
    VertexIndex &headId = head[v], &nextId = next[v], &prevId = prev[v];
    // redirect the pointers of the neighbors.
    next[prevId] = nextId; // prev[v] should always be valid.
    if (nextId != NULL_VERTEX) { prev[nextId] = prevId; }
    else { head[headId] = prevId; } // v is the tail of the block, update the head of the block.
    // update size of the block that v belongs to.
    -- prev[headId];
    // clear the pointers of this vertex.
    headId = prevId = nextId = NULL_VERTEX;
    return nextId;
}


ManualLinkedList ManualLinkedListBase::newList() {
    VertexIndex newId;
    if (blockPool == NULL_VERTEX) {
        // No recycled block available. Create a new one.
        newId = prev.size();
        prev.resize(newId + 1);
        next.resize(newId + 1);
        head.resize(newId + 1);
    } else {
        // There is a recycled block available. Reuse it.
        newId = blockPool;
        blockPool = next[blockPool];        
    }
    return ManualLinkedList{*this, newId};
}


ManualLinkedList::~ManualLinkedList() {
    for (auto it : *this) { listBase.head[it] = NULL_VERTEX; }
    listBase.recycleList(id);
}


void ManualLinkedList::flushHead() {
    for (auto it : *this) { listBase.head[it] = id; }
}


void ManualLinkedList::add(VertexIndex v) {
    // The vertex is already in this block, no need to add it again.
    if (listBase.head[v] == id) { return; }

    // If the vertex is in another block, remove it from the old block.
    if (listBase.head[v] != NULL_VERTEX) { listBase.erase(v); }

    // New vertices are always inserted after the head of the block.
    VertexIndex &nextId = listBase.next[id];
    listBase.head[v] = id;
    listBase.prev[v] = id;
    listBase.next[v] = nextId;
    if (nextId != NULL_VERTEX) { listBase.prev[nextId] = v; }
    else { listBase.head[id] = v; } // v is the tail of the block.
    nextId = v;
    ++ listBase.prev[id]; // update size of the block.
}


void ManualLinkedList::merge(ManualLinkedList& other) {
    // If the other block is empty, do nothing.
    if (other.empty()) { return; }
    // If this block is empty, just take over the other block.
    if (empty()) { std::swap(id, other.id); return; }
    else {
        // Both blocks are non-empty. Merge them.
        VertexIndex thisTail = listBase.head[id];
        VertexIndex otherHead = listBase.next[other.id];
        VertexIndex otherTail = listBase.head[other.id];
        // Link the two blocks.
        listBase.next[thisTail] = otherHead;
        listBase.prev[otherHead] = thisTail;
        listBase.head[id] = otherTail;
        // Update the head of all vertices in the other block to this id.
        // Currently, other.begin() and other.end() are valid.
        for (auto it : other) { listBase.head[it] = id; }
        // Update size of the block.
        listBase.prev[id] += listBase.prev[other.id];
        // Clear the other block.
        listBase.prev[other.id] = 0;
        listBase.next[other.id] = listBase.head[other.id] = NULL_VERTEX;
    }
}
