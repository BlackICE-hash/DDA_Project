#pragma once

#include "dls/types.hpp"
#include <vector>
#include <functional>

namespace dls {

// Default comparator using operator<
template <typename T>
struct DefaultLess {
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};

// Document comparator by score descending (higher relevance first)
struct DocumentScoreDesc {
    bool operator()(const Document& a, const Document& b) const {
        if (a.score != b.score) return a.score > b.score;
        return a.id < b.id; // tie-breaker
    }
};

// Document comparator by ID ascending
struct DocumentIdAsc {
    bool operator()(const Document& a, const Document& b) const {
        return a.id < b.id;
    }
};

/**
 * @brief Merge Sort implementation.
 * 
 * IDEA:
 * Divide the array recursively into halves until singletons remain, then merge
 * the sorted sub-arrays using a temporary buffer.
 * 
 * TIME COMPLEXITY:
 * Best: O(n log n), Average: O(n log n), Worst: O(n log n).
 * 
 * SPACE COMPLEXITY:
 * O(n) auxiliary memory for merging buffers.
 * 
 * WHEN IT FAILS:
 * Degrades in performance when memory allocation is slow or memory is strictly constrained.
 */
template <typename T, typename Comp = DefaultLess<T>>
void mergeSort(std::vector<T>& arr, Metrics& metrics, Comp comp = Comp());

/**
 * @brief Quick Sort implementation.
 * 
 * IDEA:
 * Partition the array around a chosen pivot (e.g. median-of-three or last element)
 * such that elements smaller than pivot are placed on the left, greater on the right,
 * then recursively sort both partitions.
 * 
 * TIME COMPLEXITY:
 * Best: O(n log n), Average: O(n log n), Worst: O(n^2) (e.g. naive pivot on already-sorted data).
 * 
 * SPACE COMPLEXITY:
 * O(log n) stack space on average, O(n) worst case.
 * 
 * WHEN IT FAILS:
 * Degrades to O(n^2) if pivot selection repeatedly produces unbalanced 0:(n-1) splits
 * (e.g., sorted or all-identical elements with end-element pivot).
 */
template <typename T, typename Comp = DefaultLess<T>>
void quickSort(std::vector<T>& arr, Metrics& metrics, Comp comp = Comp());

// Overload specifically with choice of pivot strategy (for demonstrating adversarial cases)
enum class PivotStrategy {
    LastElement,    // Naive: worst case O(n^2) on sorted data
    MedianOfThree   // Balanced: robust against sorted data
};

template <typename T, typename Comp = DefaultLess<T>>
void quickSortEx(std::vector<T>& arr, Metrics& metrics, PivotStrategy strategy, Comp comp = Comp());

} // namespace dls

#include "dls/sort.inl"
