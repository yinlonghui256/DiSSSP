#include "Graph.h"


Graph Graph::transform2ConstDeg() const{
    // For each vertex, we create a zero-length circle.
    // For each edge, we create two corresponding vertices in the new graph.
    // And we let the circle of the vertex include the corresponding vertices of its incident edges.

    Graph g(getNumOfVertices() + getNumOfEdges() * 2, true);

    std::vector<VertexIndex> head(getNumOfVertices());
    std::vector<VertexIndex> curr(getNumOfVertices());

    for (VertexIndex i = 0; i < getNumOfVertices(); ++i) {
        head[i] = i;
        curr[i] = i;
    }
    
    VertexIndex tmp = getNumOfVertices();
    for (VertexIndex v = 0; v < getNumOfVertices(); ++v) {
        const auto& neighbors = getNeighbors(v);
        for (Arc neighbor: neighbors) {
            auto to = neighbor.to;
            auto length = neighbor.length;
            g.addEdge(curr[v], tmp, 0);
            curr[v] = tmp;
            g.addEdge(curr[to], tmp + 1, 0);
            curr[to] = tmp + 1;
            g.addEdge(tmp, tmp + 1, length);
            tmp += 2;
        }
    }

    for (VertexIndex i = 0; i < getNumOfVertices(); ++i) {
        if (head[i] != curr[i]) {
            g.addEdge(curr[i], head[i], 0);
        }
    }

    return g;
}