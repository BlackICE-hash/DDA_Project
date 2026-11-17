#pragma once

#include "dls/types.hpp"
#include <vector>
#include <cstdint>
#include <utility>

namespace dls {

struct MaxFlowResult {
    int64_t maxFlow{0};
    std::vector<std::vector<int64_t>> flowMatrix;
};

/**
 * @brief Edmonds-Karp BFS Implementation of Ford-Fulkerson Max Flow.
 * 
 * IDEA:
 * Computes maximum s-t flow by finding shortest augmenting paths in the residual network
 * using Breadth-First Search (BFS).
 * 
 * TIME COMPLEXITY:
 * O(V * E^2).
 * 
 * SPACE COMPLEXITY:
 * O(V^2) residual network table.
 * 
 * WHEN IT FAILS:
 * Negative capacities or invalid source/sink indices (s == t).
 */
MaxFlowResult edmondsKarpMaxFlow(int numVertices,
                                 const std::vector<std::vector<int64_t>>& capacity,
                                 int source, int sink,
                                 Metrics& metrics);

struct MatchingResult {
    int maxMatching{0};
    // Pair of (readerId, bookCopyId)
    std::vector<std::pair<int, int>> pairs;
};

/**
 * @brief Maximum Bipartite Matching via Max Flow Reduction.
 * 
 * IDEA:
 * Reduces bipartite matching to maximum flow:
 * Adds super-source s connected to left set (readers), super-sink t connected to right set (copies),
 * all edges with capacity 1. Max flow equals maximum matching cardinality.
 * 
 * TIME COMPLEXITY:
 * O(V * E^2) (in practice O(V * E) on unit-capacity networks).
 * 
 * SPACE COMPLEXITY:
 * O(V^2).
 * 
 * WHEN IT FAILS:
 * Empty inputs or vertex index mismatches.
 */
MatchingResult maxBipartiteMatching(int numLeft, int numRight,
                                    const std::vector<std::pair<int, int>>& bipartiteEdges,
                                    Metrics& metrics);

} // namespace dls
