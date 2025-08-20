#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <set>
#include "Types.h"
#include "debug.h"


class Length;


/**
 * @brief Graph is represented in the way of adjacency lists.
 * Source is always indexed 0.
 * Graph only supports reading in data and basic preprocessing.
 * The algorithm is implemented in the GraphContext class.
 */
class Graph {
protected:
    VertexIndex numOfVertices; // The number of vertices in the graph, which is $n$ in the paper.
    // vertices are indexed from 0 to numOfVertices - 1.

    size_t numOfEdges = 0; // The number of edges in the graph, which is $m$ in the paper.

    std::vector<std::vector<Arc>> adjacencyList;
    
    bool isConstDegree; // Whether the graph is a constant-degree graph.

public:

    Graph(VertexIndex n, bool isConstDeg = false) : numOfVertices(n), adjacencyList(n), isConstDegree(isConstDeg) {}

    Graph(const std::string& filename, bool isConstDeg = false);

    Graph(Graph&&) = default;
    Graph& operator=(Graph&&) = default;
    Graph(const Graph&) = default;
    Graph& operator=(const Graph&) = default;

    VertexIndex getNumOfVertices() const { return numOfVertices; }

    size_t getNumOfEdges() const { return numOfEdges; }

    const std::vector<Arc>& getNeighbors(VertexIndex v) const { return adjacencyList.at(v); }

    void addEdge(VertexIndex from, VertexIndex to, ActualLength length) {
        adjacencyList.at(from).emplace_back(to, length);
        ++ numOfEdges;
    }

	// Used merely for debugging.
    friend std::ostream& operator<<(std::ostream& os, const Graph& g);

    // Transforms a general directed graph into a constant-bounded-degree graph.
    // More precisely, each vertex has at most 2 outgoing edges (and at most 2 incoming edges, though we do not care about them).
	// The indices in the old graph are preserved in the new graph.
	// The new graph will have (n + 2 * m) vertices, and (n + 3 * m) edges, where n is the number of vertices in the old graph, and m is the number of edges in the old graph.
    Graph transform2ConstDeg() const ;

};


// generates a random graph and writes it to a file for testing.
void genRandGraph2File(const std::string& filename, VertexIndex n, size_t m,
    ActualLength minLen = 1.0, ActualLength maxLen = 10.0, unsigned int seed = 0);
