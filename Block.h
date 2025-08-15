#pragma once

#include <list>
#include <unordered_set>
#include "Length.h"
#include "GraphContext.h"


using BlockContainer = std::list<VertexIndex>;
using BlockIterator = BlockContainer::iterator;


/**
 * @brief A Block is an unsorted linked list of vertices, and the manage unit of FrontierManager.
 * It maintains an interval [lowerBound, upperBound) for vertices in this Block.
 * It is also designed to contain no more than M(=capacity) items (possibly empty). 
 * The constraints may be temporarily violated, but will be restored before added into FrontierManager.
 * 
 * It might be argued that why not using std::unordered_set?
 * In fact, we need item-removal by VertexIndex, where unordered_set has an advantage over linked lists,
 * for which we even added complicated VertexContext in FrontierManager to manage.
 * However, theoretically, unordered_set is not guaranteed to run in deterministic O(1) time per operation.
 */
class Block {

    Length lowerBound; //inclusive
    Length upperBound; // exclusive
    size_t capacity; // designed max number of items in this Block, typically M. 
    BlockContainer items;

    FrontierManager& frontierManager; // The FrontierManager that this Block belongs to.

public:

    Block(Length lower, Length upper, size_t cap = 0)
    : lowerBound(lower), upperBound(upper), capacity(cap) {
        if (lowerBound >= upperBound) { throw std::invalid_argument("Block lowerBound must be less than upperBound"); }
    }

    Block(Length lower, Length upper, size_t cap = 0, BlockContainer && its)
    : lowerBound(lower), upperBound(upper), capacity(cap), items(std::move(its)) {
        if (lowerBound >= upperBound) { throw std::invalid_argument("Block lowerBound must be less than upperBound"); }
    }

    Block(BlockContainer && its) : items(std::move(its)) {}

    size_t getSize() const { return items.size(); }

    Length getLowerBound() const {return lowerBound; }

    Length getUpperBound() const {return upperBound; }

    size_t getCapacity() const { return capacity; }

    bool overSized() const { return items.size() > capacity; }

    bool suit(Length length) const { return length >= lowerBound && length < upperBound; }

    // Add an item to this Block, return the iterator pointing to the item.
    // If the Block is oversized, it will be split into two Blocks in frontierManager.
    BlockIterator addItem(VertexIndex item) { return items.insert(items.end(), item); }

    void removeItem(BlockIterator it) { items.erase(it); }

    size_t countNoGreater(const GraphContext& context, Length threshold) const;

    // Extracts all items in this Block that are less than the threshold to form a new Block.
    // The original Block is modified to remove these items.
    std::shared_ptr<Block> extractLessThan(const GraphContext& context, Length threshold);

    // Extracts the smallest q items to form a new Block.
    // The original Block is modified to remove these items.
    std::shared_ptr<Block> extractMinQ(const GraphContext& context, size_t q) { return extractLessThan(context, locateMinQ(context, q + 1)); }

    // Find the q-th smallest item in this Block.
    // This function runs in linear time.
    Length locateMinQ(const GraphContext& context, size_t q) const ;

    // Split this Block into two Blocks by median.
    // This block would hold the larger half, and the function returns the smaller half.
    std::shared_ptr<Block> splitAtMedian(const GraphContext& context) { return extractMinQ(context, items.size() / 2); }

};
