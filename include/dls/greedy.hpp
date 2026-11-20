#pragma once

#include "dls/types.hpp"
#include "dls/generator.hpp"
#include "dls/heap.hpp"
#include "dls/disjoint_set.hpp"
#include <vector>
#include <string>
#include <map>
#include <memory>

// NOTE: "std::pair<int, int>" is a simple two-item package holding .first and .second (e.g. intervalId and roomId).
// "std::map<char, std::string>" is a binary search tree storing character-to-bitstring codes in Huffman coding.
// "std::shared_ptr<HuffmanNode>" is a smart pointer that automatically frees the tree root when no longer used.

namespace dls {

// ============================================================================
// 1. ACTIVITY SELECTION
// ============================================================================

// WHAT:    Greedily selects the maximum number of non-overlapping study activities for a single room.
// IDEA:    Sorts activities by earliest finish time. Greedily picks each activity whose start time
//          is greater than or equal to the finish time of the last selected activity.
// EXAMPLE: Activities (1-4), (3-5), (0-6), (5-7) -> picks (1-4) and (5-7) (2 total)
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(n) for sorted copy
// VIVA:    Sorting by finish time (earliest finish time) is optimal; sorting by start time or duration is sub-optimal.
std::vector<Interval> selectActivities(std::vector<Interval> activities, Metrics& metrics);

// ============================================================================
// 2. INTERVAL PARTITIONING
// ============================================================================

struct PartitionResult {
    int roomsUsed{0};
    // Pair of (intervalId, roomId)
    std::vector<std::pair<int, int>> assignment;
};

// WHAT:    Schedules all study groups using the minimum possible number of conference rooms.
// IDEA:    Sorts intervals by start time. Uses RoomMinHeap to track when each room frees up.
//          Reuses the earliest-finishing room if free; otherwise allocates a new room.
// EXAMPLE: 30 groups needing rooms -> 5 rooms optimally accommodate all without conflict
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(n)
// VIVA:    Number of rooms needed equals the maximum depth (concurrency) of overlapping intervals.
PartitionResult partitionIntervals(std::vector<Interval> intervals, Metrics& metrics);

// ============================================================================
// 3. HUFFMAN CODING
// ============================================================================

struct HuffmanResult {
    std::string encodedBits;
    std::map<char, std::string> codebook;
    std::shared_ptr<HuffmanNode> treeRoot;
    std::string decodedText;
};

// WHAT:    Compresses text abstracts into variable-length prefix-free binary codes.
// IDEA:    Counts character frequencies, pushes leaf nodes into HuffmanMinHeap, and repeatedly
//          merges the two lowest-frequency trees until a single optimal prefix tree remains.
// EXAMPLE: "ABRACADABRA" -> compresses 88 bits down to 28 bits with prefix code tree
// TIME:    best O(k log k) / avg O(k log k) / worst O(k log k) where k is unique characters      SPACE: O(k)
// VIVA:    More frequent characters receive shorter bit sequences, minimizing total weighted path length.
HuffmanResult huffmanEncode(const std::string& text, Metrics& metrics);

// WHAT:    Decodes a bitstring back into original text using the Huffman tree.
// IDEA:    Starts at root; traverses left on '0' and right on '1' until hitting a leaf, then emits character.
// EXAMPLE: bits "0100" with tree -> recovers "AB"
// TIME:    best O(b) / avg O(b) / worst O(b) where b is number of bits      SPACE: O(1)
// VIVA:    Prefix property guarantees that no character's code is a prefix of another, preventing ambiguity.
std::string huffmanDecode(const std::string& bits, const HuffmanNode* root, Metrics& metrics);

// ============================================================================
// 4. DIJKSTRA'S ALGORITHM
// ============================================================================

struct DijkstraResult {
    std::vector<int64_t> distances;
    std::vector<int> predecessors;
};

// WHAT:    Finds shortest delivery paths from a library hub to all other branches with non-negative edge costs.
// IDEA:    Maintains tentative distances initialized to infinity. Uses DistMinHeap to extract the unvisited
//          vertex with minimum tentative distance and relaxes its outgoing edges.
// EXAMPLE: Source 0 in 4-vertex graph -> distances = [0, 10, 25, 30]
// TIME:    best O((V + E) log V) / avg O((V + E) log V) / worst O((V + E) log V)      SPACE: O(V + E)
// VIVA:    Fails on graphs with negative edge weights; Bellman-Ford must be used instead.
DijkstraResult dijkstraShortestPath(int numVertices, const std::vector<Edge>& edges, int source, Metrics& metrics);

// ============================================================================
// 5. PRIM'S ALGORITHM
// ============================================================================

struct MSTResult {
    int64_t totalWeight{0};
    std::vector<Edge> mstEdges;
    bool connected{true};
};

// WHAT:    Computes a Minimum Spanning Tree connecting all branches using Prim's algorithm.
// IDEA:    Grows a single tree starting from vertex 0. Uses DistMinHeap to greedily select
//          the lightest edge crossing from a tree vertex to an unvisited vertex.
// EXAMPLE: 8 library branches -> connects all 8 with 7 transit links at total cost 99
// TIME:    best O((V + E) log V) / avg O((V + E) log V) / worst O((V + E) log V)      SPACE: O(V + E)
// VIVA:    Faster than Kruskal on dense graphs where E ≈ V^2; matches Kruskal's total MST weight.
MSTResult primMST(int numVertices, const std::vector<Edge>& edges, Metrics& metrics);

// ============================================================================
// 6. KRUSKAL'S ALGORITHM
// ============================================================================

// WHAT:    Computes a Minimum Spanning Tree using Kruskal's edge-sorting and Union-Find algorithm.
// IDEA:    Sorts all edges by weight in ascending order. Iterates through edges, adding each edge (u, v)
//          to the MST if and only if DisjointSet::unite(u, v) succeeds (avoiding cycles).
// EXAMPLE: 8 library branches -> selects 7 lightest non-cyclical edges yielding identical cost 99
// TIME:    best O(E log E) / avg O(E log E) / worst O(E log E)      SPACE: O(V + E)
// VIVA:    Uses DisjointSet path compression and union by rank to test for cycles in O(alpha(V)) time.
MSTResult kruskalMST(int numVertices, std::vector<Edge> edges, Metrics& metrics);

} // namespace dls
