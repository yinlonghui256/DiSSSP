#include "FrontierManager.h"


bool FrontierManager::clearEmptyPrefixD1() {
    while (!D1.empty()) {
        if (D1.begin()->second->empty()) {
            D1.erase(D1.begin());
        } else {
            return true; // D1.front() is non-empty.
        }
    }
    return false; // D1 is empty.
}

bool FrontierManager::clearEmptyPrefixD0() {
    while (!D0.empty()) {
        if (D0.front()->empty()) {
            D0.pop_front();
        } else {
            return true; // D0.front() is non-empty.
        }
    }
    return false; // D0 is empty.
}

void FrontierManager::insert(VertexIndex v){
    auto v_length = context.getDhat()[v];

    currentLowerBound = std::min(currentLowerBound, v_length);

    if (v_length >= upperBound) { return; } // Ignore items that exceed the upper bound.

    if (D1.empty()) {
        // If D1 is empty, we need to add a default block.
        addDefaultBlock();
    }

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

    if (pBlock -> empty()) { return; } // Ignore empty blocks.

    // If the block is oversized, it will be split into O(L/M) blocks of size <= M.
    if (pBlock->overSized()) {
        auto smallerHalf = pBlock->splitAtMedian(context);
        batchPrepend(pBlock);
        batchPrepend(smallerHalf);
    } else {
        D0.push_front(pBlock);
    }
}


// First, try to extract more than M or all items, S0 from D0 and S1 from D1,
// If |S0| + |S1| <= M: pull all items.
// Otherwise, construct S = S0 \cup S1, and locate the (M + 1)-th smallest item x in S.
// Split S0 = S0L (< x) \cup S0G (>= x), S1 = S1L (< x) \cup S1G (>= x).
// Then output S0L \cup S1L as the result, and insert back S0G to D0 and S1G to D1.
// Either S0G or S1G will be non-empty, update currentLowerBound to x.
// Total time is linear to the output size, except the following two issues:

// 1) In extracting S1 and S0, we may encounter under-sized blocks (if their capacity < M/2).
// Extracting blocks in S0 is O(1) and merging blocks is linear, so it is ok with D0.
// We need to quantify the number of additional insertions requested on S1.
// In our insert operation, we tried to ensure blocks added into D1 have normal size [M/2, M].
// So under-sized blocks occur in D1 only when:
// 1. D1 contains only one block, then extracting it takes O(1) time.
// 2. Some formerly inserted vertex was removed from that block.
//    Then we amortize the extraction time of an under-size block to that vertex's insertion:
//    because extracting is O(log(N/M)) in D1 and insertion is O(log(N/M)) per vertex.

// 2) Inserting back S1G may also cost O(1) insertion per pull.
// If there exists one vertex in the output that comes from D1,
// we amortize these O(1) insertion time to that vertex's insertion.
// So we divide the problem into two cases:
// 1. M-th element of S0 is smaller than current minimal element of D1.front() or D1 is empty.
//    The output will contain no vertex from D1, but we do not extract any block from D1.
// 2. M-th element of S0 is greater than current minimal element of D1.front().
//    The output will contain at least one vertex from D1, it is ok to insert S1G back to D1.

// Note that the above arguments hold even if M == 1.
// These ensures that pull() is amortized to linear time per output size.
ShPBlock FrontierManager::pull() {

    ShPBlock S0 = newBlock(upperBound, currentLowerBound);
    while (!D0.empty()) {
        S0 -> merge(* D0.front());
        D0.pop_front();
        if (S0 -> getSize() > M) { break; }
    }

    if (S0 -> getSize() >= M) {
        Length S0Mth = S0 -> locateMinQ(context, M);
        Length D1min = clearEmptyPrefixD1() ? D1.begin() -> second -> min(context) : upperBound;
        if (S0Mth < D1min) {
            // Case 1: output contains no vertex from D1, and extract no block from D1.
            auto S0L = S0 -> extracetLessThanOrEqual(context, S0Mth);
            Length S0Gmin = S0 -> min(context);
            // Now S0 becomes S0G, and |S0L| == M.
            currentLowerBound = std::min(D1min, S0Gmin);
            if (!S0 -> empty()) {
                // If S0 is non-empty, insert it back to D0.
                // Note that now |S0| <= M.
                D0.push_front(S0);
            }
            return S0L;
        }
    }

    // Case 2: output contains some vertex from D1, we can do O(1) extraction/insertion on D1.
    ShPBlock S1 = newBlock(upperBound, currentLowerBound);
    while (!D1.empty()) {
        S1 -> merge(D1.begin()->second);
        D1.erase(D1.begin());
        if (S1 -> getSize() > M) { break; }
    }

    if (S0 -> getSize() + S1 -> getSize() <= M) {
        // S0 / S1 must have extracted all items in D0 / D1.
        // Now both D0 and D1 become empty.
        S0 -> merge(*S1);
        currentLowerBound = upperBound;
        return S0;
    }

    // We are able to pull M items, so we can spend O(M) time.
    // Find the (M + 1)-th smallest item x in S0 and S1.
    // We cannot merge S0 and S1 because this may corrupt their linked list structure.
    std::vector<Length> cache;
    cache.reserve(S0 -> getSize() + S1 -> getSize());
    for (auto it : *S0) { cache.emplace_back(context.getDhat()[it]); }
    for (auto it : *S1) { cache.emplace_back(context.getDhat()[it]); }

    Length x = linearLocateMinQ(cache, M + 1);

    auto S0L = S0 -> extractLessThan(context, x);
    auto S1L = S1 -> extractLessThan(context, x);

    // Now S0 becomes S0G and S1 becomes S1G, and |S0L| + |S1L| == M.
    // S0G/S1G is empty if and only if D0/D1 is now empty.
    // And they cannot both be empty, so x is the new currentLowerBound.
    // Besides, |S0| and |S1| are bounded by 2M.
    // Now We insert them back into D0 / D1.

    // If S0 is non-empty, batch-prepend it to D0.
    // Note that |S0G| <= 2M, this takes O(M) time. 
    // In D0, we do not need to intentionally avoid under-sized blocks,
    // because we can extract them in O(1) time.
    // We only need to ensure not to insert an empty block.
    if (S0 -> overSized()) {
        auto smallerHalf = S0 -> splitAtMedian(context);
        D0.push_front(S0);
        D0.push_front(smallerHalf);
    } else if (!S0 -> empty()) {
        D0.push_front(S0);
    }

    // For S1, we need to ensure inserting it back does not increase under-sized blocks in D1.
    // If S1 is under-sized and D1 is non-empty, merge it to the first block in D1.
    // If this makes the first block in D1 oversized, let S1 take over the smaller half.
    if (S1 -> underSized() && !D1.empty()) {
        D1.begin()->second->merge(*S1);
        if (D1.begin()->second->overSized()) {
            S1 = D1.begin()->second->splitAtMedian(context);
        }
    }

    // Now D1 is empty, or S1 is normal/over-sized.
    // If S1 is now empty then D1 is empty. In this case, nothing to do.
    // If S1 is over-sized(but still <= 2M), we split it into two blocks and insert into D1.
    // If S1 is normal-sized, we directly insert it into D1.
    if (S1 -> overSized()) {
        auto smallerHalf = S1 -> splitAtMedian(context);
        D1[smallerHalf->getUpperBound()] = smallerHalf;
        D1[S1->getUpperBound()] = S1;
    } else if (!S1 -> empty()) {
        D1[S1->getUpperBound()] = S1;
    }

    currentLowerBound = x;
    S0L->merge(*S1L);
    return S0L;
}

