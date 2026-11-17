#pragma once

#include "dls/search.hpp"

namespace dls {

namespace detail {

template <typename T>
int binarySearchRecursiveInternal(const std::vector<T>& arr, int low, int high, const T& target, Metrics& metrics) {
    // Base case: target not present in remaining range
    if (low > high) {
        return -1;
    }

    // Step 1: Compute middle index avoiding integer overflow
    int mid = low + (high - low) / 2;

    // Step 2: Check if target is at mid
    metrics.comparisons++;
    if (arr[mid] == target) {
        return mid; // Target found
    }

    // Step 3: Check if target is in left half
    metrics.comparisons++;
    if (target < arr[mid]) {
        return binarySearchRecursiveInternal(arr, low, mid - 1, target, metrics);
    }

    // Step 4: Otherwise search in right half
    return binarySearchRecursiveInternal(arr, mid + 1, high, target, metrics);
}

} // namespace detail

template <typename T>
int binarySearchIterative(const std::vector<T>& arr, const T& target, Metrics& metrics) {
    metrics.startTimer();
    int low = 0;
    int high = static_cast<int>(arr.size()) - 1;

    // Step 1: Loop while search boundaries are valid
    while (low <= high) {
        // Step 2: Calculate midpoint
        int mid = low + (high - low) / 2;

        metrics.comparisons++;
        // Step 3: Check match
        if (arr[mid] == target) {
            metrics.stopTimer();
            return mid;
        }

        metrics.comparisons++;
        // Step 4: Narrow search boundary
        if (arr[mid] < target) {
            low = mid + 1; // Discard left half
        } else {
            high = mid - 1; // Discard right half
        }
    }

    metrics.stopTimer();
    return -1; // Not found
}

template <typename T>
int binarySearchRecursive(const std::vector<T>& arr, const T& target, Metrics& metrics) {
    metrics.startTimer();
    int result = detail::binarySearchRecursiveInternal(arr, 0, static_cast<int>(arr.size()) - 1, target, metrics);
    metrics.stopTimer();
    return result;
}

} // namespace dls
