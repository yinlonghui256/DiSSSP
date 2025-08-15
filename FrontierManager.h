#pragma once

#include <list>
#include <map>
#include "Length.h"
#include "Block.h"


using BlockContainer = std::list<VertexIndex>;
using BlockIterator = BlockContainer::iterator;

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

    // When a vertex is inserted a second time into the frontier manager,
    // as dhat is always monotonically non-increasing,
    // the old occurrence (if exists) needs to be removed.
    // This information is stored in vertexContexts.
    struct VertexContext {
        bool valid;
        std::weak_ptr<Block> block; // The block that contains this vertex.
        BlockIterator it; // The iterator pointing to this vertex in the block.
        VertexContext() : valid(false), block(), it() {}
        VertexContext(std::shared_ptr<Block> b, BlockIterator i) : valid(true), block(b), it(i) {}
        void remove() { if (!valid) return; block.lock()->removeItem(it); valid = false; }
        void update(std::shared_ptr<Block> b, BlockIterator i) { remove(); block = b; it = i; valid = true; }
        bool check(std::shared_ptr<Block> b) const { return valid && block.lock() != b; }
    };

    std::vector<VertexContext> vertexContexts; // Contexts for all vertices.

    GraphContext& context; // The graph context that contains the graph and the dhat array.

    std::list<std::shared_ptr<Block>> D0;   // D_0 in the paper
    std::map<Length, std::shared_ptr<Block>> D1; // D_1 in the paper
    size_t M; // M in the paper, default capacity of the Blocks.
    Length upperBound; // B in the paper, upper bound for all inserted items.

    public:

    bool checkVertexContext(VertexIndex v, const std::shared_ptr<Block>& block) const {
        return vertexContexts[v].check(block); 
    }

    void updateVertexContext(VertexIndex v, const std::shared_ptr<Block>& block, BlockIterator it) {
        vertexContexts[v].update(block, it);
    }

    void insert(VertexIndex v);

    void batchPrepend(std::shared_ptr<Block> block);



};
