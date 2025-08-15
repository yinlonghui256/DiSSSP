#include "ManualLinkedList.h"

VertexIndex ManualLinkedListBase::erase(VertexIndex v) {
    VertexIndex &headId = head[v], &nextId = next[v], &prevId = prev[v];
    // redirect the pointers of the neighbors.
    next[prevId] = nextId; // prev[v] of any vertex should be valid.
    if (nextId != INVALID_VERTEX) { prev[nextId] = prevId; }
    // update size of the block that v belongs to.
    -- prev[headId]; 
    // clear the pointers of this vertex.
    headId = prevId = nextId = INVALID_VERTEX;
    return nextId;
}

ManualLinkedList ManualLinkedListBase::newList() {
    VertexIndex newId = next[blockPool];
    if (newId != INVALID_VERTEX) {
        // There is a recycled block available. Reuse it.
        next[blockPool] = next[newId];
    } else {
        // No recycled block available. Create a new one.
        newId = prev.size();
        prev.resize(newId + 1);
        next.resize(newId + 1);
    }
    return ManualLinkedList{*this, newId};
}


void ManualLinkedList::flushHead() {
    for (auto it = begin(); it != end(); it = next(it)) {
        listBase.head[it] = id;
    }
}

void ManualLinkedList::add(VertexIndex v) {
    // The vertex is already in this block, no need to add it again.
    if (listBase.head[v] == id) { return; }

    // If the vertex is in another block, remove it from the old block.
    if (listBase.head[v] != INVALID_VERTEX) { listBase.erase(v); }

    // New vertices are always inserted at the head of the block.
    VertexIndex &nextId = listBase.next[id];
    listBase.head[v] = id;
    listBase.prev[v] = id;
    listBase.next[v] = nextId;
    if (nextId != INVALID_VERTEX) {
        listBase.prev[nextId] = v;
    }
    nextId = v;
    ++ listBase.prev[id]; // update size of the block.
}