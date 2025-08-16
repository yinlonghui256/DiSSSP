#pragma once

#include <concepts>
#include <type_traits>
#include <limits>
#include "Graph.h"

/**
 * @brief 
 * Our algorithm works under the Comparison-Addition Model:
 * all edge weights only need to support comparison and addition operations.
 * 
 * Theoretically, the minimum algebraic structure for this is a
 * Total Order Monoid:
 *  1. It is closed under addition.
 *  2. Addition is associative: (A + B) + C = A + (B + C).
 *  3. It has an identity element 0: 0 + A = A + 0 = A.
 *  4. It is totally ordered: either A < B, A == B, or A > B.
 *  5. If A < B, then A + C < B + C and C + A < C + B.
 *  6. 0 <= A for any A.
 * 
 * For Single-Source Shortest Path problem, monoid (R_{>=0}, +) works.
 * For Single-Source Bottleneck Path problem, monoid (R \cup {-\infty}, max) works. 
 * 
 * We do not implement a concept fully verifying the above properties here.
 * Just to remind if future developers want to implement their custom length types.
 */

template<typename T>
concept AddableComparable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a < b } -> std::convertible_to<bool>;
};


/**
 * @brief 
 * This defines the \hat{d} Length in the paper.
 * 
 * @tparam T 
 * Actual lengths in the graph, adpots the Comparison-Addition Model.
 */
template<AddableComparable ActualLengthT>
class LengthWrapper{
    ActualLengthT length; // The sum of length of path from source to this vertex.

    size_t numOfEdges; // The number of edges in the path
    
    VertexIndex prevVertexIndex; // The previous vertex in the path
    
    VertexIndex thisVertexIndex; // The index of this vertex
    // Though not specified in the paper,
    // we additionally store the index of this vertex to make various operations smoother.

public:

    constexpr LengthWrapper() LengthWrapper(ActualLengthT{std::numeric_limits<double>::infinity()}, SIZE_MAX, INVALID_VERTEX, INVALID_VERTEX) {}

    constexpr LengthWrapper(ActualLengthT len, size_t edges, VertexIndex prevIndex, VertexIndex thisIndex)
        : length(len), numOfEdges(edges), prevVertexIndex(prevIndex), thisVertexIndex(thisIndex) {}

    constexpr LengthWrapper(LengthWrapper&& other) = default;
    constexpr LengthWrapper& operator=(LengthWrapper&& other) = default;
    constexpr LengthWrapper(const LengthWrapper& other) = default;
    constexpr LengthWrapper& operator=(const LengthWrapper& other) = default;

    static constexpr LengthWrapper zero() { return LengthWrapper(ActualLengthT{0.0}, 0, 0, 0); }
    static constexpr LengthWrapper infinity() { return LengthWrapper(); }

    auto operator == (const LengthWrapper& other) const {
        return length == other.length &&
               numOfEdges == other.numOfEdges &&
               prevVertexIndex == other.prevVertexIndex &&
               thisVertexIndex == other.thisVertexIndex;
    }

    auto operator <=> (const LengthWrapper& other) const {
        if (auto cmp = length <=> other.length; cmp != 0) { return cmp; }
        if (auto cmp = numOfEdges <=> other.numOfEdges; cmp != 0) { return cmp; }
        if (auto cmp = prevVertexIndex <=> other.prevVertexIndex; cmp != 0) { return cmp; }
        return thisVertexIndex <=> other.thisVertexIndex;
    }

    VertexIndex getIndex() const { return thisVertexIndex; }

    LengthWrapper relax(const VertexIndex& to, ActualLengthT edgeLength) const {
        return {length + edgeLength, numOfEdges + 1, thisVertexIndex, to};
    }
};


// For simplicity, we use double as the length type.
// We are ignoring truncation errors and floating-point precision issues in this project.
using ActualLength = double;

using Length = LengthWrapper<ActualLength>;

/**
 * @brief 
 * Linear search for the q-th smallest item among items at  
 * index = (first + step * i) for i in in [0, (last - first) / step).
 * 
 * We use the typical linear search algorithm here.
 * Each time partition the items into groups of 5, and find the median of the medians of those groups.
 * This would serve as a close estimate to the median, and we may recurse down.
 * 
 * For the sake of recursion, this function would swap the q-th element to the first position in cache.
 * 
 * It might be argued that why not using std::nth_element?
 * Though it in average runs in linear time, but not promised linear for worst case.
 * As a loyal implementation of the algorithm of our paper,
 * we need to manually implement a truly linear time algorithm.
 * It is possible that the hidden time coefficient per element is larger than std::nth_element. 
 * 
 * @param cache The vector to search in.
 * @param first The starting index of the range.
 * @param last The ending index of the range (exclusive).
 * @param step The step size for searching.
 * @param q The q-th smallest element to find.
 * @return Length The q-th smallest element in the specified range.
 */
Length linearLocateMinQ(std::vector<Length>& cache, size_t q, size_t first = 0, size_t last = 0, size_t step = 1);
