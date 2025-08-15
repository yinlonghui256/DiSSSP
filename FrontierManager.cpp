#include "FrontierManager.h"


void FrontierManager::insert(VertexIndex v){
    auto v_length = context.getDhat()[v];

    if (v_length >= upperBound) { return; } // Ignore items that exceed the upper bound.

    // Find the appropriate block in D1 to insert the item.
    // If the algorithm is correct, there should always be a suitable block.
    auto it = D1.upper_bound(v_length);
    if (it != D1.end() && it->second->suit(v_length)) {
        if (vertexContexts[v].valid) {
            auto oldBlock = vertexContexts[v].block.lock();
            if (oldBlock && oldBlock != it->second) {
                // If the old block is not the same as the new one, remove the old occurrence.
                    oldBlock->removeItem(vertexContexts[v].it);
                } 
            }
        if (it->second->overSized()) {
            // If the block is oversized, it will be split into two blocks.
            auto newBlock = it->second->splitAtMedian(context);
            D1[newBlock->getUpperBound()] = newBlock;
        }
    } else {
        throw std::logic_error("No suitable block found for the item in FrontierManager::insert.");
    }
}