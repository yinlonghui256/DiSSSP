#pragma once

#include <list>
#include <stack>
#include "Graph.h"
#include "Block.h"
#include "ManualLinkedList.h"
#include "FrontierManager.h"


class GraphContext {

    Graph graph;

    std::vector<Length> dhat; // The \hat{d} array in the paper, storing the lengths of the shortest paths from the source to each vertex.

    std::shared_ptr<ManualLinkedListBase> spListBase;


    // The return value of BMSSP, used between recursive calls, as specified in algorithm 3 in the paper.
    // U and W may overlap, we need Block to rule out repetitions.
    using BMSSPReturn = std::pair<Length, ShpBlock>; // (B', U)

    // The return value of FindPivot, as specified in algorithm 1 in the paper.
    using FindPivotReturn = std::pair<UList, UList>; // (P, W)

    using Parameters = std::tuple<size_t, size_t, size_t>; // (l, k, t)

    // recursive function for BMSSP, which is called by the top layer BMSSP function.
    // the signature is specified as in algorithm 3 in the paper.
    BMSSPReturn BMSSP_recurse(Parameters lkt, Length B, ShpBlock S);

    // base case for BMSSP, which is called when l = 0.
    // Implemented as algorithm 2 in the paper.
    BMSSPReturn BMSSP_basecase(Parameters lkt, Length B, ShpBlock S);

    // FindPivot function, which is called in the BMSSP_recurse function.
    // The signature is specified as in algorithm 1 in the paper.
    FindPivotReturn FindPivot(Parameters lkt, Length B, ShpBlock S);

    // extracts the vertices from uList that are less than or equal to the threshold.
    // If strict is true, extract strictly less than; otherwise, extract less than or equal (defualt false).
    UList extractLessThanOrEqual(UList& uList, Length threshold, bool strict = false);

public:

    GraphContext(Graph&& g)
    : graph(g) {
        spListBase = std::make_shared<ManualLinkedListBase>(g.getNumOfVertices());
        dhat.reserve(g.getNumOfVertices());
        dhat.emplace_back(0.0, 0, 0, 0); // source vertex
        for (VertexIndex v = 1; v < g.getNumOfVertices(); ++v) {
            dhat.emplace_back(std::numeric_limits<ActualLength>::infinity(), SIZE_MAX, NULL_VERTEX, v);
        }
    }

    const std::vector<Length> & getDhat() const { return dhat; }

    ManualLinkedList newList() { return spListBase->newList(); }

    // This function is the entry point for the BMSSP algorithm.
    // It would update dhat to the expected values.
    void BMSSP();

};
