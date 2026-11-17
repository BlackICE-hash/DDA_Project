#pragma once

#include "dls/types.hpp"
#include <vector>
#include <utility>
#include <stdexcept>

namespace dls {

/**
 * @brief Custom binary Min-Heap implemented from scratch.
 * 
 * IDEA:
 * Array-based complete binary tree where parent node at index i satisfies
 * heap property: comp(tree[i], tree[child]) is true (i.e. parent <= child).
 * Children of index i are at 2*i + 1 and 2*i + 2.
 * 
 * TIME COMPLEXITY:
 * push: O(log n), pop: O(log n), top: O(1).
 * 
 * SPACE COMPLEXITY:
 * O(n) storage in std::vector.
 */
template <typename T, typename Comp = std::less<T>>
class MinHeap {
private:
    std::vector<T> data_;
    Comp comp_;
    Metrics* metrics_{nullptr};

    void siftUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (metrics_) metrics_->comparisons++;
            // If data_[index] < data_[parent], restore heap invariant
            if (comp_(data_[index], data_[parent])) {
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
                if (comp_(data_[left], data_[smallest])) {
                    smallest = left;
                }
            }

            if (right < n) {
                if (metrics_) metrics_->comparisons++;
                if (comp_(data_[right], data_[smallest])) {
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
    MinHeap(Comp comp = Comp(), Metrics* m = nullptr)
        : comp_(comp), metrics_(m) {}

    void setMetrics(Metrics* m) {
        metrics_ = m;
    }

    void push(const T& value) {
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

    const T& top() const {
        if (data_.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data_[0];
    }

    bool empty() const {
        return data_.empty();
    }

    size_t size() const {
        return data_.size();
    }

    void clear() {
        data_.clear();
    }
};

} // namespace dls
