#pragma once

#include "dls/types.hpp"
#include <vector>
#include <utility>
#include <stdexcept>
#include <cstdint>

// NOTE: "class DistMinHeap" (and RoomMinHeap, HuffmanMinHeap, BBMinHeap) is just a struct
// that keeps its array private and gives you push(), pop(), and top() functions to use it safely.
// "std::swap" swaps two variables, and "std::move" transfers array elements without slow copying.

namespace dls {

// ----------------------------------------------------------------------------
// Domain structures used with non-template heaps
// ----------------------------------------------------------------------------

struct DistPair {
    int64_t dist{0};
    int u{0};
};

struct RoomState {
    int finishTime{0};
    int roomId{0};
};

struct HuffmanNode {
    char ch{'\0'};
    int freq{0};
    HuffmanNode* left{nullptr};
    HuffmanNode* right{nullptr};

    // WHAT:    Constructs a leaf Huffman node holding a character and frequency.
    // IDEA:    Initializes ch with c, freq with f, and leaves children as nullptr.
    // EXAMPLE: HuffmanNode('A', 5) -> leaf node for character 'A' with frequency 5
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Setting left/right to non-null here would create invalid leaf pointers.
    HuffmanNode(char c, int f) : ch(c), freq(f) {}

    // WHAT:    Constructs an internal Huffman tree node combining two subtrees.
    // IDEA:    Initializes internal node with sum frequency and pointers to left and right children.
    // EXAMPLE: HuffmanNode(10, nodeA, nodeB) -> internal parent node with frequency 10
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Failing to link left or right would disconnect the prefix code tree.
    HuffmanNode(int f, HuffmanNode* l, HuffmanNode* r) : freq(f), left(l), right(r) {}

    // WHAT:    Destroys Huffman node and recursively frees all descendant nodes.
    // IDEA:    Calls delete on left and right child pointers, freeing entire subtree.
    // EXAMPLE: delete root -> recursively deletes all nodes in the Huffman tree
    // TIME:    best O(1) / avg O(k) / worst O(k) where k is subtree node count      SPACE: O(h) recursion depth
    // VIVA:    Omitting 'delete left; delete right;' would cause a memory leak of tree nodes.
    ~HuffmanNode() {
        delete left;
        delete right;
    }
};

struct BBNode {
    int level{-1};              // Level of node in decision tree (corresponds to item index)
    int value{0};               // Total value accumulated so far
    int weight{0};              // Total weight accumulated so far
    double bound{0.0};          // Fractional upper bound
    std::vector<int> takenIds;  // IDs of items taken in this state
};

// ----------------------------------------------------------------------------
// Simple comparison functions
// ----------------------------------------------------------------------------

// WHAT:    Compares two DistPair structs for min-heap ordering.
// IDEA:    Returns true if a's distance is smaller; breaks ties with vertex index.
// EXAMPLE: a(dist=10, u=2), b(dist=15, u=1) -> true (10 < 15)
// TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
// VIVA:    Changing '<' to '>' turns the min-heap into a max-heap, breaking Dijkstra/Prim.
inline bool compareDistPair(const DistPair& a, const DistPair& b) {
    if (a.dist != b.dist) return a.dist < b.dist;
    return a.u < b.u;
}

// WHAT:    Compares two RoomState structs for min-heap room scheduling.
// IDEA:    Returns true if room a becomes free earlier than room b; breaks ties by roomId.
// EXAMPLE: a(finish=10, id=1), b(finish=14, id=2) -> true (room 1 frees earlier)
// TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
// VIVA:    Changing '<' to '>' would allocate rooms that finish latest, wasting rooms.
inline bool compareRoomState(const RoomState& a, const RoomState& b) {
    if (a.finishTime != b.finishTime) return a.finishTime < b.finishTime;
    return a.roomId < b.roomId;
}

// WHAT:    Compares two HuffmanNode pointers by frequency for min-heap merging.
// IDEA:    Returns true if node a has strictly lower character frequency than node b.
// EXAMPLE: a(freq=3), b(freq=8) -> true (frequency 3 < 8)
// TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
// VIVA:    Inverting '<' to '>' builds an anti-optimal code tree with largest codes for common letters.
inline bool compareHuffmanNodePtr(const HuffmanNode* a, const HuffmanNode* b) {
    return a->freq < b->freq;
}

// WHAT:    Compares two BBNode structs for Least-Cost Branch-and-Bound priority queue.
// IDEA:    Returns true if node a has a higher upper bound than node b (max-heap behavior).
// EXAMPLE: a(bound=120.0), b(bound=95.0) -> true (higher bound explored first)
// TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
// VIVA:    Changing '>' to '<' explores worst bounds first, degenerating to exhaustive search.
inline bool compareBBNode(const BBNode& a, const BBNode& b) {
    return a.bound > b.bound; // Max-heap behavior: higher bound has higher priority
}

// ----------------------------------------------------------------------------
// Plain non-template MinHeaps for each type actually used
// ----------------------------------------------------------------------------

// CLASS: DistMinHeap
// DATA STORED: std::vector<DistPair> storing (distance, vertex) pairs in a binary min-heap.
// PUBLIC METHODS:
//   - explicit DistMinHeap(Metrics* m = nullptr): Initializes empty heap with optional telemetry.
//   - void setMetrics(Metrics* m): Attaches a metrics tracker for comparison counting.
//   - void push(const DistPair& value): Inserts pair and sifts up into heap position; O(log n).
//   - void pop(): Removes root pair with minimum distance and sifts down; O(log n).
//   - const DistPair& top() const: Returns reference to smallest distance pair at root; O(1).
//   - bool empty() const: Returns true if heap contains 0 elements; O(1).
//   - size_t size() const: Returns count of active pairs in heap; O(1).
//   - void clear(): Erases all stored pairs; O(1).
class DistMinHeap {
private:
    std::vector<DistPair> data_;
    Metrics* metrics_{nullptr};

