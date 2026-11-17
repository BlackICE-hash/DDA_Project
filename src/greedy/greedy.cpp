#include "dls/greedy.hpp"
#include "dls/sort.hpp"
#include <climits>
#include <queue> // NOT used, we use dls::MinHeap!

namespace dls {

// ============================================================================
// 1. ACTIVITY SELECTION
// ============================================================================

/**
 * @brief Activity Selection implementation.
 */
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

namespace {

// Room state in MinHeap: tracks (finishTime, roomId)
struct RoomState {
    int finishTime;
    int roomId;

    bool operator<(const RoomState& other) const {
        if (finishTime != other.finishTime) return finishTime < other.finishTime;
        return roomId < other.roomId;
    }
};

} // anonymous namespace

PartitionResult partitionIntervals(std::vector<Interval> intervals, Metrics& metrics) {
    if (intervals.empty()) return {0, {}};

    metrics.startTimer();

    // Step 1: Sort intervals by increasing start time using custom mergeSort
    mergeSort(intervals, metrics, [](const Interval& a, const Interval& b) {
        if (a.start != b.start) return a.start < b.start;
        return a.finish < b.finish;
    });

    // Custom MinHeap to store active rooms ordered by earliest finish time
    MinHeap<RoomState> roomHeap(std::less<RoomState>(), &metrics);
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

// Custom comparator for HuffmanNode pointers in MinHeap
struct HuffmanNodePtrComp {
    bool operator()(const HuffmanNode* a, const HuffmanNode* b) const {
        return a->freq < b->freq;
    }
};

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

    // Step 2: Populate MinHeap with initial single-character leaf nodes
    MinHeap<HuffmanNode*, HuffmanNodePtrComp> heap(HuffmanNodePtrComp(), &metrics);
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

namespace {

struct DistPair {
    int64_t dist;
    int u;

    bool operator<(const DistPair& other) const {
        return dist < other.dist;
    }
};

} // anonymous namespace

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

    // Step 3: Custom MinHeap for priority queue
    MinHeap<DistPair> pq(std::less<DistPair>(), &metrics);
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

    // Custom MinHeap storing (key, vertex)
    MinHeap<DistPair> pq(std::less<DistPair>(), &metrics);

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
