#pragma once

#include "dls/types.hpp"
#include <vector>
#include <functional>

// NOTE: "template <typename T>" allows these sorting algorithms to sort vectors of any type (int, Document, etc.).
// "struct DocumentScoreDesc" is a comparator struct whose operator() defines custom sorting rules (e.g. highest score first).
// "enum class PivotStrategy" is a strongly-typed list of choices for selecting quicksort pivots (LastElement, MedianOfThree).

namespace dls {

// Default comparator using operator<
template <typename T>
struct DefaultLess {
    // WHAT:    Default comparator returning whether a < b.
    // IDEA:    Applies standard '<' operator on elements of type T.
    // EXAMPLE: DefaultLess<int>()(3, 7) -> true
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Swapping '<' with '>' turns any ascending sort using this comparator into a descending sort.
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};

// Document comparator by score descending (higher relevance first)
struct DocumentScoreDesc {
    // WHAT:    Compares documents by relevance score descending, breaking ties by ID ascending.
    // IDEA:    Checks if a.score > b.score; if scores are identical, picks the smaller document id.
    // EXAMPLE: docA(score=95.0, id=2), docB(score=90.0, id=1) -> true (higher score first)
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Changing 'a.score > b.score' to '<' ranks lowest-scoring documents first.
    bool operator()(const Document& a, const Document& b) const {
        if (a.score != b.score) return a.score > b.score;
        return a.id < b.id; // tie-breaker
    }
};

// Document comparator by ID ascending
struct DocumentIdAsc {
    // WHAT:    Compares two documents by ID in ascending numerical order.
    // IDEA:    Returns true if a.id < b.id.
    // EXAMPLE: docA(id=10), docB(id=25) -> true
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Used to sort catalog documents before binary search lookups.
    bool operator()(const Document& a, const Document& b) const {
        return a.id < b.id;
    }
};

// WHAT:    Sorts an array in-place using the Merge Sort divide-and-conquer algorithm.
// IDEA:    Recursively halves the array into left and right sub-arrays, sorts each half,
//          and merges them into sorted order using an auxiliary buffer.
// EXAMPLE: mergeSort([38, 27, 43, 3, 9, 82, 10]) -> [3, 9, 10, 27, 38, 43, 82]
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(n) auxiliary buffer
// VIVA:    Merge sort is stable and guarantees O(n log n) even on adversarial inputs, but allocates an O(n) buffer.
template <typename T, typename Comp = DefaultLess<T>>
void mergeSort(std::vector<T>& arr, Metrics& metrics, Comp comp = Comp());

// WHAT:    Sorts an array using Quick Sort with Median-of-Three pivot selection.
// IDEA:    Partitions elements around a median-of-three pivot such that smaller items move left
//          and larger move right, then recursively sorts partitions in-place.
// EXAMPLE: quickSort([5, 2, 9, 1, 7]) -> [1, 2, 5, 7, 9]
// TIME:    best O(n log n) / avg O(n log n) / worst O(n^2)      SPACE: O(log n) call stack frames
// VIVA:    Quick sort operates in-place without an auxiliary buffer, making it much faster in practice.
template <typename T, typename Comp = DefaultLess<T>>
void quickSort(std::vector<T>& arr, Metrics& metrics, Comp comp = Comp());

// Overload specifically with choice of pivot strategy (for demonstrating adversarial cases)
enum class PivotStrategy {
    LastElement,    // Naive: worst case O(n^2) on sorted data
    MedianOfThree   // Balanced: robust against sorted data
};

// WHAT:    Sorts an array using Quick Sort with an explicit choice of pivot strategy.
// IDEA:    Allows selecting LastElement pivot (to demonstrate O(n^2) degradation on sorted data)
//          or MedianOfThree pivot (to demonstrate robust O(n log n) partitioning).
// EXAMPLE: quickSortEx(arr, metrics, PivotStrategy::MedianOfThree) -> sorted array
// TIME:    best O(n log n) / avg O(n log n) / worst O(n^2)      SPACE: O(log n) stack
// VIVA:    Testing with LastElement on pre-sorted data shows comparison count jump from O(n log n) to O(n^2).
template <typename T, typename Comp = DefaultLess<T>>
void quickSortEx(std::vector<T>& arr, Metrics& metrics, PivotStrategy strategy, Comp comp = Comp());

} // namespace dls

#include "dls/sort.inl"
