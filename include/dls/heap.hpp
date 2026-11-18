#pragma once

#include "dls/types.hpp"
#include <vector>
#include <utility>
#include <stdexcept>
#include <cstdint>

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

    HuffmanNode(char c, int f) : ch(c), freq(f) {}
    HuffmanNode(int f, HuffmanNode* l, HuffmanNode* r) : freq(f), left(l), right(r) {}
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

inline bool compareDistPair(const DistPair& a, const DistPair& b) {
    if (a.dist != b.dist) return a.dist < b.dist;
    return a.u < b.u;
}

inline bool compareRoomState(const RoomState& a, const RoomState& b) {
    if (a.finishTime != b.finishTime) return a.finishTime < b.finishTime;
    return a.roomId < b.roomId;
}

inline bool compareHuffmanNodePtr(const HuffmanNode* a, const HuffmanNode* b) {
    return a->freq < b->freq;
}

inline bool compareBBNode(const BBNode& a, const BBNode& b) {
    return a.bound > b.bound; // Max-heap behavior: higher bound has higher priority
}

// ----------------------------------------------------------------------------
// Plain non-template MinHeaps for each type actually used
// ----------------------------------------------------------------------------

/**
 * @brief Plain non-template MinHeap for DistPair (used in Dijkstra & Prim).
 */
class DistMinHeap {
private:
    std::vector<DistPair> data_;
    Metrics* metrics_{nullptr};

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
    explicit DistMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    void setMetrics(Metrics* m) { metrics_ = m; }

    void push(const DistPair& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    const DistPair& top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }
    void clear() { data_.clear(); }
};

/**
 * @brief Plain non-template MinHeap for RoomState (used in Interval Partitioning).
 */
class RoomMinHeap {
private:
    std::vector<RoomState> data_;
    Metrics* metrics_{nullptr};

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
    explicit RoomMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    void setMetrics(Metrics* m) { metrics_ = m; }

    void push(const RoomState& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    const RoomState& top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }
    void clear() { data_.clear(); }
};

/**
 * @brief Plain non-template MinHeap for HuffmanNode* (used in Huffman Coding).
 */
class HuffmanMinHeap {
private:
    std::vector<HuffmanNode*> data_;
    Metrics* metrics_{nullptr};

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
    explicit HuffmanMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    void setMetrics(Metrics* m) { metrics_ = m; }

    void push(HuffmanNode* value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    HuffmanNode* top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }
    void clear() { data_.clear(); }
};

/**
 * @brief Plain non-template MinHeap for BBNode (used in Least-Cost Branch & Bound Knapsack).
 */
class BBMinHeap {
private:
    std::vector<BBNode> data_;
    Metrics* metrics_{nullptr};

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
    explicit BBMinHeap(Metrics* m = nullptr) : metrics_(m) {}

    void setMetrics(Metrics* m) { metrics_ = m; }

    void push(const BBNode& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
    }

    const BBNode& top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }
    void clear() { data_.clear(); }
};

} // namespace dls
