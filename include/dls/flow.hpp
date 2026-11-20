#pragma once

#include "dls/types.hpp"
#include <vector>
#include <cstdint>
#include <utility>

// NOTE: "struct MaxFlowResult" and "struct MatchingResult" hold network flow answers.
// "std::pair<int, int>" represents a paired match: .first is student ID, .second is book copy ID.

namespace dls {

struct MaxFlowResult {
    int64_t maxFlow{0};
    std::vector<std::vector<int64_t>> flowMatrix;
};

// WHAT:    Computes maximum network throughput between branches using Edmonds-Karp BFS max flow.
// IDEA:    Finds shortest augmenting paths from source to sink in the residual network using BFS.
//          Pushes bottleneck capacity along each path until no augmenting path remains.
// EXAMPLE: Source 0 to Sink 5 with fiber trunk lines -> normal bandwidth 160 Mbps, collapses to 20 Mbps on cut
// TIME:    best O(E) / avg O(V * E^2) / worst O(V * E^2)      SPACE: O(V^2) residual network grid
// VIVA:    Using BFS guarantees that each edge becomes critical at most V/2 times, yielding polynomial runtime.
MaxFlowResult edmondsKarpMaxFlow(int numVertices,
                                 const std::vector<std::vector<int64_t>>& capacity,
                                 int source, int sink,
                                 Metrics& metrics);

struct MatchingResult {
    int maxMatching{0};
    // Pair of (readerId, bookCopyId)
    std::vector<std::pair<int, int>> pairs;
};

// WHAT:    Assigns student book reservation requests to available physical book copies.
// IDEA:    Constructs a flow network with a super-source s to all readers and all copies to super-sink t,
//          all with capacity 1. Runs Edmonds-Karp; non-zero flow edges define the optimal matching.
// EXAMPLE: 5 students wanting books -> matches 4 students uniquely without book contention
// TIME:    best O(E) / avg O(V * E^2) / worst O(V * E^2) (O(V * E) on unit networks)      SPACE: O(V^2)
// VIVA:    Integrality theorem guarantees that with integer capacities (here all 1), max flow values are 0 or 1.
MatchingResult maxBipartiteMatching(int numLeft, int numRight,
                                    const std::vector<std::pair<int, int>>& bipartiteEdges,
                                    Metrics& metrics);

} // namespace dls
