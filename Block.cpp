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
    // boundary cases short-circuit.
    if (q == items.size()) { return max(context); }
    if (q == 1) { return min(context); }

    // linked list is not convenient for random access,
    // we reorganize them into a vector for easier processing.
    std::vector<Length> cache;
    cache.reserve(items.size());
    for (auto it: items) { cache.emplace_back(context.getDhat()[it]); }

    // use linear time selection algorithm to find the k-th smallest item.
    linearLocateMinQ<Length>(cache, q);
    return cache[0];
    //return linearLocateMinQ(cache, q);
}


ShpBlock Block::extractLessThanOrEqual(GraphContext& context, Length threshold, bool strict) {
    lowerBound = threshold;

    // If threshold >= upperBound, then we are extracting all items anyway, and this Block will become empty.
    if (threshold >= upperBound) {
        auto newBlock = std::make_shared<Block>(*this);
        items = context.newList();
        return newBlock;
    }

    auto newList = context.newList();
    if (strict) {
        for (auto it = items.begin(); it != items.end(); ) {
            auto currIt = it ++;
            if (context.getDhat()[*currIt] < threshold) {
                newList.add(*currIt); // As being added into newList, it will be removed from the current Block.
            }
        }
    } else {
        for (auto it = items.begin(); it != items.end(); ) {
            auto currIt = it ++;
            if (context.getDhat()[*currIt] <= threshold) {
                newList.add(*currIt); // As being added into newList, it will be removed from the current Block.
            }
        }
    }

    return std::make_shared<Block>(threshold, lowerBound, capacity, std::move(newList));
}


UList Block::toUList() {
    UList res = std::make_unique<std::list<VertexIndex>>();
    for (auto it: items) { res->emplace_back(it); }
    return res;
}

void Block::merge(Block& other)  {
    items.merge(other.items);
    upperBound = std::max(upperBound, other.upperBound);
    lowerBound = std::min(lowerBound, other.lowerBound);
}

Length Block::min(const GraphContext& g) const {
    Length minLength = upperBound;
    for (auto v : items) {
        minLength = std::min(minLength, g.getDhat()[v]);
    }
    return minLength;
}


Length Block::max(const GraphContext& g) const {
    Length maxLength = lowerBound;
    for (auto v : items) {
        maxLength = std::max(maxLength, g.getDhat()[v]);
    }
    return maxLength;
}