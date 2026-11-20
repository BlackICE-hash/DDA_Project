#include "dls/greedy.hpp"
#include "dls/sort.hpp"
#include <climits>
#include <queue> // NOT used, we use dls::MinHeap!

// NOTE: This file implements Greedy algorithms for digital library management.
// A "Greedy algorithm" always makes the best immediate choice at each step without backtracking.

namespace dls {

// ============================================================================
// 1. ACTIVITY SELECTION
// ============================================================================

// WHAT:    Selects the maximum count of non-overlapping study activities for a single room.
// IDEA:    Sorts activities by earliest finish time. Greedily takes each activity whose start time
//          is >= the finish time of the last scheduled activity.
// EXAMPLE: Activities with intervals (1-4), (3-5), (0-6), (5-7) -> picks (1-4) and (5-7) (2 total)
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(n)
// VIVA:    Sorting by earliest finish time leaves the maximum possible free time for subsequent activities.
std::vector<Interval> selectActivities(std::vector<Interval> activities, Metrics& metrics) {
    if (activities.empty()) return {};

    metrics.startTimer();

    // Step 1: Sort activities by increasing finish time using custom mergeSort
    mergeSort(activities, metrics, [](const Interval& a, const Interval& b) {
        if (a.finish != b.finish) return a.finish < b.finish;
        return a.start < b.start;
    });

    std::vector<Interval> selected;
    // Step 2: Greedily pick the first activity (earliest finish time)
    selected.push_back(activities[0]);
    int lastFinish = activities[0].finish;

    // Step 3: Iterate through remaining activities and greedily select compatible ones
    for (size_t i = 1; i < activities.size(); ++i) {
        metrics.comparisons++;
        // If current activity starts at or after the previous selected activity finishes
        if (activities[i].start >= lastFinish) {
            selected.push_back(activities[i]);
            lastFinish = activities[i].finish; // Update finish threshold
        }
    }

    metrics.stopTimer();
    return selected;
}

// ============================================================================
// 2. INTERVAL PARTITIONING
// ============================================================================

// WHAT:    Schedules all study groups using the minimum possible number of conference rooms.
// IDEA:    Sorts intervals by start time. Uses RoomMinHeap to track room availability;
//          reuses earliest-finishing room if free, otherwise allocates a new room.
// EXAMPLE: 30 overlapping study groups -> scheduled across exactly 5 rooms without conflict
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(n)
// VIVA:    Min-heap stores active rooms; the peak heap size equals the theoretical minimum rooms needed.
PartitionResult partitionIntervals(std::vector<Interval> intervals, Metrics& metrics) {
    if (intervals.empty()) return {0, {}};

    metrics.startTimer();

    // Step 1: Sort intervals by increasing start time using custom mergeSort
    mergeSort(intervals, metrics, [](const Interval& a, const Interval& b) {
        if (a.start != b.start) return a.start < b.start;
        return a.finish < b.finish;
    });

    // Custom non-template RoomMinHeap to store active rooms ordered by earliest finish time
    RoomMinHeap roomHeap(&metrics);
    int nextRoomId = 1;
    PartitionResult result;
    result.assignment.reserve(intervals.size());

    // Step 2: Iterate through each interval in chronological order
    for (const auto& interval : intervals) {
        if (!roomHeap.empty()) {
            metrics.comparisons++;
            RoomState earliestRoom = roomHeap.top();
            // If the earliest room finishes before or when current interval starts
            if (earliestRoom.finishTime <= interval.start) {
                // Reuse this room
                roomHeap.pop();
                result.assignment.push_back({interval.id, earliestRoom.roomId});
                earliestRoom.finishTime = interval.finish; // Extend room busy time
                roomHeap.push(earliestRoom);
                continue;
            }
        }

        // All existing rooms are busy: allocate a brand-new room
        int newRoomId = nextRoomId++;
        result.assignment.push_back({interval.id, newRoomId});
        roomHeap.push({interval.finish, newRoomId});
    }

    result.roomsUsed = nextRoomId - 1;
    metrics.stopTimer();
    return result;
}

// ============================================================================
// 3. HUFFMAN CODING
// ============================================================================

namespace {

// WHAT:    Recursive DFS helper that traverses a Huffman tree to generate prefix bitstrings.
// IDEA:    Traverses left branch appending '0', right branch appending '1'.
//          When a leaf is reached, records codebook[node->ch] = prefix.
// EXAMPLE: Leaf 'A' at path left-right -> codebook['A'] = "01"
// TIME:    best O(k) / avg O(k) / worst O(k) where k is unique characters      SPACE: O(k) recursion stack
// VIVA:    Assigns shorter bitstrings to shallower leaves, giving frequent letters shorter codes.
void buildCodebook(const HuffmanNode* node, const std::string& prefix, std::map<char, std::string>& codebook) {
    if (!node) return;

    // Leaf node: record character code
    if (!node->left && !node->right) {
        // Special case: single unique character in text gets code "0"
        codebook[node->ch] = prefix.empty() ? "0" : prefix;
        return;
    }

    // Traverse left with '0', right with '1'
    buildCodebook(node->left, prefix + "0", codebook);
    buildCodebook(node->right, prefix + "1", codebook);
}

} // anonymous namespace

// WHAT:    Compresses text abstracts into variable-length prefix-free bit sequences.
// IDEA:    Builds frequency map, puts leaf nodes into HuffmanMinHeap, and repeatedly merges
//          the two lowest-frequency trees until a single tree root remains.
// EXAMPLE: "ABRACADABRA" -> compresses 88 bits down to 28 bits (68% savings)
// TIME:    best O(k log k) / avg O(k log k) / worst O(k log k)      SPACE: O(k)
// VIVA:    The two smallest frequencies are always combined first, minimizing total weighted tree depth.
HuffmanResult huffmanEncode(const std::string& text, Metrics& metrics) {
    if (text.empty()) {
        return {"", {}, nullptr, ""};
    }

    metrics.startTimer();

    // Step 1: Count character frequencies
    std::map<char, int> freqMap;
    for (char c : text) {
        freqMap[c]++;
    }

    // Step 2: Populate HuffmanMinHeap with initial single-character leaf nodes
    HuffmanMinHeap heap(&metrics);
    for (const auto& pair : freqMap) {
        heap.push(new HuffmanNode(pair.first, pair.second));
    }

    // Step 3: Iteratively merge two lowest-frequency trees until one root remains
    while (heap.size() > 1) {
        HuffmanNode* left = heap.top();
        heap.pop();
        HuffmanNode* right = heap.top();
        heap.pop();

        // Create internal parent node
        HuffmanNode* parent = new HuffmanNode(left->freq + right->freq, left, right);
        heap.push(parent);
    }

    HuffmanNode* root = heap.top();
    heap.pop();

    // Step 4: Build prefix code dictionary
    std::map<char, std::string> codebook;
    buildCodebook(root, "", codebook);

    // Step 5: Encode input text into bit sequence
    std::string encoded;
    for (char c : text) {
        encoded += codebook[c];
    }

    HuffmanResult result;
    result.encodedBits = encoded;
    result.codebook = codebook;
    result.treeRoot = std::shared_ptr<HuffmanNode>(root);

    // Decode to verify round-trip
    result.decodedText = huffmanDecode(encoded, root, metrics);

    metrics.stopTimer();
    return result;
}

// WHAT:    Decodes a compressed binary bitstring back into original text using the Huffman tree.
// IDEA:    Starts at root, moving left on '0' and right on '1'.
//          When a leaf is hit, appends its character to output and restarts at root.
// EXAMPLE: bits "0100" with tree -> decodes to "AB"
// TIME:    best O(b) / avg O(b) / worst O(b) where b is number of bits      SPACE: O(1)
// VIVA:    No lookahead needed: because it is a prefix-free code, hitting a leaf uniquely determines the character.
std::string huffmanDecode(const std::string& bits, const HuffmanNode* root, Metrics& metrics) {
    if (!root || bits.empty()) return "";

    std::string decoded;
    const HuffmanNode* curr = root;

    for (char bit : bits) {
        metrics.comparisons++;
        // Single character edge case
        if (!curr->left && !curr->right) {
            decoded += curr->ch;
            continue;
        }

        if (bit == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }

        // Reached leaf node: append character and restart from root
        if (curr && !curr->left && !curr->right) {
            decoded += curr->ch;
            curr = root;
        }
    }

    return decoded;
}

// ============================================================================
// 4. DIJKSTRA'S ALGORITHM
// ============================================================================

// WHAT:    Computes shortest transit delivery paths from a source library hub to all branches.
// IDEA:    Maintains distance table initialized to INF. Uses DistMinHeap to extract the unvisited
//          vertex with minimum tentative distance and relaxes its outgoing edges.
// EXAMPLE: Source 0 with 4 branches -> distances [0, 10, 25, 30]
// TIME:    best O((V + E) log V) / avg O((V + E) log V) / worst O((V + E) log V)      SPACE: O(V + E)
// VIVA:    Checking 'd > dist[u]' skips outdated heap entries, implementing lazy deletion.
DijkstraResult dijkstraShortestPath(int numVertices, const std::vector<Edge>& edges, int source, Metrics& metrics) {
    metrics.startTimer();

    // Step 1: Build adjacency list
    std::vector<std::vector<std::pair<int, int>>> adj(numVertices);
    for (const auto& e : edges) {
        adj[e.u].push_back({e.v, e.weight});
    }

    // Step 2: Initialize distance table
    const int64_t INF = LLONG_MAX / 4;
    std::vector<int64_t> dist(numVertices, INF);
    std::vector<int> pred(numVertices, -1);
    dist[source] = 0;

    // Step 3: Custom non-template DistMinHeap for priority queue
    DistMinHeap pq(&metrics);
    pq.push({0, source});

    while (!pq.empty()) {
        DistPair top = pq.top();
        pq.pop();
        int u = top.u;
        int64_t d = top.dist;

        // Skip outdated relaxed pairs in heap
        metrics.comparisons++;
        if (d > dist[u]) continue;

        // Step 4: Relax outgoing edges from vertex u
        for (const auto& neighbor : adj[u]) {
            int v = neighbor.first;
            int64_t weight = neighbor.second;

            metrics.comparisons++;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pred[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    metrics.stopTimer();
    return {dist, pred};
}

// ============================================================================
// 5. PRIM'S ALGORITHM
// ============================================================================

// WHAT:    Finds Minimum Spanning Tree connecting all branches using Prim's algorithm.
// IDEA:    Grows MST from vertex 0 using DistMinHeap to greedily select the lightest edge
//          connecting a tree vertex to an unvisited vertex.
// EXAMPLE: 8 library branches -> connects all 8 with 7 transit links at total cost 99
// TIME:    best O((V + E) log V) / avg O((V + E) log V) / worst O((V + E) log V)      SPACE: O(V + E)
// VIVA:    Maintains 'inMST' array to ensure visited vertices are never added to the tree twice.
MSTResult primMST(int numVertices, const std::vector<Edge>& edges, Metrics& metrics) {
    if (numVertices == 0) return {0, {}, true};

    metrics.startTimer();

    // Step 1: Build undirected adjacency list
    std::vector<std::vector<std::pair<int, int>>> adj(numVertices);
    for (const auto& e : edges) {
        adj[e.u].push_back({e.v, e.weight});
        adj[e.v].push_back({e.u, e.weight});
    }

    const int64_t INF = LLONG_MAX / 4;
    std::vector<int64_t> key(numVertices, INF);
    std::vector<int> parent(numVertices, -1);
    std::vector<bool> inMST(numVertices, false);

    // Custom non-template DistMinHeap storing (key, vertex)
    DistMinHeap pq(&metrics);

    // Start with root vertex 0
    key[0] = 0;
    pq.push({0, 0});

    MSTResult result;
    result.totalWeight = 0;
    int verticesInMST = 0;

    while (!pq.empty()) {
        DistPair top = pq.top();
        pq.pop();
        int u = top.u;

        if (inMST[u]) continue;
        inMST[u] = true;
        verticesInMST++;

        if (parent[u] != -1) {
            result.totalWeight += top.dist;
            result.mstEdges.push_back({parent[u], u, static_cast<int>(top.dist)});
        }

        // Relax neighbors
        for (const auto& neighbor : adj[u]) {
            int v = neighbor.first;
            int weight = neighbor.second;

            metrics.comparisons++;
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }

    result.connected = (verticesInMST == numVertices);
    metrics.stopTimer();
    return result;
}

// ============================================================================
// 6. KRUSKAL'S ALGORITHM
// ============================================================================

// WHAT:    Computes Minimum Spanning Tree using Kruskal's edge sorting and Union-Find algorithm.
// IDEA:    Sorts edges by weight ascending. Iterates through edges; adds edge (u, v) to MST
//          if and only if DisjointSet::unite(u, v) succeeds (detects and avoids cycles).
// EXAMPLE: 8 library branches -> selects 7 lightest non-cyclical edges with total cost 99
// TIME:    best O(E log E) / avg O(E log E) / worst O(E log E)      SPACE: O(V + E)
// VIVA:    Early exit stops immediately when V-1 edges have been added to the MST.
MSTResult kruskalMST(int numVertices, std::vector<Edge> edges, Metrics& metrics) {
    if (numVertices == 0) return {0, {}, true};

    metrics.startTimer();

    // Step 1: Sort all edges by increasing weight using custom mergeSort
    mergeSort(edges, metrics, [](const Edge& a, const Edge& b) {
        return a.weight < b.weight;
    });

    // Step 2: Initialize custom DisjointSet (Union-Find)
    DisjointSet ds(numVertices, &metrics);

    MSTResult result;
    result.totalWeight = 0;

    // Step 3: Iterate through sorted edges and greedily unite disjoint trees
    for (const auto& edge : edges) {
        // If u and v are in different components, unite them and add edge to MST
        if (ds.unite(edge.u, edge.v)) {
            result.mstEdges.push_back(edge);
            result.totalWeight += edge.weight;

            // Early exit if V-1 edges have been added
            if (static_cast<int>(result.mstEdges.size()) == numVertices - 1) {
                break;
            }
        }
    }

    result.connected = (static_cast<int>(result.mstEdges.size()) == numVertices - 1 || numVertices == 1);
    metrics.stopTimer();
    return result;
}

} // namespace dls
