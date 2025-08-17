#include "GraphContext.h"


GraphContext::BMSSPReturn GraphContext::BMSSP_recurse(Parameters lkt, Length B, ShpBlock S) {
    auto [l, k, t] = lkt;

    // Implemented as algorithm 3 in the paper.
    if (l == 0) { return BMSSP_basecase({0, k, t}, B, S); }

    auto [P, W] = FindPivot(B, S);

    size_t M = (1 << ((l - 1) * t)); // M = 2^((l-1)*t).
    size_t largeWorkload = k << (l * t); // largeWorkload = k * 2^(l*t).
    FrontierManager D(*this, M, B);

    D.insert(std::move(P));

    // i in the paper is merely for better specification and clearer proof.
    // No need to introduce in the code.
    Length Bprime = D.getCurrentLowerBound(); // Now Bprime is B_0' in the paper.
    ShpBlock U = std::make_shared<Block>(newList(), B, Bprime, largeWorkload);

    while (true) {
        auto [Bi, S] = D.pull();
        if (S -> empty()) { break; }
        ShpBlock Ui;
        std::tie(Bprime, Ui) = BMSSP_recurse({l - 1, k, t}, Bi, S); // Now Bprime is B_i' in the paper.

        // By algorithm property, Ui is disjoint with U which contains U_0 ~ U_{i-1}.
        if (U -> getSize() + Ui -> getSize() > largeWorkload) {
            U -> merge(*Ui);
            break;
        }

        ShpBlock K = std::make_shared<Block>(newList(), Bi, Bprime, M);
        // Update Ui's out degrees.
        for (auto u: *Ui) {
            for (auto [v, weight_uv]: graph.getNeighbors(u)) {
                Length relax = dhat[u].relax(v, weight_uv);
                if (relax <= dhat[v] && relax < B) {
                    dhat[v] = relax;
                    if (relax >= Bi) {
                        D.insert(v); // Insert into FrontierManager directly if in [Bi, B).
                    } else {
                        K -> addItem(v); // Otherwise, add to K to be batch-prepended later.
                    }
                }
            }
        }
        U -> merge(*Ui); // Merge Ui into U.

        // Any v in S such that dhat[v] < Bprime is guaranteed to be in Ui;
        // Those remaining in S must have dhat[v] \in [Bprime, Bi).
        K -> merge(*S);
        D.batchPrepend(std::move(K)); // Batch-prepend K to FrontierManager.
    }
    return std::make_pair(Bprime, U);
}


GraphContext::BMSSPReturn GraphContext::BMSSP_basecase(Parameters lkt, Length B, ShpBlock S) {
    auto [l, k, t] = lkt;
    if (S -> getSize() != 1) {
        throw std::invalid_argument("BMSSP base case requires a block with exactly one item.");
    }
    // In base case, there is no need to use FrontierManager.
    // A simple std::map suffices.
    VertexIndex x = * S->begin();
    ShpBlock U = std::make_shared<Block>(newList(), B, dhat[x], k);
    std::map<Length, VertexIndex> H;
    U -> addItem(x);
    H[dhat[x]] = x;
    while (!H.empty() && ! U -> overSized()) {
        auto [_, u] = * H.begin();
        H.erase(H.begin());
        U -> addItem(u);
        for (const auto& [v, weight_uv]: graph.getNeighbors(u)) {
            Length relax = dhat[u].relax(v, weight_uv);
            if (relax <= dhat[v] && relax < B) {
                if (auto it = H.find(dhat[v]); it != H.end()) {
                    H.erase(it); // Remove the old value of dhat[v] if exists.
                }
                dhat[v] = relax; // Update dhat[v] with the new value.
                H[dhat[v]] = v; // Insert the new value into H.
            }
        }
    }
    if (U -> getSize() <= k) {
        return std::make_pair(B, U); // Return B and U if the size is within limit.
    } else {
        // U's size must be k + 1.
        Length Bprime = U -> max();
        return std::make_pair(Bprime, U->extractLessThanOrEqual(*this, Bprime, true));
    }
}


GraphContext::FindPivotReturn GraphContext::FindPivot(Length B, ShpBlock S) {
    // Implementation of the FindPivot function as specified in the paper.
    // This function will find the pivot vertices and return the appropriate FindPivotReturn structure.
}


UList GraphContext::extractLessThanOrEqual(UList& uList, Length threshold, bool strict) {
    // Implementation of the extraction logic from uList based on the threshold.
    // If strict is true, it extracts vertices strictly less than the threshold;
    // otherwise, it extracts vertices less than or equal to the threshold.
    UList result = std::make_unique<std::list<VertexIndex>>();
    for (const auto& vertex : *uList) {
        if ((strict && dhat[vertex] < threshold) || (!strict && dhat[vertex] <= threshold)) {
            result->push_back(vertex);
        }
    }
    return result;
}

void GraphContext::BMSSP() {
    // Entry point for the BMSSP algorithm.

    // auto initialBlock = std::make_shared<Block>(graph.getNumOfVertices());
    // BMSSPReturn result = BMSSP_recurse(graph.getNumOfVertices(), Length::infinity(), initialBlock);
    
    // Process the result and update dhat as needed.
    // This part of the code will handle the final updates to dhat based on the BMSSP results.
}