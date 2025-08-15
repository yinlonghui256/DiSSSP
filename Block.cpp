#include "Block.h"


size_t Block::countNoGreater(const GraphContext& context, Length threshold) const {
    size_t count = 0;
    for (auto it: items){
        if (context.getDhat()[it] <= threshold) {
            ++count;
        }
    }
    return count;
}


Length Block::locateMinQ(const GraphContext& context, size_t q) const {
    if (q == 0 || q > items.size()) {
        throw std::out_of_range("k is out of range in locateMinK");
    }
    // linked list is not convenient for random access,
    // we reorganize them into a vector for easier processing.
    std::vector<Length> cache;
    cache.reserve(items.size());
    for (auto it: items) { cache.emplace_back(context.getDhat()[it]); }
    // use linear time selection algorithm to find the k-th smallest item.
    return linearLocateMinQ(cache, q);
}


// in principle, lowerBound < threshold < upperBound.
// threshold itself should be equal to some item in the Block.
// This function should only be called when the Block is oversized.
ShPBlock Block::extractLessThan(GraphContext& context, Length threshold) {
    if (threshold <= lowerBound || upperBound <= threshold) {
        throw std::out_of_range("threshold is out of range in extractNoGreater");
    } else if (!overSized()) {
        throw std::logic_error("Block is not oversized. Should not call extractNoGreater.");
    }

    lowerBound = threshold;

    auto newList = context.newList();
    for (auto it = items.begin(); it != items.end(); ) {
        auto currIt = it ++;
        if (context.getDhat()[*currIt] < threshold) {
            newList.add(*currIt); // As being added into newList, it will be removed from the current Block.
        }
    }
    return std::make_shared<Block>(lowerBound, threshold, capacity, std::move(newList));
}


UList Block::toUList() {
    UList res = std::make_unique<std::list<VertexIndex>>();
    for (auto it: items) { res->emplace_back(it); }
    return res;
}