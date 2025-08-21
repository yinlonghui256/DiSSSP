#pragma once

#include <cassert>
#include <cstddef>

#include <cmath>
#include <bit>
#include <limits>
#include <random>

#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <tuple>
#include <vector>

#include "debug.h"


// We do not maintain vertex objects, they are uniquely identified by their index.
using VertexIndex = size_t;
constexpr VertexIndex NULL_VERTEX = std::numeric_limits<VertexIndex>::max();


/**
 * Our algorithm works under the Comparison-Addition Model:
 * all edge weights only need to support comparison and addition operations.
 *
 * Theoretically, the minimum algebraic structure for this is a
 * Total Order Monoid (M, 0, +, <):
 *  1. It is closed under addition.
 *  2. Addition is associative: (A + B) + C = A + (B + C).
 *  3. It has an identity element 0: 0 + A = A + 0 = A.
 *  4. It is totally ordered: either A < B, A = B, or B < A.
 *  5. If A < B, then A + C < B + C and C + A < C + B.
 *  6. 0 < A for any A != 0.
 *
 * For Single-Source Shortest Path problem, monoid (R_{>=0}, 0, +, <) works.
 * For Single-Source Bottleneck Path problem, monoid (R\cup{-\infty}, -\infty, max, <) works.
 *
 * For simplicity, we use double as the length type.
 * We are ignoring truncation errors and floating-point precision issues in this project.
 * Future developers may define ActualLength as any custom type that satisfies the above properties.
 */

using ActualLength = double;

// Arc compresses an edge in the graph.
struct Arc {
    VertexIndex to;
    ActualLength length;
};


// A list of vertices, used in the FindPivot function.
using UList = std::unique_ptr<std::list<VertexIndex>>;
