#include "dls/flow.hpp"
#include <climits>
#include <vector>

namespace dls {

namespace {

/**
 * Custom BFS to find shortest augmenting path in residual network.
 * Returns true if an augmenting path from source to sink exists, filling parent pointers.
 */
bool bfsAugmentingPath(int n, const std::vector<std::vector<int64_t>>& residual,
                       int source, int sink, std::vector<int>& parent, Metrics& metrics) {
    for (int i = 0; i < n; ++i) {
        parent[i] = -1;
    }
    parent[source] = source;

    // Queue implemented from scratch via vector and head index
    std::vector<int> q;
    q.reserve(n);
    q.push_back(source);
    size_t head = 0;

    while (head < q.size()) {
        int u = q[head++];

        for (int v = 0; v < n; ++v) {
            metrics.comparisons++;
            // If v is unvisited and residual capacity > 0
            if (parent[v] == -1 && residual[u][v] > 0) {
                parent[v] = u;
                if (v == sink) {
                    return true; // Reached sink
                }
                q.push_back(v);
            }
        }
    }

    return false;
}

} // anonymous namespace

MaxFlowResult edmondsKarpMaxFlow(int numVertices,
                                 const std::vector<std::vector<int64_t>>& capacity,
                                 int source, int sink,
                                 Metrics& metrics) {
    if (numVertices <= 0 || source < 0 || sink < 0 || source == sink) {
        return {0, {}};
    }

    metrics.startTimer();

    // Residual graph initialized to original capacities
    std::vector<std::vector<int64_t>> residual = capacity;
    std::vector<std::vector<int64_t>> flow(numVertices, std::vector<int64_t>(numVertices, 0));
    std::vector<int> parent(numVertices);

    int64_t totalFlow = 0;

    // While an augmenting path exists from source to sink
    while (bfsAugmentingPath(numVertices, residual, source, sink, parent, metrics)) {
        // Step 1: Find bottleneck capacity along augmenting path
        int64_t bottleneck = LLONG_MAX;
        int curr = sink;
        while (curr != source) {
            int p = parent[curr];
            metrics.comparisons++;
            if (residual[p][curr] < bottleneck) {
                bottleneck = residual[p][curr];
            }
            curr = p;
        }

        // Step 2: Augment flow and update residual capacities
        curr = sink;
        while (curr != source) {
            int p = parent[curr];
            residual[p][curr] -= bottleneck;
            residual[curr][p] += bottleneck; // Back-edge residual capacity
            flow[p][curr] += bottleneck;
            flow[curr][p] -= bottleneck;
            curr = p;
        }

        totalFlow += bottleneck;
    }

    metrics.stopTimer();
    return {totalFlow, flow};
}

MatchingResult maxBipartiteMatching(int numLeft, int numRight,
                                    const std::vector<std::pair<int, int>>& bipartiteEdges,
                                    Metrics& metrics) {
    if (numLeft <= 0 || numRight <= 0) {
        return {0, {}};
    }

    metrics.startTimer();

    // Construct flow network:
    // Vertices:
    // 0: Super-source s
    // 1 .. numLeft: Left vertices (readers)
    // numLeft + 1 .. numLeft + numRight: Right vertices (book copies)
    // Total - 1: Super-sink t
    int totalVertices = numLeft + numRight + 2;
    int source = 0;
    int sink = totalVertices - 1;

    std::vector<std::vector<int64_t>> capacity(totalVertices, std::vector<int64_t>(totalVertices, 0));

    // Connect source to all left vertices
    for (int u = 1; u <= numLeft; ++u) {
        capacity[source][u] = 1;
    }

    // Connect all right vertices to sink
    for (int v = 1; v <= numRight; ++v) {
        int rightIndex = numLeft + v;
        capacity[rightIndex][sink] = 1;
    }

    // Add bipartite edges (1-indexed for left and right sets in bipartiteEdges)
    for (const auto& edge : bipartiteEdges) {
        int u = edge.first;   // Left node 0..numLeft-1 or 1..numLeft
        int v = edge.second;  // Right node 0..numRight-1 or 1..numRight
        // Handle 0-indexed inputs
        int leftNode = u + 1;
        int rightNode = numLeft + v + 1;

        if (leftNode <= numLeft && rightNode < sink) {
            capacity[leftNode][rightNode] = 1;
        }
    }

    // Run Edmonds-Karp max flow
    MaxFlowResult flowRes = edmondsKarpMaxFlow(totalVertices, capacity, source, sink, metrics);

    // Extract matched pairs where flow is 1
    MatchingResult matchRes;
    matchRes.maxMatching = static_cast<int>(flowRes.maxFlow);

    for (int u = 1; u <= numLeft; ++u) {
        for (int v = 1; v <= numRight; ++v) {
            int rightIndex = numLeft + v;
            if (flowRes.flowMatrix[u][rightIndex] > 0) {
                matchRes.pairs.push_back({u - 1, v - 1}); // Return 0-indexed pairs
            }
        }
    }

    metrics.stopTimer();
    return matchRes;
}

} // namespace dls
