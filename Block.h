#pragma once

#include "Length.h"
#include "ManualLinkedList.h"


using BlockContainer = ManualLinkedList;
// renaming this because we had multiple alternative implementations.
// Just for smoother transition.
using ShpBlock = std::shared_ptr<Block>;

/**
 * @brief A Block is an unsorted linked list of vertices, and the manage unit of FrontierManager.
 * It maintains an interval [lowerBound, upperBound) for vertices in this Block.
 * The upperBound, once initialized, will never be changed, until deconstructed.
 * It is also designed to contain no more than M(=capacity) items (possibly empty). 
 * The constraints may be temporarily violated, but will be restored before added into FrontierManager.
 * 
 * It might be argued that why not using std::unordered_set?
 * Though it may work very well in practice, even better than our implementation,
 * unordered_set is not guaranteed to run in deterministic O(1) time per operation.
 */
class Block {

    BlockContainer items;
    Length upperBound; // exclusive
    Length lowerBound; //inclusive
    size_t capacity; // designed max number of items in this Block, typically M. 

public:

    Block(BlockContainer&& its, Length upper, Length lower, size_t cap = 0)
    : items(std::move(its)), upperBound(upper), lowerBound(lower), capacity(cap) {
        if (upperBound < lowerBound) { throw std::invalid_argument("Block lowerBound must be less than upperBound"); }
    }

    Block(Block&& other) = default;
    Block& operator=(Block&& other) = default;
    Block(const Block& other) = default;
    Block& operator=(const Block& other) = default;

    size_t getSize() const { return items.size(); }

    Length getUpperBound() const {return upperBound; }

    Length getLowerBound() const {return lowerBound; }

    size_t getCapacity() const { return capacity; }

    bool overSized() const { return items.size() > capacity; }
    bool underSized() const { return items.size() < capacity / 2; }
    bool normalSized() const { return items.size() >= capacity / 2 && items.size() <= capacity; }

    bool suit(Length length) const { return length < upperBound && length >= lowerBound; }

    // Add an vertex to this Block.
    // caller responsible to check whether suit.
    void addItem(VertexIndex v) { items.add(v); }

    // VertexIndex removeItem(VertexIndex v) { return items.erase(v); }

    size_t countNoGreater(const GraphContext& context, Length threshold) const;

    // Extracts all items in this Block that are less than or equal to the threshold to form a new Block.
    // The original Block is modified to remove these items.
    // If strict is true, then extract items < threshold. Else, extract items <= threshold.
    // In default, strict == false.
    ShpBlock extractLessThanOrEqual(GraphContext& context, Length threshold, bool strict = false);


    // Extracts the smallest q items to form a new Block.
    // The original Block is modified to remove these items.
    ShpBlock extractMinQ(GraphContext& context, size_t q) { return extractLessThanOrEqual(context, locateMinQ(context, q)); }

    // Find the q-th smallest item in this Block.
    // This function runs in linear time.
    Length locateMinQ(const GraphContext& context, size_t q) const;

    // Split this Block into two Blocks by median.
    // This block would hold the larger half, and the function returns the smaller half.
    ShpBlock splitAtMedian(GraphContext& context) { return extractMinQ(context, items.size() / 2); }

    UList toUList();

    // Merge another block into this block.
    // The other block will be empty after the merge.
    // The complexity is linear to the size of the other linked list.
    void merge(Block& other);

    // allow iterating over items of this Block.
    auto begin() { return items.begin(); }
    auto end() { return items.end(); }

    // Find the minimum Length in this Block.
    // If the Block is empty, return upperBound.
    Length min(const GraphContext& g) const;

    // Find the maximum Length in this Block.
    // If the Block is empty, return lowerBound.
    Length max(const GraphContext& g) const;

    bool empty() const { return items.empty(); }
};
