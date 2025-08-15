#pragma once

#include <list>
#include <map>
#include "Length.h"
#include "Block.h"


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

    GraphContext& context; // The graph context that contains the graph and the dhat array.

    std::list<ShPBlock> D0;   // D_0 in the paper
    std::map<Length, ShPBlock> D1; // D_1 in the paper
    size_t M; // M in the paper, default capacity of the Blocks.
    Length upperBound; // B in the paper, upper bound for all inserted items.
    Length currentLowerBound; // B_i in the paper, the latest lower bound of all items in D0 and D1.

    public:

    FrontierManager(GraphContext& ctx, size_t m, Length ub)
    : context(ctx), M(m), upperBound(ub), currentLowerBound(Length::zero()) {
        D1[upperBound] = newBlock();
    }

    // Insert a vertex into the FrontierManager.
    // If the vertex's length exceeds upperBound, it will be ignored.
    // If the block it is inserted into is oversized, it will be split into two blocks.
    // If it is currently in another block, it will first be removed from the old block.
    void insert(VertexIndex v);

    // Batch insert.
    void insert(UList vertices) { for (auto v : *vertices) { insert(v); } }

    // Batch-prepend a block of vertices into D0.
    // Caller ensure pBlock->upperBound <= currentLowerBound. 
    void batchPrepend(ShPBlock pBlock);

    ShPBlock newBlock() {
        return std::make_shared<Block>(context.newList(), Length::zero(), upperBound, M);
    }

    Length getCurrentLowerBound() const { return currentLowerBound; }
    
    // Pull smallest M vertices from D0 and D1 and update currentLowerBound.
    // If there are no more tham M vertices, all vertices are pulled.
    // If it contains no vertices, pull an empty block.
    // Because the size of the blocks may decrease outside FrontierManager,
    // Emptyness is only checked when trying to pull.
    ShPBlock pull();
};
