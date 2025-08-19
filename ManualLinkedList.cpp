#include "ManualLinkedList.h"


VertexIndex ManualLinkedListBase::erase(VertexIndex v) {
    VertexIndex &headId = head[v], &nextId = next[v], &prevId = prev[v];
    DEBUG_MLL_LOG("Erasing vertex " << v << " from its old ManualLinkedList of id " << headId);
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


void ManualLinkedListBase::recycleList(VertexIndex id) {
    DEBUG_MLL_LOG("Recycling ManualLinkedList with id " << id);
    // Add the block to the block pool.
    next[id] = blockPool;
    blockPool = id;
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
    DEBUG_MLL_LOG("Creating a new ManualLinkedList; newId: " << newId);
    return ManualLinkedList{shared_from_this(), newId};
}


void ManualLinkedListBase::debugPrint() const {
#ifdef DEBUG_MLL
    std::cout << "ManualLinkedListBase state:" << std::endl;
    std::cout << "indx: \t";
    for (auto i = 0; i < head.size(); ++i) { std::cout << (i == blockPool ? "B" : std::to_string(i)) << " \t"; }
    std::cout << std::endl;
    std::cout << "prev: \t";
    for (const auto& p : prev) { std::cout << (p == NULL_VERTEX ? "N" : std::to_string(p)) << " \t"; }
    std::cout << std::endl;
    std::cout << "next: \t";
    for (const auto& n : next) { std::cout << (n == NULL_VERTEX ? "N" : std::to_string(n)) << " \t"; }
    std::cout << std::endl;
    std::cout << "head: \t";
    for (const auto& h : head) { std::cout << (h == NULL_VERTEX ? "N" : std::to_string(h)) << " \t"; }
    std::cout << std::endl;
#endif // DEBUG
}


ManualLinkedList::~ManualLinkedList() {
    auto pListBase = wpListBase.lock();
    if (!pListBase) {
        DEBUG_MLL_LOG("Temporary ManualLinkedList object with id " << id << " is being destructed.");
        return;
    }
    DEBUG_MLL_LOG("Real ManualLinkedList object with id " << id << " is being destructed.");
    for (auto it : *this) { pListBase->head[it] = NULL_VERTEX; }
    pListBase->recycleList(id);
}


void ManualLinkedList::flushHead() {
    for (auto it : *this) { wpListBase.lock()->head[it] = id; }
}


void ManualLinkedList::add(VertexIndex v) {
    DEBUG_MLL_LOG("Adding vertex " << v << " to ManualLinkedList with id " << id);
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
    DEBUG_MLL_LOG("Merging linked list of id " << other.id << " into the tail of linked list of id " << id);
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


void ManualLinkedList::debugPrint() const {
#ifdef DEBUG_MLL
    auto pListBase = wpListBase.lock();
    std::cout << "ManualLinkedList state for id " << id << ":" << std::endl;
    std::cout << "  size: " << pListBase->prev[id] << std::endl;
    std::cout << "  list: ";
    for (auto it : *this) { std::cout << it << " ->\t"; }
    std::cout << std::endl;
    std::cout << "  tail: " << pListBase->head[id] << std::endl;
#endif
}