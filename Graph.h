#pragma once

#include <vector>
#include "Length.h"


// We do not maintain vertex objects, they are uniquely identified by their index.
using VertexIndex = size_t;

constexpr VertexIndex NULL_VERTEX = SIZE_MAX;

struct Arc {
    VertexIndex to;
    ActualLength length;
};


/**
 * @brief Graph is represented in the way of adjacency lists.
 * Source is always indexed 0.
 * Lengths(dhat array) are stored in the Graph.
 */
class Graph {
protected:
    VertexIndex numOfVertices; // The number of vertices in the graph, which is $n$ in the paper.
    // vertices are indexed from 0 to numOfVertices - 1.

    size_t numOfEdges = 0; // The number of edges in the graph, which is $m$ in the paper.

    std::vector<std::vector<Arc>> adjacencyList;
    
    bool isConstDegree; // Whether the graph is a constant-degree graph.

public:

    Graph(VertexIndex n, bool isConstGeg = false) : numOfVertices(n), adjacencyList(n), isConstDegree(isConstGeg) {}

    VertexIndex getNumOfVertices() const { return numOfVertices; }

    size_t getNumOfEdges() const { return numOfEdges; }

    virtual const std::vector<Arc>& getNeighbors(VertexIndex v) const { return adjacencyList.at(v); }

    virtual void addEdge(VertexIndex from, VertexIndex to, ActualLength length) {
        adjacencyList.at(from).emplace_back(to, length);
        ++ numOfEdges;
    }

    friend std::ostream& operator<<(std::ostream& os, const Graph& g);

    // Transforms a general directed graph into a constant-bounded-degree graph.
    // More precisely, each vertex has at most 2 outgoing edges (and at most 2 incominig edges, though we do not care about them).
    // The indices in the 
    Graph transform2ConstDeg() const ;

};

