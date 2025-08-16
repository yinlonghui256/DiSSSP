#include "FrontierManager.h"


void FrontierManager::insert(VertexIndex v){
    auto v_length = context.getDhat()[v];

    currentLowerBound = std::min(currentLowerBound, v_length);

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
    // First, try to extract more than M or all items, S0 from D0 and S1 from D1,
    // If |S0| + |S1| <= M: pull all items.
    // Otherwise, construct S = S0 \cup S1, and locate the (M + 1)-th smallest item x in S.
    // Split S0 = S0L (< x) \cup S0G (>= x), S1 = S1L (< x) \cup S1G (>= x).
    // Then output S0L \cup S1L as the result, and insert back S0G to D0 and S1G to D1.
    // Either S0G or S1G will be non-empty, update currentLowerBound to x.
    // Total time is linear to the output size, except the following two issues:
    
    // In extracting S1 and S0, it is possible that we encounter under-sized blocks (if their capacity < M/2).
    // In extreme cases we may even encouter empty blocks such that we may need to extract even more than M blocks.
    // In our insert and batch-prepend operation, we ensured that blocks added into D0 and D1 have normal size [M/2, M].
    // So under-sized blocks occur only when:
    // 1. if D0 / D1 contains only one block, then extracting them takes O(1) time.
    // 2. Some formerly inserted vertex was removed from that block,
    //    and we may amortize this extra time to that vertex's insertion / batch-prepend:
    //    because extracting is O(1) / O(log(N/M)) in D0 / D1 but insertion is O(max{1, log(L/M)}) / O(log(N/M)) per vertex.

    // Inserting back S1G may also cost constant insertion per pull, which is O(log(N/M)) time, as |S1G| = O(M).
    // If M > 1, then M >= 2^t; this is amortized O(log(N/M) / M) = o(1) per inserted item.
    // (Because O(1) extra insertion per pull, and there are at most N/M pulls, i.e., O(1/M) pull per item.)
    // If M == 1, we adjust the whole function to spend O(1) time. (Because std::map::begin() is O(1).)

    // These ensures that pull() is amortized to linear time per output size.

    if (M == 1) {
        // Find the first non-empty block in D0/D1.
        // For M=1, D1.erase() is called only for an empty block or the block to be pulled.
        while (!D0.empty()) {
            if (D0.front()->empty()) {
                D0.pop_front();
            } else {
                break;
            }
        }
        while (!D1.empty()) {
            if (D1.begin()->second->empty()) {
                D1.erase(D1.begin());
            } else {
                break;
            }
        }

        if (D0.empty() && D1.empty()) {
            D1[upperBound] = newBlock(upperBound);
            currentLowerBound = upperBound;
            // No more items to pull.
            return newBlock();
        }
        if (D0.empty()) {
            // Pull from D1.
            auto block = D1.begin()->second;
            D1.erase(D1.begin());
            // Remove empty block prefixes from D1.
            while (!D1.empty()) {
                if (D1.begin()->second->empty()) {
                    D1.erase(D1.begin());
                } else {
                    break;
                }
            }
            if (D1.empty()) {
                D1[upperBound] = newBlock(upperBound); // D1 needs a default block.
                currentLowerBound = upperBound;
            } else {
                currentLowerBound = context.getDhat()[*(D1.begin()->second->begin())];
            }
            return block;
        }
        if (D1.empty()) {
            D1[upperBound] = newBlock(upperBound); // D1 needs a default block.
            // Pull from D0.
            auto block = D0.front();
            D0.pop_front();
            // Remove empty block prefixes from D1.
            while (!D0.empty()) {
                if (D0.front()->empty()) {
                    D0.pop_front();
                } else {
                    break;
                }
            }
            if (D0.empty()) {
                currentLowerBound = upperBound;
            } else {
                currentLowerBound = context.getDhat()[*(D0.front()->begin())];
            }
            return block;
        }
        // Now both D0 and D1 are non-empty.
        auto length0 = context.getDhat()[*(D0.front()->begin())];
        auto length1 = context.getDhat()[*(D1.begin()->second->begin())];
        if (length0 < length1) {
            // Pull from D0.
            auto block = D0.front();
            D0.pop_front();

            while (!D0.empty()) {
                if (D0.front()->empty()) {
                    D0.pop_front();
                } else {
                    break;
                }
            }

            if (D0.empty()) {
                currentLowerBound = length1;
            } else {
                currentLowerBound = std::min(length1, context.getDhat()[*(D0.front()->begin())]);
            }
            return block;
        } else {
            // Pull from D1.
            auto block = D1.begin()->second;
            D1.erase(D1.begin());

            while (!D1.empty()) {
                if (D1.begin()->second->empty()) {
                    D1.erase(D1.begin());
                } else {
                    break;
                }
            }

            if (D1.empty()) {
                D1[upperBound] = newBlock(upperBound); // D1 needs a default block.
                currentLowerBound = length0;
            } else {
                currentLowerBound = std::min(length0, context.getDhat()[*(D1.begin()->second->begin())]);
            }
            return block;
        }
    } else {
        ShPBlock S0 = newBlock(upperBound, currentLowerBound);
        while (!D0.empty()) {
            S0 -> merge(* D0.front());
            D0.pop_front();
            if (S0 -> getSize() > M) { break; }
        }
        
        ShPBlock S1 = newBlock(upperBound, currentLowerBound);
        while (!D1.empty()) {
            S1 -> merge(D1.begin()->second);
            D1.erase(D1.begin());
            if (S1 -> getSize() > M) { break; }
        }

        if (S0 -> getSize() + S1 -> getSize() <= M) {
            // S0 / S1 must have extracted all items in D0 / D1.
            // Now both D0 and D1 become empty.
            D1[upperBound] = newBlock(upperBound); // D1 needs a default block.
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
        // Besides, |S0G| and |S1G| are bounded by 2M.
        // Now We insert them back into D0 / D1.

        // If S0 is under-sized, try to merge it with the front block in D0.
        // If D0 is empty, skip to next step.
        // Merge S0 to D0.front.
        // If D0.front is still under-sized, it is ok because it must have been under-sized before,
        // so pull() does not increase the number of under-sized blocks.
        // Otherwise, extract the smaller half to S0. Then both D0.front() and S0 become normal-sized.
        if (S0 -> getSize() < M/2 && !D0.empty()) {
            D0.front()->merge(*S0);
            if (D0.front()->overSized()) {
                S0 = D0.front()->splitAtMedian(context);
            }
        }

        // If S0 is over-sized, split it into two blocks.
        // If S0 is non-empty and normal-sized, insert it into D0.
        // Else, S0 is empty, do nothing.
        if (S0 -> overSized()) {
            auto smallerHalf = S0 -> splitAtMedian(context);
            D0.push_front(S0);
            D0.push_front(smallerHalf);
        } else if (S0 -> getSize()) {
            D0.push_front(S0);
        }

        // The logic for S1 is similar to S0.
        if (S1 -> getSize() < M/2 && !D1.empty()) {
            D1.begin()->second->merge(*S1);
            if (D1.begin()->second->overSized()) {
                S1 = D1.begin()->second->splitAtMedian(context);
            }
        }

        if (S1 -> overSized()) {
            auto smallerHalf = S1 -> splitAtMedian(context);
            D1[smallerHalf->getUpperBound()] = smallerHalf;
            D1[S1->getUpperBound()] = S1;
        } else if (S1 -> getSize()) {
            D1[S1->getUpperBound()] = S1;
        } else if (D1.empty()) {
            D1[upperBound] = newBlock(upperBound); // D1 needs a default block.
        }

        currentLowerBound = x;
        S0L->merge(*S1L);
        return S0L;
    }
}


