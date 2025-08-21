#pragma once


#include "types.h"


/**
 * This defines the \hat{d} Length in the paper.
 */
class Length{
    ActualLength length; // The sum of length of path from source to this vertex.

    size_t numOfEdges; // The number of edges in the path
    
    VertexIndex prevVertexIndex; // The previous vertex in the path
    
    VertexIndex thisVertexIndex; // The index of this vertex
    // Though not specified in the paper,
    // we additionally store the index of this vertex to make various operations smoother.

public:

    constexpr Length(): Length(ActualLength{std::numeric_limits<double>::infinity()}, SIZE_MAX, NULL_VERTEX, NULL_VERTEX) {}

    constexpr Length(ActualLength len, size_t edges, VertexIndex prevIndex, VertexIndex thisIndex)
        : length(len), numOfEdges(edges), prevVertexIndex(prevIndex), thisVertexIndex(thisIndex) {}

    constexpr Length(Length&& other) = default;
    constexpr Length& operator=(Length&& other) = default;
    constexpr Length(const Length& other) = default;
    constexpr Length& operator=(const Length& other) = default;

    static constexpr Length zero() { return Length(ActualLength{0.0}, 0, 0, 0); }
    static constexpr Length infinity() { return Length(); }

    auto operator == (const Length& other) const {
        return length == other.length &&
               numOfEdges == other.numOfEdges &&
               prevVertexIndex == other.prevVertexIndex &&
               thisVertexIndex == other.thisVertexIndex;
    }

    auto operator <=> (const Length& other) const {
        return std::tie(length, numOfEdges, prevVertexIndex, thisVertexIndex) <=>
            std::tie(other.length, other.numOfEdges, other.prevVertexIndex, other.thisVertexIndex);
        // if (auto cmp = length <=> other.length; cmp != 0) { return cmp; }
        // if (auto cmp = numOfEdges <=> other.numOfEdges; cmp != 0) { return cmp; }
        // if (auto cmp = prevVertexIndex <=> other.prevVertexIndex; cmp != 0) { return cmp; }
        // return thisVertexIndex <=> other.thisVertexIndex;
    }

    friend std::ostream& operator<<(std::ostream& os, const Length& l) {
        if (l == Length::infinity()) { os << "{INF}"; }
        else if (l == Length::zero()) { os << "{ZERO}"; }
        else {
            os << "{" << l.length << ", |" << l.numOfEdges << "|, " <<
                (l.prevVertexIndex == NULL_VERTEX ? "N" : std::to_string(l.prevVertexIndex)) << " -> " <<
                (l.thisVertexIndex == NULL_VERTEX ? "N" : std::to_string(l.thisVertexIndex)) << "}";
        }
        return os;
    }

    VertexIndex getIndex() const { return thisVertexIndex; }

	ActualLength getLength() const { return length; }

    Length relax(const VertexIndex& to, ActualLength edgeLength) const {
        return {length + edgeLength, numOfEdges + 1, thisVertexIndex, to};
    }
};


/**
 * @brief 
 * Linear search for the q-th smallest item among items at  
 * index = (first + step * i) for i in in [0, (last - first) / step).
 * The function would swap the q-th element to cache[first].
 * 
 * We use the typical linear search algorithm here.
 * Each time partition the items into groups of 5, and find the median of the medians of those groups.
 * This would serve as a close estimate to the median, and we may recurse down.
 * 
 * It might be argued that why not using std::nth_element?
 * Though it in average runs in linear time, but not promised linear for worst case.
 * As a loyal implementation of the algorithm of our paper,
 * we need to manually implement a truly linear time algorithm.
 * It is possible that the hidden time coefficient per element is larger than std::nth_element. 
 * 
 * @param cache The vector to search in.
 * @param q The q-th smallest element to find.
 * @param first The starting index of the range.
 * @param last The ending index of the range (exclusive).
 * @param step The step size for searching.
 */
template <typename L>
void linearLocateMinQ(std::vector<L>& cache, size_t q, size_t first = 0, size_t last = 0, size_t step = 1);


#ifdef DEBUG_LENGTH
extern template void linearLocateMinQ<int>(std::vector<int>& cache, size_t q, size_t first, size_t last, size_t step);
#endif

extern template void linearLocateMinQ(std::vector<Length>& cache, size_t q, size_t first, size_t last, size_t step);
