#pragma once

#include "dls/sort.hpp"
#include <utility>

namespace dls {

namespace detail {

/**
 * Merge two sorted contiguous sub-ranges: [left..mid] and [mid+1..right].
 * Uses an auxiliary buffer to temporarily store elements and copies back.
 */
template <typename T, typename Comp>
void merge(std::vector<T>& arr, std::vector<T>& buffer, int left, int mid, int right, Metrics& metrics, Comp comp) {
    // Step 1: Copy active range into temporary auxiliary buffer
    for (int idx = left; idx <= right; ++idx) {
        buffer[idx] = arr[idx];
    }

    int i = left;      // Pointer to beginning of left half
    int j = mid + 1;   // Pointer to beginning of right half
    int k = left;      // Pointer to current output insertion index in original array

    // Step 2: Compare elements from both halves and place smaller element into arr
    while (i <= mid && j <= right) {
        metrics.comparisons++; // Count key comparison
        if (comp(buffer[i], buffer[j])) {
            arr[k++] = buffer[i++];
        } else {
            arr[k++] = buffer[j++];
        }
    }

    // Step 3: Copy remaining elements from left half (if any)
    while (i <= mid) {
        arr[k++] = buffer[i++];
    }

    // Step 4: Copy remaining elements from right half (if any)
    while (j <= right) {
        arr[k++] = buffer[j++];
    }
}

/**
 * Recursive merge sort worker function.
 */
template <typename T, typename Comp>
void mergeSortInternal(std::vector<T>& arr, std::vector<T>& buffer, int left, int right, Metrics& metrics, Comp comp) {
    // Base case: 0 or 1 element is already sorted
    if (left >= right) {
        return;
    }

    // Step 1: Compute midpoint to divide range into two halves
    int mid = left + (right - left) / 2;

    // Step 2: Recursively sort left half [left..mid]
    mergeSortInternal(arr, buffer, left, mid, metrics, comp);

    // Step 3: Recursively sort right half [mid+1..right]
    mergeSortInternal(arr, buffer, mid + 1, right, metrics, comp);

    // Step 4: Combine the two sorted halves
    merge(arr, buffer, left, mid, right, metrics, comp);
}

/**
 * Helper to select pivot index based on strategy.
 */
template <typename T, typename Comp>
int choosePivot(std::vector<T>& arr, int low, int high, PivotStrategy strategy, Metrics& metrics, Comp comp) {
    if (strategy == PivotStrategy::LastElement) {
        return high; // Naive: last element
    }

    // Median of Three: low, mid, high
    int mid = low + (high - low) / 2;

    metrics.comparisons++;
    if (comp(arr[mid], arr[low])) {
        std::swap(arr[low], arr[mid]);
    }
    metrics.comparisons++;
    if (comp(arr[high], arr[low])) {
        std::swap(arr[low], arr[high]);
    }
    metrics.comparisons++;
    if (comp(arr[high], arr[mid])) {
        std::swap(arr[mid], arr[high]);
    }

    // Move median value to high position to use standard Lomuto partition
    std::swap(arr[mid], arr[high]);
    return high;
}

/**
 * Lomuto partition scheme:
 * Places pivot at correct position, elements smaller to left, larger to right.
 */
template <typename T, typename Comp>
int partition(std::vector<T>& arr, int low, int high, PivotStrategy strategy, Metrics& metrics, Comp comp) {
    // Step 1: Select pivot and place it at position 'high'
    choosePivot(arr, low, high, strategy, metrics, comp);
    const T pivot = arr[high];

    // Step 2: Index of smaller element boundary
    int i = low - 1;

    // Step 3: Iterate through elements and swap smaller ones to the left
    for (int j = low; j < high; ++j) {
        metrics.comparisons++; // Count key comparison
        if (comp(arr[j], pivot)) {
            i++;
            std::swap(arr[i], arr[j]); // Move smaller element to left partition
        }
    }

    // Step 4: Place pivot in its final sorted position
    std::swap(arr[i + 1], arr[high]);
    return i + 1; // Return pivot position
}

/**
 * Recursive quicksort worker function.
 */
template <typename T, typename Comp>
void quickSortInternal(std::vector<T>& arr, int low, int high, PivotStrategy strategy, Metrics& metrics, Comp comp) {
    // Base case: range has 0 or 1 element
    if (low >= high) {
        return;
    }

    // Step 1: Partition array and retrieve pivot position
    int p = partition(arr, low, high, strategy, metrics, comp);

    // Step 2: Recursively sort elements before pivot
    quickSortInternal(arr, low, p - 1, strategy, metrics, comp);

    // Step 3: Recursively sort elements after pivot
    quickSortInternal(arr, p + 1, high, strategy, metrics, comp);
}

} // namespace detail

/**
 * Merge Sort top-level entry point.
 */
template <typename T, typename Comp>
void mergeSort(std::vector<T>& arr, Metrics& metrics, Comp comp) {
    if (arr.size() <= 1) return;

    metrics.startTimer();
    metrics.peakBytes = sizeof(T) * arr.size(); // Auxiliary buffer size
    std::vector<T> buffer(arr.size());

    detail::mergeSortInternal(arr, buffer, 0, static_cast<int>(arr.size()) - 1, metrics, comp);
    metrics.stopTimer();
}

/**
 * Quick Sort top-level entry point (defaults to MedianOfThree for stability).
 */
template <typename T, typename Comp>
void quickSort(std::vector<T>& arr, Metrics& metrics, Comp comp) {
    quickSortEx(arr, metrics, PivotStrategy::MedianOfThree, comp);
}

/**
 * Quick Sort with explicit pivot strategy (e.g. LastElement to demonstrate adversarial worst case).
 */
template <typename T, typename Comp>
void quickSortEx(std::vector<T>& arr, Metrics& metrics, PivotStrategy strategy, Comp comp) {
    if (arr.size() <= 1) return;

    metrics.startTimer();
    detail::quickSortInternal(arr, 0, static_cast<int>(arr.size()) - 1, strategy, metrics, comp);
    metrics.stopTimer();
}

} // namespace dls
