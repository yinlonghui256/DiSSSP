#pragma once

#include "GraphContext.h"


class Heap;
class BinaryHeap;
class FibonacciHeap;


class DijstraWithHeap : public GraphContext {

	std::unique_ptr<Heap> heap;

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

// In the heap implementation of Dijkstra algorithm,
// no matter how it is implemented, it needs to track whether a key has been inserted.
// There are three ways to do this:
// 1. store a global array d[v] for the lengths of each vertex; 
//	  each time to update v to a new value d, use d[v] to locate its old position;
// 2. store a cache array pos[v] for the position in heap of each vertex;
//    each time to update v to a new value d, use pos[v] to update; however, every heap operation may cause pos to be udpated;
// 3. accept multiple occurrence of a vertex, and the heap property ensures the smallest occurrence always first get popped;
//    so each time only need to check whether a popped instance is a out-dated occurrence;
//
// For both these implementations, for binary heaps, are equivalent, as they all take O(\log n) time.
// but functionally/semantically the third does not satisfy the requirements.
// For Fibonacci heap, the third is inacceptable because decrease-key needs to be implemented in O(1) time but this is amortizing O(\log n) time.
// if pos were to be a pointer instead of a position in the heap array, then it is O(1) in both binary and fibonacci heap.
// for the first case, it is possible for bst tree but not for fibonacci heap: you cannot locate the position of a value in a fibonacci heap;


// a -> b -> a; c -> d -> c;
// a -> d -> c -> a;
// 


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

	// Use a vector structure to store all the <K, V> pairs
	// the children of vertex i are 2i+1 and 2i+2;
	// to insert/decrease-key, we first need to check whether key is already in the heap;
	// and more importantly, if it is in the heap, we need to locate its precise position.
	// we may: use its value to search for it; or cache it in an array.


	std::multimap<ActualLength, VertexIndex> heapMap;
	std::unordered_map<VertexIndex, ActualLength> vertexMap;
	


};