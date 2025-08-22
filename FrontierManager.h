#pragma once


#include "Block.h"


class BMSSP; // Forward declaration to avoid circular dependency.


/**
 * @brief FrontierManager is the data structure mentioned in lemma 3.3 in the paper.
 * It maintians "frontier" vertices into Blocks and supports Insert, Batch-prepend, and Pull operations.
 * 
 * Its inner implementation is a linked list(D0) and a binary search tree(D1) of blocks.
 * D0 is block-wise monotone and receives input of batch-prepend.
 * D1 is block-wise sorted by their upper bounds as keys.
 * The intervals are disjoint and their union is the whole range of lengths of interest.
 */
class FrontierManager {

    BMSSP& context; // The graph context that contains the graph and the dhat array.

    std::forward_list<ShpBlock> D0;   // D_0 in the paper
    std::map<Length, ShpBlock> D1; // D_1 in the paper
    size_t M; // M in the paper, default capacity of the Blocks.
    Length upperBound; // B in the paper, upper bound for all inserted items.
    Length currentLowerBound; // B_i(B_i') in the paper, the latest lower bound of all items.
    // changes its value only when pull() or batchPrepend() is called or P (in the paper) is inserted.
    // immediatedly after each pull, equals to B_i in the paper.
    // immediatedly after all insertion and batch-prepend, equals to B_i' in the paper.
    // Before the first pull, initialized to B.
    // if P is empty, then B_0 = B.
    // else, update to min length of P, i.e., B_0 as in the paper.

    // Called when a vertex is to be inserted but D1 is empty.
    // Caller to check whether the above conditions are satisfied.
    void addDefaultBlock() { D1[upperBound] = newBlock(upperBound); }

    // Clear empty blocks in D1's prefix, until D1.front() is non-empty or D1 is empty.
    // If D1.front() is non-empty, return true.
    // If D1 is empty, return false.
    bool clearEmptyPrefixD1();

    // Similar to switchD1FrontNonEmpty(), but for D0.
    bool clearEmptyPrefixD0();

    // merely used for debugging.
    void sanityCheckD1();

public:

    FrontierManager(BMSSP& ctx, size_t m, Length ub)
    : context(ctx), M(m), upperBound(ub), currentLowerBound(ub) {
		DEBUG_FRONTIER_LOG("Constructing FrontierManager with M = " << M << ", upperBound = " << upperBound);
    }

    // Insert a vertex into the FrontierManager.
    // If the vertex's length exceeds upperBound, it will be ignored.
    // If the block it is inserted into is oversized, it will be split into two blocks.
    // If it is currently in another block, it will first be removed from the old block, and then insert.
    void insert(VertexIndex v);

    // Batch insert.
    void insert(UList vertices) { for (auto v : *vertices) { insert(v); } }

    // Batch-prepend a block of vertices into D0.
    // Caller ensure pBlock->upperBound <= currentLowerBound. 
    void batchPrepend(ShpBlock pBlock);

    ShpBlock newBlock(Length ub = Length::infinity(), Length lb = Length::zero());

    Length getCurrentLowerBound() const { return currentLowerBound; }
    
    // Pull smallest M vertices from D0 and D1 and currentLowerBound up to date.
    // If there are no more tham M vertices, all vertices are pulled.
    // If it contains no vertices, pull an empty block.
    // Because the size of the blocks may decrease outside FrontierManager,
    // Emptyness is only checked when trying to pull.
    // If both D0 and D1 are empty, will return an empty block.
    std::pair<Length, ShpBlock> pull();
};
