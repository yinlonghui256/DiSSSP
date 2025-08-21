#include "Block.h"
#include "BMSSP.h"


void Block::addItem(VertexIndex v) {
    DEBUG_BLOCK_LOG("Adding item " << v << " to " << *this);
    items.add(v);
}


size_t Block::countNoGreater(const BMSSP& context, Length threshold) const {
	DEBUG_BLOCK_LOG("Counting items no greater than " << threshold << " in " << *this);
    size_t count = 0;
    for (auto it: items){
        if (context.getDhat()[it] <= threshold) {
            ++count;
        }
    }
    return count;
}


Length Block::locateMinQ(const BMSSP& context, size_t q) const {
	DEBUG_BLOCK_LOG("Locating " << q << "-th smallest item in " << *this);
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


ShpBlock Block::extractLessThanOrEqual(BMSSP& context, Length threshold, bool strict) {
	DEBUG_BLOCK_LOG("Extracting items no greater than " << threshold << " in " << *this);
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
            auto curr = *it;
            ++ it;
            if (context.getDhat()[curr] < threshold) {
                newList.add(curr); // As being added into newList, it will be removed from the current Block.
            }
        }
    } else {
        for (auto it = items.begin(); it != items.end(); ) {
            auto curr = *it;
            ++ it;
            if (context.getDhat()[curr] <= threshold) {
                newList.add(curr); // As being added into newList, it will be removed from the current Block.
            }
        }
    }

    return std::make_shared<Block>(std::move(newList), threshold, lowerBound, capacity);
}


UList Block::toUList() {
	DEBUG_BLOCK_LOG("Converting to UList" << *this);
    UList res = std::make_unique<std::list<VertexIndex>>();
    for (auto it: items) { res->emplace_back(it); }
    return res;
}

void Block::merge(Block& other)  {
	DEBUG_BLOCK_LOG("Merging " << other << "into " << *this);
    items.merge(other.items);
    upperBound = std::max(upperBound, other.upperBound);
    lowerBound = std::min(lowerBound, other.lowerBound);
}

Length Block::min(const BMSSP& g) const {
    Length minLength = upperBound;
    for (auto v : items) {
        minLength = std::min(minLength, g.getDhat()[v]);
    }
    return minLength;
}


Length Block::max(const BMSSP& g) const {
    Length maxLength = lowerBound;
    for (auto v : items) {
        maxLength = std::max(maxLength, g.getDhat()[v]);
    }
    return maxLength;
}


void Block::removeUnsuit(const BMSSP& g) {
    DEBUG_BLOCK_LOG("Removing unsuited items from " << *this);
    for (auto it = items.begin(); it != items.end(); ) {
        auto curr = *it;
        ++ it;
        if (!suit(g.getDhat()[curr]) ) {
            items.erase(curr);
        }
    }
    DEBUG_BLOCK_LOG("After removing unsuited items, Block is now: " << *this);
}


void Block::extendLowerBound(Length newLowerBound) {
    DEBUG_BLOCK_LOG("Extending lower bound of " << *this << " to " << newLowerBound);
    lowerBound = std::min(lowerBound, newLowerBound);
}