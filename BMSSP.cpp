#include "BMSSP.h"

BMSSP::BMSSPReturn BMSSP::BMSSP_recurse(Parameters lkt, Length B, ShpBlock S) {
    auto [l, k, t] = lkt;

    DEBUG_BMSSP_LOG("BMSSP_recurse called with parameters: l=" << l << ", k=" << k << ", t=" << t << ", B=" << B << ", S=" << *S);

    // Implemented as algorithm 3 in the paper.
    if (l == 0) { return BMSSP_basecase({ 0, k, t }, B, S); }

    auto [P, W] = FindPivot(lkt, B, S);

    size_t M = (size_t(1) << ((l - 1) * t)); // M = 2^((l-1)*t).
    size_t largeWorkload = k << (l * t); // largeWorkload = k * 2^(l*t).
    FrontierManager D(*this, M, B);

    D.insert(std::move(P));

    // i in the paper is merely for better specification and clearer proof.
    // No need to introduce in the code.
    Length Bprime = D.getCurrentLowerBound(); // Now Bprime is B_0' in the paper.
    ShpBlock U = std::make_shared<Block>(newList(), B, Bprime, largeWorkload);

    while (true) {
        auto [Bi, S] = D.pull();
        if (S->empty()) { break; }
        ShpBlock Ui;
        std::tie(Bprime, Ui) = BMSSP_recurse({ l - 1, k, t }, Bi, S); // Now Bprime is B_i' in the paper.

        // By algorithm property, Ui is disjoint with U which contains U_0 ~ U_{i-1}.
        if (U->getSize() + Ui->getSize() > largeWorkload) {
            U->merge(*Ui);
            break;
        }

        ShpBlock K = std::make_shared<Block>(newList(), Bi, Bprime, M);
        // Update Ui's out degrees.
        for (auto u : *Ui) {
            for (auto [v, weight_uv] : constDegGraph.getNeighbors(u)) {
                Length relax = dhat[u].relax(v, weight_uv);
                if (relax <= dhat[v] && relax < B) {
                    dhat[v] = relax;
                    if (relax >= Bi) {
                        D.insert(v); // Insert into FrontierManager directly if in [Bi, B).
                    }
                    else {
                        K->addItem(v); // Otherwise, add to K to be batch-prepended later.
                    }
                }
            }
        }
        U->merge(*Ui); // Merge Ui into U.

        // Any v in S such that dhat[v] < Bprime is guaranteed to be in Ui;
        // Those remaining in S must have dhat[v] \in [Bprime, Bi).
        K->merge(*S);
        D.batchPrepend(std::move(K)); // Batch-prepend K to FrontierManager.
    }
	auto WLessThanBprime = extractLessThanOrEqual(W, Bprime, true);
    for (auto v : *WLessThanBprime) { U->addItem(v); }
	DEBUG_BMSSP_LOG("BMSSP_recurse of parameter l=" << l << ", k=" << k << ", t=" << t << ", B=" << B << "completed with B'=" << Bprime << ", U=" << *U);
    return std::make_pair(Bprime, U);
}


