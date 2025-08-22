#pragma once

#include "Graph.h"


class GraphContext {
protected:
	Graph graph; // The graph to be processed.

	std::string_view methodName; // The name of the method used for solving the problem.

public:
	GraphContext(Graph&& g, std::string_view name): graph(std::move(g)), methodName(name) {}

    virtual void solve() = 0;

    virtual ActualLength getLength(VertexIndex v) const = 0;

	virtual void printLengths() const {
		for (VertexIndex v = 0; v < graph.getNumOfVertices(); ++v) {
			std::cout << "d[" << std::setw(4) << v << "]=" << std::setw(8) << getLength(v) << std::endl;
		}
	}

	bool examine(const GraphContext& other) const {
		std::cout << "Examining lengths of " << methodName << " against " << other.methodName << std::endl;
		bool allMatch = true;
		size_t countMismatch = 0;
		for (VertexIndex v = 0; v < graph.getNumOfVertices(); ++v) {
			if (std::abs(getLength(v) - other.getLength(v)) > 1e-6) {
				allMatch = false;
				++countMismatch;
				if (countMismatch <= 10) {
					std::cout << "Mismatch at vertex " << v << ": " << getLength(v) << " != " << other.getLength(v) << std::endl;
				}
			}
		}
		std::cout << (allMatch ? "All lengths match." : "There are mismatches.") << std::endl;
	}
};