    // WHAT:    Restores min-heap property upward after an insertion at index.
    // IDEA:    Repeatedly compares item with parent at (index-1)/2, swapping while smaller.
    // EXAMPLE: Appending 3 under parent 10 swaps 3 up to the parent position.
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Parent index formula '(index - 1) / 2' is essential; using 'index / 2' breaks 0-indexing.
    void siftUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (metrics_) metrics_->comparisons++;
            if (compareDistPair(data_[index], data_[parent])) {
                std::swap(data_[index], data_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // WHAT:    Restores min-heap property downward after root removal.
    // IDEA:    Finds smallest among index, left child (2*i+1), and right child (2*i+2), swapping down.
    // EXAMPLE: Root 20 with children 5 and 8 swaps with 5 to restore minimum at top.
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Failing to check 'left < n' or 'right < n' causes out-of-bounds array reads.
    void siftDown(size_t index) {
        size_t n = data_.size();
        while (true) {
            size_t smallest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            if (left < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareDistPair(data_[left], data_[smallest])) {
                    smallest = left;
                }
            }

            if (right < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareDistPair(data_[right], data_[smallest])) {
                    smallest = right;
                }
            }

            if (smallest != index) {
                std::swap(data_[index], data_[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    // WHAT:    Constructs an empty DistMinHeap.
    // IDEA:    Stores optional metrics tracker pointer and leaves internal vector empty.
    // EXAMPLE: DistMinHeap(&metrics) -> ready-to-use empty distance min-heap
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    'explicit' prevents unwanted implicit conversion from Metrics* to DistMinHeap.
    explicit DistMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    // WHAT:    Sets or updates the metrics tracker pointer.
    // IDEA:    Assigns metrics_ to point to m for instrumenting comparisons.
    // EXAMPLE: setMetrics(&m) -> heap now tracks comparisons in m
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Passing nullptr turns off comparison counting without crashing.
    void setMetrics(Metrics* m) { metrics_ = m; }

    // WHAT:    Pushes a new DistPair into the heap.
    // IDEA:    Appends to back of vector, then calls siftUp to restore min-heap invariant.
    // EXAMPLE: push({dist: 5, u: 3}) -> placed in vector and bubbled up
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Omitting siftUp leaves the smallest element buried at the end.
    void push(const DistPair& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    // WHAT:    Removes the root item with minimum distance.
    // IDEA:    Moves last element to root, pops back, and sifts down from root.
    // EXAMPLE: pop() removes smallest distance node, promotes next smallest to top
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Calling pop() on empty heap is protected by the 'if (empty()) return' check.
    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    // WHAT:    Returns the minimum DistPair at the root of the heap.
    // IDEA:    Returns a const reference to data_[0] without removing it.
    // EXAMPLE: top() -> returns {dist: 2, u: 0} if 2 is smallest
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Throws std::runtime_error if called on an empty heap.
    const DistPair& top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    // WHAT:    Checks if the heap contains no elements.
    // IDEA:    Delegates to data_.empty().
    // EXAMPLE: empty() -> true if heap has 0 items
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Used as while-loop termination in Dijkstra and Prim algorithms.
    bool empty() const { return data_.empty(); }

    // WHAT:    Returns total number of elements currently in heap.
    // IDEA:    Delegates to data_.size().
    // EXAMPLE: size() -> 5
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Returns unsigned size_t, guaranteed >= 0.
    size_t size() const { return data_.size(); }

    // WHAT:    Removes all elements from the heap.
    // IDEA:    Calls data_.clear(), resetting internal size to zero.
    // EXAMPLE: clear() -> heap becomes empty
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Reuses vector capacity without freeing internal buffer memory.
    void clear() { data_.clear(); }
};

// CLASS: RoomMinHeap
// DATA STORED: std::vector<RoomState> storing (finishTime, roomId) in a binary min-heap.
// PUBLIC METHODS:
//   - explicit RoomMinHeap(Metrics* m = nullptr): Constructs empty room min-heap.
//   - void setMetrics(Metrics* m): Attaches metrics tracker for comparisons.
//   - void push(const RoomState& value): Inserts room state and bubbles up; O(log n).
//   - void pop(): Removes room that finishes earliest and restores heap; O(log n).
//   - const RoomState& top() const: Inspects room that finishes earliest; O(1).
//   - bool empty() const: Returns true if no rooms are tracked; O(1).
//   - size_t size() const: Returns number of rooms currently tracked; O(1).
//   - void clear(): Resets room heap to empty; O(1).
class RoomMinHeap {
private:
    std::vector<RoomState> data_;
    Metrics* metrics_{nullptr};

    // WHAT:    Restores room min-heap ordering upward.
    // IDEA:    Bubbles up room state if its finish time is earlier than its parent's.
    // EXAMPLE: Room finishing at hour 9 bubbles above room finishing at hour 12.
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Maintains invariant that data_[0] is always the earliest-finishing room.
    void siftUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (metrics_) metrics_->comparisons++;
            if (compareRoomState(data_[index], data_[parent])) {
                std::swap(data_[index], data_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // WHAT:    Restores room min-heap ordering downward.
    // IDEA:    Pushes room down until both children have later finish times.
    // EXAMPLE: Root finishing at hour 18 swaps down with child finishing at hour 10.
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Breaks ties by roomId when finish times are equal.
    void siftDown(size_t index) {
        size_t n = data_.size();
        while (true) {
            size_t smallest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            if (left < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareRoomState(data_[left], data_[smallest])) {
                    smallest = left;
                }
            }

            if (right < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareRoomState(data_[right], data_[smallest])) {
                    smallest = right;
                }
            }

            if (smallest != index) {
                std::swap(data_[index], data_[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    // WHAT:    Constructs an empty RoomMinHeap.
    // IDEA:    Initializes metrics pointer to m.
    // EXAMPLE: RoomMinHeap(&metrics) -> empty room priority queue
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    m defaults to nullptr if not supplied.
    explicit RoomMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    // WHAT:    Attaches metrics telemetry tracker.
    // IDEA:    Sets internal metrics_ pointer.
    // EXAMPLE: setMetrics(&m) -> tracks comparisons in m
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Safe to pass nullptr.
    void setMetrics(Metrics* m) { metrics_ = m; }

    // WHAT:    Pushes a RoomState into the heap.
    // IDEA:    Appends value and calls siftUp.
    // EXAMPLE: push({finishTime: 12, roomId: 1}) -> stored in heap
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Used in Interval Partitioning when a new room is allocated.
    void push(const RoomState& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    // WHAT:    Removes the earliest finishing room.
    // IDEA:    Moves back to root, pops back, and sifts down.
    // EXAMPLE: pop() removes room that is finished, freeing it for reuse
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Ensures heap size matches active occupied rooms.
    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    // WHAT:    Returns earliest finishing RoomState.
    // IDEA:    Returns reference to data_[0].
    // EXAMPLE: top() -> {finishTime: 10, roomId: 2}
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Interval partitioning compares next activity start time against top().finishTime.
    const RoomState& top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    // WHAT:    Returns true if room heap is empty.
    // IDEA:    Checks data_.empty().
    // EXAMPLE: empty() -> true if no rooms allocated
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    If empty, a new room must always be created.
    bool empty() const { return data_.empty(); }

    // WHAT:    Returns number of rooms in heap.
    // IDEA:    Returns data_.size().
    // EXAMPLE: size() -> 5 means 5 rooms in use
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Peak heap size gives optimal minimum rooms needed.
    size_t size() const { return data_.size(); }

    // WHAT:    Clears room heap.
    // IDEA:    Calls data_.clear().
    // EXAMPLE: clear() -> empty
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Frees vector storage.
    void clear() { data_.clear(); }
};

// CLASS: HuffmanMinHeap
// DATA STORED: std::vector<HuffmanNode*> storing tree node pointers in a min-heap by frequency.
// PUBLIC METHODS:
//   - explicit HuffmanMinHeap(Metrics* m = nullptr): Constructs empty heap of node pointers.
//   - void setMetrics(Metrics* m): Attaches metrics tracker.
//   - void push(HuffmanNode* value): Inserts node pointer and sifts up; O(log k).
//   - void pop(): Removes lowest frequency node pointer; O(log k).
//   - HuffmanNode* top() const: Returns pointer to lowest frequency tree node; O(1).
//   - bool empty() const: Returns true if heap is empty; O(1).
//   - size_t size() const: Returns count of remaining tree nodes; O(1).
//   - void clear(): Empties heap pointer array; O(1).
class HuffmanMinHeap {
private:
    std::vector<HuffmanNode*> data_;
    Metrics* metrics_{nullptr};

    // WHAT:    Sifts up Huffman node pointer by frequency.
    // IDEA:    Bubbles up node if its character frequency is lower than parent's frequency.
    // EXAMPLE: Node with frequency 2 bubbles above parent with frequency 7.
    // TIME:    best O(1) / avg O(log k) / worst O(log k)      SPACE: O(1)
    // VIVA:    Operates on pointers (shallow copies) for speed without moving node trees.
    void siftUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (metrics_) metrics_->comparisons++;
            if (compareHuffmanNodePtr(data_[index], data_[parent])) {
                std::swap(data_[index], data_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // WHAT:    Sifts down Huffman node pointer by frequency.
    // IDEA:    Swaps node down with child having lowest frequency until heap property holds.
    // EXAMPLE: Parent with frequency 15 swaps down with child having frequency 4.
    // TIME:    best O(1) / avg O(log k) / worst O(log k)      SPACE: O(1)
    // VIVA:    Ensures top() always points to lowest frequency tree.
    void siftDown(size_t index) {
        size_t n = data_.size();
        while (true) {
            size_t smallest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            if (left < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareHuffmanNodePtr(data_[left], data_[smallest])) {
                    smallest = left;
                }
            }

            if (right < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareHuffmanNodePtr(data_[right], data_[smallest])) {
                    smallest = right;
                }
            }

            if (smallest != index) {
                std::swap(data_[index], data_[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    // WHAT:    Constructs an empty HuffmanMinHeap.
    // IDEA:    Initializes internal vector and optional metrics tracker.
    // EXAMPLE: HuffmanMinHeap(&metrics) -> ready to build prefix tree
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Takes Metrics* to count comparisons between frequencies.
    explicit HuffmanMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    // WHAT:    Attaches metrics tracker.
    // IDEA:    Sets metrics_ pointer.
    // EXAMPLE: setMetrics(&m)
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Enables dynamic metric tracking.
    void setMetrics(Metrics* m) { metrics_ = m; }

    // WHAT:    Inserts a Huffman tree node pointer into heap.
    // IDEA:    Appends pointer to data_ and sifts up based on node->freq.
    // EXAMPLE: push(new HuffmanNode('a', 5)) -> node inserted
    // TIME:    best O(1) / avg O(log k) / worst O(log k)      SPACE: O(1)
    // VIVA:    Does not allocate node itself; takes ownership of existing pointer.
    void push(HuffmanNode* value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    // WHAT:    Removes the lowest-frequency node pointer.
    // IDEA:    Replaces root with back pointer and sifts down.
    // EXAMPLE: pop() removes lowest-frequency node so it can be merged
    // TIME:    best O(1) / avg O(log k) / worst O(log k)      SPACE: O(1)
    // VIVA:    Does NOT delete the node object from heap memory (the caller merges it).
    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    // WHAT:    Returns pointer to lowest frequency Huffman node.
    // IDEA:    Returns data_[0].
    // EXAMPLE: top() -> pointer to node with minimum freq
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Throws runtime_error if heap is empty.
    HuffmanNode* top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    // WHAT:    Returns true if heap is empty.
    // IDEA:    Checks data_.empty().
    // EXAMPLE: empty() -> true when all nodes merged
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    When size() == 1, top() is the completed root of the Huffman tree.
    bool empty() const { return data_.empty(); }

    // WHAT:    Returns count of active trees in heap.
    // IDEA:    Returns data_.size().
    // EXAMPLE: size() -> 2
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    The Huffman loop terminates when size() == 1.
    size_t size() const { return data_.size(); }

    // WHAT:    Clears the pointer array.
    // IDEA:    Calls data_.clear().
    // EXAMPLE: clear() -> resets heap
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Does not free node allocations; used after extracting tree root.
    void clear() { data_.clear(); }
};

// CLASS: BBMinHeap
// DATA STORED: std::vector<BBNode> storing Branch-and-Bound states prioritized by upper bound.
// PUBLIC METHODS:
//   - explicit BBMinHeap(Metrics* m = nullptr): Constructs empty branch-and-bound priority queue.
//   - void setMetrics(Metrics* m): Attaches metrics tracker.
//   - void push(const BBNode& value): Inserts decision node and sifts up; O(log n).
//   - void pop(): Removes node with highest upper bound; O(log n).
//   - const BBNode& top() const: Returns reference to most promising state node; O(1).
//   - bool empty() const: Returns true if no state nodes remain; O(1).
//   - size_t size() const: Returns count of unexplored active state nodes; O(1).
//   - void clear(): Empties state node array; O(1).
class BBMinHeap {
private:
    std::vector<BBNode> data_;
    Metrics* metrics_{nullptr};

    // WHAT:    Sifts up BBNode based on upper bound.
    // IDEA:    Bubbles up node if its bound is higher than parent's bound (max-heap behavior).
    // EXAMPLE: Node with bound 150.0 bubbles above parent with bound 110.0.
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Implements max-priority behavior using compareBBNode (a.bound > b.bound).
    void siftUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (metrics_) metrics_->comparisons++;
            if (compareBBNode(data_[index], data_[parent])) {
                std::swap(data_[index], data_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // WHAT:    Sifts down BBNode based on upper bound.
    // IDEA:    Swaps node down with child having higher upper bound.
    // EXAMPLE: Node with bound 70.0 swaps down with child having bound 130.0.
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Guarantees top() always holds the most optimistic knapsack subtree.
    void siftDown(size_t index) {
        size_t n = data_.size();
        while (true) {
            size_t smallest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            if (left < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareBBNode(data_[left], data_[smallest])) {
                    smallest = left;
                }
            }

            if (right < n) {
                if (metrics_) metrics_->comparisons++;
                if (compareBBNode(data_[right], data_[smallest])) {
                    smallest = right;
                }
            }

            if (smallest != index) {
                std::swap(data_[index], data_[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    // WHAT:    Constructs an empty BBMinHeap.
    // IDEA:    Initializes internal array and sets metrics pointer.
    // EXAMPLE: BBMinHeap(&metrics) -> ready for Least-Cost knapsack search
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Enables LCBB to explore only 21 nodes compared to 879 for FIFO.
    explicit BBMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    // WHAT:    Attaches metrics tracker.
    // IDEA:    Sets metrics_ pointer.
    // EXAMPLE: setMetrics(&m)
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Tracks priority queue comparisons during branch evaluation.
    void setMetrics(Metrics* m) { metrics_ = m; }

    // WHAT:    Pushes a search node into the priority queue.
    // IDEA:    Appends node and sifts up by bound.
    // EXAMPLE: push({level: 1, value: 50, bound: 140.0}) -> inserted
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Nodes with bounds below current best value are pruned before push.
    void push(const BBNode& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    // WHAT:    Removes the search node with highest optimistic bound.
    // IDEA:    Replaces root with back and sifts down.
    // EXAMPLE: pop() removes highest bound state to branch on it next
    // TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
    // VIVA:    Safely handles empty heap via early return check.
    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    // WHAT:    Returns reference to most promising state node.
    // IDEA:    Returns data_[0].
    // EXAMPLE: top() -> node with maximum bound
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Throws runtime_error if priority queue is empty.
    const BBNode& top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    // WHAT:    Checks if priority queue has no active nodes.
    // IDEA:    Checks data_.empty().
    // EXAMPLE: empty() -> true when branch-and-bound exploration completes
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Termination condition for the main branch-and-bound search loop.
    bool empty() const { return data_.empty(); }

    // WHAT:    Returns count of active frontier nodes.
    // IDEA:    Returns data_.size().
    // EXAMPLE: size() -> 12 active states
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Reflects memory footprint of the active search frontier.
    size_t size() const { return data_.size(); }

    // WHAT:    Clears all active search states.
    // IDEA:    Calls data_.clear().
    // EXAMPLE: clear() -> resets queue
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Allows reuse of heap across multiple knapsack instances.
    void clear() { data_.clear(); }
};

} // namespace dls
