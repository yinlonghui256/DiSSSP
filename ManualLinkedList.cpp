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
    return ManualLinkedList{shared_from_this(), newId};
}


ManualLinkedList::~ManualLinkedList() {
    auto pListBase = wpListBase.lock();
    for (auto it : *this) { pListBase->head[it] = NULL_VERTEX; }
    pListBase->recycleList(id);
}


void ManualLinkedList::flushHead() {
    for (auto it : *this) { wpListBase.lock()->head[it] = id; }
}


void ManualLinkedList::add(VertexIndex v) {
    auto pListBase = wpListBase.lock();
    // The vertex is already in this block, no need to add it again.
    if (pListBase->head[v] == id) { return; }

    // If the vertex is in another block, remove it from the old block.
    if (pListBase->head[v] != NULL_VERTEX) { pListBase->erase(v); }

    // New vertices are always inserted after the head of the block.
    VertexIndex &nextId = pListBase->next[id];
    pListBase->head[v] = id;
    pListBase->prev[v] = id;
    pListBase->next[v] = nextId;
    if (nextId != NULL_VERTEX) { pListBase->prev[nextId] = v; }
    else { pListBase->head[id] = v; } // v is the tail of the block.
    nextId = v;
    ++ pListBase->prev[id]; // update size of the block.
}


void ManualLinkedList::merge(ManualLinkedList& other) {
    auto pListBase = wpListBase.lock();
    // If the other block is empty, do nothing.
    if (other.empty()) { return; }
    // If this block is empty, just take over the other block.
    if (empty()) { std::swap(id, other.id); return; }
    else {
        // Both blocks are non-empty. Merge them.
        VertexIndex thisTail = pListBase->head[id];
        VertexIndex otherHead = pListBase->next[other.id];
        VertexIndex otherTail = pListBase->head[other.id];
        // Link the two blocks.
        pListBase->next[thisTail] = otherHead;
        pListBase->prev[otherHead] = thisTail;
        pListBase->head[id] = otherTail;
        // Update the head of all vertices in the other block to this id.
        // Currently, other.begin() and other.end() are valid.
        for (auto it : other) { pListBase->head[it] = id; }
        // Update size of the block.
        pListBase->prev[id] += pListBase->prev[other.id];
        // Clear the other block.
        pListBase->prev[other.id] = 0;
        pListBase->next[other.id] = pListBase->head[other.id] = NULL_VERTEX;
    }
}
