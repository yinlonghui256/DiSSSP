#pragma once

#include <vector>
#include <list>
#include <map>
#include "Graph.h"
#include "Block.h"

class GraphContext {

    Graph graph;

    std::vector<Length> dhat; // The \hat{d} array in the paper, storing the lengths of the shortest paths from the source to each vertex.

public:

    GraphContext(Graph&& g)
    : graph(g) {
        dhat.reserve(g.getNumOfVertices());
        dhat.emplace_back(0.0, 0, 0, 0); // source vertex
        for (VertexIndex v = 1; v < g.getNumOfVertices(); ++v) {
            dhat.emplace_back(std::numeric_limits<ActualLength>::infinity(), SIZE_MAX, INVALID_VERTEX, v);
        }
    }

    const std::vector<Length> & getDhat() const { return dhat; }



};