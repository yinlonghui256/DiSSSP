#pragma once

#include <list>
#include <unordered_set>
#include "Length.h"
#include "GraphContext.h"


using BlockContainer = ManualLinkedList;
// using BlockIterator = BlockContainer::iterator;


/**
 * @brief A Block is an unsorted linked list of vertices, and the manage unit of FrontierManager.
 * It maintains an interval [lowerBound, upperBound) for vertices in this Block.
 * It is also designed to contain no more than M(=capacity) items (possibly empty). 
 * The constraints may be temporarily violated, but will be restored before added into FrontierManager.
 * 
 * It might be argued that why not using std::unordered_set?
 * Though it may work very well in practice, even better than our implementation,
 * unordered_set is not guaranteed to run in deterministic O(1) time per operation.
 */
class Block {

    BlockContainer items;
    Length lowerBound; //inclusive
    Length upperBound; // exclusive
    size_t capacity; // designed max number of items in this Block, typically M. 

public:

    Block(BlockContainer&& its, Length lower, Length upper, size_t cap = 0)
    : items(std::move(its)), lowerBound(lower), upperBound(upper), capacity(cap) {
        if (lowerBound >= upperBound) { throw std::invalid_argument("Block lowerBound must be less than upperBound"); }
    }

    Block(BlockContainer&& its) : items(std::move(its)) {}

    size_t getSize() const { return items.size(); }

    Length getLowerBound() const {return lowerBound; }

    Length getUpperBound() const {return upperBound; }

    size_t getCapacity() const { return capacity; }

    bool overSized() const { return items.size() > capacity; }

    bool suit(Length length) const { return length >= lowerBound && length < upperBound; }

    // Add an vertex to this Block.
    // If the Block is oversized, it will be split into two Blocks in frontierManager.
    void addItem(VertexIndex v) { items.add(v); }

    VertexIndex removeItem(VertexIndex v) { return items.erase(v); }

    size_t countNoGreater(const GraphContext& context, Length threshold) const;

    // Extracts all items in this Block that are less than the threshold to form a new Block.
    // The original Block is modified to remove these items.
    std::shared_ptr<Block> extractLessThan(GraphContext& context, Length threshold);

    // Extracts the smallest q items to form a new Block.
    // The original Block is modified to remove these items.
    std::shared_ptr<Block> extractMinQ(GraphContext& context, size_t q) { return extractLessThan(context, locateMinQ(context, q + 1)); }

    // Find the q-th smallest item in this Block.
    // This function runs in linear time.
    Length locateMinQ(const GraphContext& context, size_t q) const ;

    // Split this Block into two Blocks by median.
    // This block would hold the larger half, and the function returns the smaller half.
    std::shared_ptr<Block> splitAtMedian(GraphContext& context) { return extractMinQ(context, items.size() / 2); }

};
