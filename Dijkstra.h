#pragma once

#include "GraphContext.h"


class Heap;
class BinaryHeap;
class FibonacciHeap;


class DijstraWithHeap : public GraphContext {

	Heap heap;

	public:
	DijstraWithHeap(Graph&& g, std::string_view name) : GraphContext(std::move(g), name) {}
	void solve() override {
		// Implementation of Dijkstra's algorithm using a generic heap.
		// This is a placeholder for the actual implementation.
	}
	ActualLength getLength(VertexIndex v) const override {
		// Placeholder for getting the length of the shortest path to vertex v.
		return 0;
	}


};

// Theorerically, Dijkstra with binary heap (std::multimap) runs in O(m \log n) time;
// When witn Fibonacci heap, it runs in O(m + n \log n) time.
// However, Fibonacci heap has a larger constant than binary heap.
class DijkstraWithBinaryHeap : public GraphContext {

public:
	DijkstraWithBinaryHeap(Graph&& g) : GraphContext(std::move(g), "DijkstraWithBinaryHeap") {}

	void solve() override {
		// Implementation of Dijkstra's algorithm using a binary heap (std::map).
		// This is a placeholder for the actual implementation.
	}

	ActualLength getLength(VertexIndex v) const override {
		// Placeholder for getting the length of the shortest path to vertex v.
		return 0;
	}
};

class DijkstraWithFibonacciHeap : public GraphContext {
public:
	DijkstraWithFibonacciHeap(Graph&& g) : GraphContext(std::move(g), "DijstraWithFibonacciHeap") {}
	void solve() override {
		// Implementation of Dijkstra's algorithm using a Fibonacci heap.
		// This is a placeholder for the actual implementation.
	}

	ActualLength getLength(VertexIndex v) const override {
		// Placeholder for getting the length of the shortest path to vertex v.
		return 0;
	}
};

class Heap {
	public:
	virtual void insert(VertexIndex v, ActualLength length) = 0;
	virtual void decreaseKey(VertexIndex v, ActualLength newLength) = 0;
	virtual std::pair<VertexIndex, ActualLength> extractMin() = 0;
	virtual bool isEmpty() const = 0;
	virtual ~Heap() = default;
};

class BinaryHeap : public Heap {
	// Implementation of a binary heap using std::map.
	std::multimap<ActualLength, VertexIndex> heapMap;
	std::unordered_map<VertexIndex, ActualLength> vertexMap;
	


};