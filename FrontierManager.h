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

    std::list<std::shared_ptr<Block>> D0;   // D_0 in the paper
    std::map<Length, std::shared_ptr<Block>> D1; // D_1 in the paper
    size_t M; // M in the paper, default capacity of the Blocks.
    Length upperBound; // B in the paper, upper bound for all inserted items.

    public:

    void insert(VertexIndex v);

    void batchPrepend(std::shared_ptr<Block> block);

    

};
