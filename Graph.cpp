#include "Graph.h"
#include "Length.h"


Graph::Graph(const std::string& filename, bool isConstDeg) : isConstDegree(isConstDeg) {
    std::ifstream fin{ filename };
    if (!fin) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    if (!(fin >> numOfVertices >> numOfEdges)) {
        throw std::runtime_error("Invalid file header: " + filename);
    }
    adjacencyList.resize(numOfVertices);
    for (size_t i = 0; i < numOfEdges; ++i) {
        VertexIndex from, to;
        ActualLength length;
        if (!(fin >> from >> to >> length)) {
            throw std::runtime_error("Not enough edge data in file: " + filename);
        }
        if (from >= numOfVertices || to >= numOfVertices) {
            throw std::runtime_error("Vertex index out of range in file: " + filename);
        }
        adjacencyList[from].emplace_back(to, length);
    }
}

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


std::ostream& operator<<(std::ostream& os, const Graph& g) {
#ifdef DEBUG_GRAPH
    os << "Graph with " << g.getNumOfVertices() << " vertices and " << g.getNumOfEdges() << " edges:\n";
    for (VertexIndex v = 0; v < g.getNumOfVertices(); ++v) {
        os << "Vertex " << v << ":   \t";
        for (auto [to, length] : g.getNeighbors(v)) {
            os << "(" << v << "->" << to << ", " << length << ")   \t";
        }
        os << "\n";
    }
#endif
    return os;
}


void genRandGraph2File(const std::string& filename, VertexIndex n, size_t m,
    ActualLength minLen, ActualLength maxLen, unsigned int seed) {
    std::ofstream fout(filename);
    if (!fout) return;

    fout << n << " " << m << "\n";
    std::random_device rd;
    std::mt19937 gen(seed ? seed : rd());
    std::uniform_int_distribution<VertexIndex> vertex_dist(0, n - 1);
    std::uniform_real_distribution<ActualLength> length_dist(minLen, maxLen);

    std::set<std::pair<VertexIndex, VertexIndex>> edge_set;
    size_t count = 0;
    while (count < m) {
        VertexIndex from = vertex_dist(gen);
        VertexIndex to = vertex_dist(gen);
		if (from == to) continue; // no self-loops
		if (edge_set.count({ from, to })) continue; // no parallel edges
        edge_set.insert({ from, to });
        ActualLength len = length_dist(gen);
        fout << from << " " << to << " " << len << "\n";
        ++count;
    }
}