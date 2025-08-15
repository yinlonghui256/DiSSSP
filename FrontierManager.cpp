#include "FrontierManager.h"


void FrontierManager::insert(VertexIndex v){
    auto v_length = context.getDhat()[v];

    if (v_length >= upperBound) { return; } // Ignore items that exceed the upper bound.

    // Find the appropriate block in D1 to insert the item.
    // If the algorithm is correct, there should always be a suitable block.
    auto it = D1.upper_bound(v_length);
    if (it != D1.end() && it->second->suit(v_length)) {
        it->second->addItem(v);
        if (it->second->overSized()) {
            // If the block is oversized, it will be split into two blocks.
            auto newBlock = it->second->splitAtMedian(context);
            D1[newBlock->getUpperBound()] = newBlock;
        }
    } else {
        throw std::logic_error("No suitable block found for the item in FrontierManager::insert.");
    }
}


void FrontierManager::batchPrepend(ShPBlock pBlock) {
    if (currentLowerBound < pBlock->getUpperBound()) {
        throw std::logic_error("pBlock upperBound exceeds currentLowerBound in FrontierManager::batchPrepend.");
    }

    // If the block is oversized, it will be split into O(L/M) blocks of size <= M.
    if (pBlock->overSized()) {
        auto smallerHalf = pBlock->splitAtMedian(context);
        batchPrepend(pBlock);
        batchPrepend(smallerHalf);
    } else {
        D0.push_front(pBlock);
    }
}


ShPBlock FrontierManager::pull() {
    // First, try to extract at least M or all items, S0 from D0 and S1 from D1,
    // If |S0| + |S1| <= M: pull all items.
    // Otherwise, construct S = S0 \cup S1, and locate the M-th smallest item x in S.
    // Split S0 = S0L (<= x) \cup S0G (>x), S1 = S1L (<= x) \cup S1G (>x).
    // Then output S0L \cup S1L as the result, and insert back S0G to D0 and S1G to D1.
    // Either S0G or S1G will be non-empty, update their lower bound to currentLowerBound.
    // Total time is linear to the output size.
    // The insertion time of S1G is O(log(N/M)/M) = O(1) per item, as M >= 2^t > O(t + log k) = log(N/M).

}


