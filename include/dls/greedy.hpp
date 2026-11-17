#pragma once

#include "dls/types.hpp"
#include "dls/generator.hpp"
#include "dls/heap.hpp"
#include "dls/disjoint_set.hpp"
#include <vector>
#include <string>
#include <map>
#include <memory>

namespace dls {

// ============================================================================
// 1. ACTIVITY SELECTION
// ============================================================================

/**
 * @brief Activity Selection Problem.
 * 
 * IDEA:
 * Greedily selects the activity that finishes earliest among remaining compatible activities.
 * 
 * TIME COMPLEXITY:
 * O(n log n) sorting + O(n) greedy selection.
 * 
 * SPACE COMPLEXITY:
 * O(n) for sorted copy and output list.
 * 
 * WHEN IT FAILS:
 * Cannot find optimal set if activities have varying weights/values (requires dynamic programming).
 */
std::vector<Interval> selectActivities(std::vector<Interval> activities, Metrics& metrics);

// ============================================================================
// 2. INTERVAL PARTITIONING
// ============================================================================

struct PartitionResult {
    int roomsUsed{0};
    // Pair of (intervalId, roomId)
    std::vector<std::pair<int, int>> assignment;
};

/**
 * @brief Interval Partitioning (Room Scheduling).
 * 
 * IDEA:
 * Sort intervals by start time. Use a min-heap tracking current rooms by earliest finish time.
 * For each interval, reuse the earliest finishing room if available; otherwise allocate a new room.
 * 
 * TIME COMPLEXITY:
 * O(n log n) sorting + O(n log d) min-heap operations (d = number of rooms).
 * 
 * SPACE COMPLEXITY:
 * O(n) for allocations and heap.
 * 
 * WHEN IT FAILS:
 * Assumes intervals cannot be rescheduled or split.
 */
PartitionResult partitionIntervals(std::vector<Interval> intervals, Metrics& metrics);

// ============================================================================
// 3. HUFFMAN CODING
// ============================================================================

struct HuffmanNode {
    char ch{'\0'};
    int freq{0};
    HuffmanNode* left{nullptr};
    HuffmanNode* right{nullptr};

    HuffmanNode(char c, int f) : ch(c), freq(f) {}
    HuffmanNode(int f, HuffmanNode* l, HuffmanNode* r) : freq(f), left(l), right(r) {}
    ~HuffmanNode() {
        delete left;
        delete right;
    }
};

struct HuffmanResult {
    std::string encodedBits;
    std::map<char, std::string> codebook;
    std::shared_ptr<HuffmanNode> treeRoot;
    std::string decodedText;
};

/**
 * @brief Huffman Coding for Text Compression.
 * 
 * IDEA:
 * Build optimal prefix code tree by iteratively merging the two lowest-frequency trees
 * extracted from a min-heap.
 * 
 * TIME COMPLEXITY:
 * O(k log k) where k is alphabet size.
 * 
 * SPACE COMPLEXITY:
 * O(k) for tree and dictionary.
 * 
 * WHEN IT FAILS:
 * Fails to compress if text is empty or character frequencies are uniformly distributed.
 */
HuffmanResult huffmanEncode(const std::string& text, Metrics& metrics);
std::string huffmanDecode(const std::string& bits, const HuffmanNode* root, Metrics& metrics);

// ============================================================================
// 4. DIJKSTRA'S ALGORITHM
// ============================================================================

struct DijkstraResult {
    std::vector<int64_t> distances;
    std::vector<int> predecessors;
};

/**
 * @brief Dijkstra's Single-Source Shortest Path.
 * 
 * IDEA:
 * Greedily relaxes edges using a custom MinHeap to extract the unvisited vertex with
 * smallest tentative distance.
 * 
 * TIME COMPLEXITY:
 * O((V + E) log V).
 * 
 * SPACE COMPLEXITY:
 * O(V + E) for adjacency list and distances.
 * 
 * WHEN IT FAILS:
 * Produces incorrect results or loops if graph contains negative edge weights (use Bellman-Ford).
 */
DijkstraResult dijkstraShortestPath(int numVertices, const std::vector<Edge>& edges, int source, Metrics& metrics);

// ============================================================================
// 5. PRIM'S ALGORITHM
// ============================================================================

struct MSTResult {
    int64_t totalWeight{0};
    std::vector<Edge> mstEdges;
    bool connected{true};
};

/**
 * @brief Prim's Minimum Spanning Tree.
 * 
 * IDEA:
 * Grows a single tree from vertex 0, greedily choosing the lightest edge connecting a
 * tree vertex to a non-tree vertex via a MinHeap.
 * 
 * TIME COMPLEXITY:
 * O((V + E) log V).
 * 
 * SPACE COMPLEXITY:
 * O(V + E).
 * 
 * WHEN IT FAILS:
 * If graph is disconnected (detected and marked in MSTResult.connected).
 */
MSTResult primMST(int numVertices, const std::vector<Edge>& edges, Metrics& metrics);

// ============================================================================
// 6. KRUSKAL'S ALGORITHM
// ============================================================================

/**
 * @brief Kruskal's Minimum Spanning Tree with Union-Find.
 * 
 * IDEA:
 * Sorts all edges by weight, then iterates through sorted edges, adding edge (u, v)
 * to MST if and only if u and v are in different connected components (preventing cycles).
 * 
 * TIME COMPLEXITY:
 * O(E log E) + O(E alpha(V)).
 * 
 * SPACE COMPLEXITY:
 * O(V + E).
 * 
 * WHEN IT FAILS:
 * If graph is disconnected (MST will contain fewer than V-1 edges).
 */
MSTResult kruskalMST(int numVertices, std::vector<Edge> edges, Metrics& metrics);

} // namespace dls
