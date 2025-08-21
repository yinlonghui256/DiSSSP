#pragma once

#include "Graph.h"


class GraphContext {
protected:
	Graph graph; // The graph to be processed.

public:
	GraphContext(Graph&& g): graph(g) {}

    virtual void solve() = 0;

	virtual ActualLength getLength(VertexIndex v) = 0;
};


// Theorerically, Dijkstra with binary heap (std::map) runs in O(m \log n) time;
// When witn Fibonacci heap, it runs in O(m + n \log n) time.
// However, Fibonacci heap has a larger constant than binary heap.
class DijkstraWithBinaryHeap : public GraphContext {
	
public:
	DijkstraWithBinaryHeap(Graph&& g): GraphContext(std::move(g)) {}

	void solve() override {
		// Implementation of Dijkstra's algorithm using a binary heap (std::map).
		// This is a placeholder for the actual implementation.
	}

	ActualLength getLength(VertexIndex v) override {
		// Placeholder for getting the length of the shortest path to vertex v.
		return 0;
	}
};

class DijkstraWithFibonacciHeap : public GraphContext {
	public:
	DijkstraWithFibonacciHeap(Graph&& g): GraphContext(std::move(g)) {}
	void solve() override {
		// Implementation of Dijkstra's algorithm using a Fibonacci heap.
		// This is a placeholder for the actual implementation.
	}

	ActualLength getLength(VertexIndex v) override {
		// Placeholder for getting the length of the shortest path to vertex v.
		return 0;
	}
};