BMSSP::BMSSPReturn BMSSP::BMSSP_basecase(Parameters lkt, Length B, ShpBlock S) {
    auto [l, k, t] = lkt;
	DEBUG_BMSSP_LOG("BMSSP_basecase called with parameters: l=" << l << ", k=" << k << ", t=" << t << ", B=" << B << ", S=" << *S);

    assert(S->getSize() == 1 && "BMSSP base case requires a block with exactly one item.");

    // In base case, there is no need to use FrontierManager.
    // A simple std::map suffices.
    VertexIndex x = *S->begin();
    ShpBlock U = std::make_shared<Block>(newList(), B, dhat[x], k);
    std::map<Length, VertexIndex> H;
    U->addItem(x);
    H[dhat[x]] = x;
    while (!H.empty() && !U->overSized()) {
        auto [_, u] = *H.begin();
        H.erase(H.begin());
        U->addItem(u);
        for (const auto& [v, weight_uv] : constDegGraph.getNeighbors(u)) {
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
    if (U->getSize() <= k) {
        return std::make_pair(B, U); // Return B and U if the size is within limit.
    }
    else {
        // U's size must be k + 1.
        Length Bprime = U->max(*this);
        return std::make_pair(Bprime, U->extractLessThanOrEqual(*this, Bprime, true));
    }
}


BMSSP::FindPivotReturn BMSSP::FindPivot(Parameters lkt, Length B, ShpBlock S) {
    auto [l, k, t] = lkt;
	DEBUG_BMSSP_LOG("FindPivot called with parameters: l=" << l << ", k=" << k << ", t=" << t << ", B=" << B << ", S=" << *S);
    // Implementation of the FindPivot function as specified in the paper.
    UList W = S->toUList();
    size_t W_count = 0;
    size_t kS = k * S->getSize();
    std::vector<size_t> layerInW(constDegGraph.getNumOfVertices(), 0);
    for (const auto& v : *W) { layerInW[v] = 1; }

    for (size_t i = 1; i <= k; ++i) {
        for (const auto& u : *W) {
            if (layerInW[u] == i) {
                for (const auto& [v, weight_uv] : constDegGraph.getNeighbors(u)) {
                    Length relax = dhat[u].relax(v, weight_uv);
                    if (relax <= dhat[v] && relax < B) {
                        dhat[v] = relax;
                        if (layerInW[v] == 0) { ++W_count; W->emplace_back(v); }
                        layerInW[v] = i + 1;
                    }
                }
            }
        }
        if (W_count > kS) {
#ifdef DEBUG_BMSSP
			DEBUG_BMSSP_LOG("FindPivot found too many vertices in layer " << i << ", returning S and W: ");
            DEBUG_OS << "W vertex\t:";  for (auto& v : *W) { DEBUG_OS << std::setw(4) << v << ", "; } DEBUG_OS << std::endl;
            DEBUG_OS << "Index   \t:";  for (size_t i = 0; i < constDegGraph.getNumOfVertices(); ++i) { DEBUG_OS << std::setw(4) << i; } DEBUG_OS << std::endl;
            DEBUG_OS << "layerInW\t:";  for (size_t i = 0; i < layerInW.size(); ++i) { DEBUG_OS << std::setw(4) << layerInW[i]; } DEBUG_OS << std::endl;
#endif
            return std::make_pair(S->toUList(), std::move(W));
        }
    }

    UList P = std::make_unique<std::list<VertexIndex>>();

    std::vector<size_t> subtree(constDegGraph.getNumOfVertices(), 0);
    std::vector<bool> isRoot(constDegGraph.getNumOfVertices(), true);

    // Now DFS.
    std::stack<VertexIndex> dfsStack;

    for (const auto& v : *W) { dfsStack.push(v); }

    // For every vertex popped, its subtree size has been correctly calculated.
    // For every vertex, if its subtree size has not been calculated, it is kept zero.
    // So if a vertex has non-zero subtree size, its subtree size has been correctly calculated.
    // If the two children of a vertex have their subtree sizes calculated,
    // then this vertex is ready to be processed.
    while (!dfsStack.empty()) {
        auto u = dfsStack.top();
#ifdef DEBUG_BMSSP
		auto stack_size = dfsStack.size();
        auto uPrev = dhat[u];
#endif
		if (subtree[u]) {dfsStack.pop(); continue;} // u has been processed, skip it.		
        size_t tmpTreeSize = 1; // Count itself.
        bool ready = true;
        for (const auto& [v, weight_uv] : constDegGraph.getNeighbors(u)) {
            if (layerInW[v] && dhat[u].relax(v, weight_uv) <= dhat[v]) {
                // This is a valid edge in F.
                isRoot[v] = false; // v is not a root.
                if (subtree[v]) {
                    tmpTreeSize += subtree[v];
                }
                else {
                    ready = false;
                    dfsStack.push(v); // Push v to stack for later processing.
                }
            }
        }
        if (ready) {
            subtree[u] = tmpTreeSize;
            dfsStack.pop();
        }
    }

    for (const auto& v : *W) {
        if (isRoot[v] && subtree[v] >= k) {
            P->emplace_back(v);
        }
    }

#ifdef DEBUG_BMSSP
	DEBUG_BMSSP_LOG("FindPivot found " << P->size() << " pivot vertices with W of size " << W -> size());
	DEBUG_OS << "Pivot vertex: ";  for (auto& v : *P) { DEBUG_OS << std::setw(4) << v << ", "; } DEBUG_OS << std::endl;
    DEBUG_OS << "W vertex\t: ";  for (auto& v : *W) { DEBUG_OS << std::setw(4) << v << ", "; } DEBUG_OS << std::endl;
    DEBUG_OS << "Index   \t:";  for (size_t i = 0; i < constDegGraph.getNumOfVertices(); ++i) { DEBUG_OS << std::setw(4) << i; } DEBUG_OS << std::endl;
    DEBUG_OS << "layerInW\t:";  for (size_t i = 0; i < layerInW.size(); ++i) { DEBUG_OS << std::setw(4) << layerInW[i]; } DEBUG_OS << std::endl;
    DEBUG_OS << "subtree \t:";  for (size_t i = 0; i < subtree.size(); ++i) { DEBUG_OS << std::setw(4) << subtree[i]; } DEBUG_OS << std::endl;
    DEBUG_OS << "isRoot  \t:";  for (size_t i = 0; i < isRoot.size(); ++i) { DEBUG_OS << std::setw(4) << isRoot[i]; } DEBUG_OS << std::endl;
#endif


    return std::make_pair(std::move(P), std::move(W));
}


UList BMSSP::extractLessThanOrEqual(UList& uList, Length threshold, bool strict) {
	DEBUG_BMSSP_LOG("Extracting vertices from uList with threshold " << threshold << " (strict: " << std::boolalpha << strict << ")");
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


void BMSSP::resetDhat() {
    // Reset the dhat array to its initial state.
    dhat.clear();
    dhat.emplace_back(Length::zero()); // Source vertex
    for (VertexIndex v = 1; v < constDegGraph.getNumOfVertices(); ++v) {
        dhat.emplace_back(std::numeric_limits<ActualLength>::infinity(), SIZE_MAX, NULL_VERTEX, v);
    }
}


void BMSSP::solve() {
    // Entry point for the BMSSP algorithm.

    // Specify parameters for the BMSSP algorithm.
	auto n = constDegGraph.getNumOfVertices();
    size_t l = static_cast<size_t>(std::ceil(std::cbrt(std::log2(n)))), k = l, t = l * l;
    DEBUG_BMSSP_LOG("Starting BMSSP algorithm on transformed graph with " << n << " vertices, with Parameters: l=" << l << ", k=" << k << ", t=" << t);

    // Start with the source vertex.
    auto initialBlock = std::make_shared<Block>(newList(), Length::infinity(), Length::zero(), 0);
    initialBlock -> addItem(0);

	DEBUG_BMSSP_LOG("Initial block created with source vertex 0; About to call top layer BMSSP_recurse.");
    // Call the recursive BMSSP function with the initial parameters.
    BMSSP_recurse({ l, k, t }, Length::infinity(), initialBlock);
}

