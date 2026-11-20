#pragma once

#include "dls/search.hpp"

// NOTE: ".inl" files contain C++ inline template definitions so the compiler can instantiate
// binary search for any data type T (integers, strings, custom objects).

namespace dls {

namespace detail {

// WHAT:    Recursive helper function that searches arr[low..high] for target.
// IDEA:    Checks midpoint; if not found, recurses on the left half [low..mid-1] or right half [mid+1..high].
// EXAMPLE: binarySearchRecursiveInternal([2, 5, 8], 0, 2, 5) -> returns index 1
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(log n) call stack frames
// VIVA:    Base case 'low > high' handles unfound elements; returning -1 flags search failure.
template <typename T>
int binarySearchRecursiveInternal(const std::vector<T>& arr, int low, int high, const T& target, Metrics& metrics) {
    // WHY: If low pointer passes high pointer, the search interval has shrunk to zero; element is absent.
    if (low > high) {
        return -1; // Target not found in the array
    }

    // WHY: Calculate midpoint using subtraction to prevent signed 32-bit integer overflow (low + high could exceed 2^31 - 1).
    int mid = low + (high - low) / 2;

    metrics.comparisons++; // WHY: Count key comparison for empirical complexity tracking
    // WHY: Check if the element at mid is the target.
    if (arr[mid] == target) {
        return mid; // Target found at index mid; return immediately
    }

    metrics.comparisons++; // WHY: Count comparison against target
    // WHY: If target is smaller than mid element, target must reside in the left half [low..mid-1].
    if (target < arr[mid]) {
        // WHY: mid - 1 because arr[mid] was already checked and proved not equal to target.
        return binarySearchRecursiveInternal(arr, low, mid - 1, target, metrics);
    }

    // WHY: Otherwise, target is greater than arr[mid], so search in the right half [mid+1..high].
    // mid + 1 because arr[mid] was already checked and proved not equal to target.
    return binarySearchRecursiveInternal(arr, mid + 1, high, target, metrics);
}

} // namespace detail

// WHAT:    Iterative binary search algorithm on a sorted vector.
// IDEA:    Maintains low and high integer indices, loops while low <= high, inspects mid,
//          and halves the search range by updating low or high.
// EXAMPLE: binarySearchIterative([2, 5, 8, 12, 16, 23, 38], 23) -> returns index 5
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1) auxiliary space
// VIVA:    Condition 'low <= high' must use '<=' so that single-element search windows (low == high) are checked.
template <typename T>
int binarySearchIterative(const std::vector<T>& arr, const T& target, Metrics& metrics) {
    metrics.startTimer(); // WHY: Start execution stopwatch

    // WHY: Initialize search range boundaries to cover the entire vector.
    int low = 0;
    int high = static_cast<int>(arr.size()) - 1;

    // WHY: Continue while there is at least one candidate element remaining in [low..high].
    while (low <= high) {
        // WHY: Calculate midpoint safely without integer overflow bug.
        int mid = low + (high - low) / 2;

        metrics.comparisons++; // WHY: Track equality comparison against target
        // WHY: If middle element matches target, we are done.
        if (arr[mid] == target) {
            metrics.stopTimer(); // WHY: Stop stopwatch before returning
            return mid;          // Match found at index mid
        }

        metrics.comparisons++; // WHY: Track ordering comparison
        // WHY: If middle element is less than target, target must be in the right half.
        if (arr[mid] < target) {
            // WHY: mid + 1 because mid was already checked; discarding left half and mid.
            low = mid + 1;
        } else {
            // WHY: mid - 1 because mid was already checked; discarding right half and mid.
            high = mid - 1;
        }
    }

    metrics.stopTimer(); // WHY: Stop stopwatch if element is not found
    return -1;           // WHY: Returns -1 to signal that target does not exist in arr
}

// WHAT:    Recursive binary search entry point with timer instrumentation.
// IDEA:    Starts timer, delegates to binarySearchRecursiveInternal across range [0..size-1], stops timer.
// EXAMPLE: binarySearchRecursive([2, 5, 8, 12], 8) -> returns index 2
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(log n) recursion call stack
// VIVA:    Unlike iterative binary search (O(1) space), recursive uses O(log n) stack memory.
template <typename T>
int binarySearchRecursive(const std::vector<T>& arr, const T& target, Metrics& metrics) {
    metrics.startTimer(); // WHY: Start timer
    // WHY: Launch recursive divide-and-conquer search from full range [0..size-1].
    int result = detail::binarySearchRecursiveInternal(arr, 0, static_cast<int>(arr.size()) - 1, target, metrics);
    metrics.stopTimer();  // WHY: Stop timer
    return result;
}

} // namespace dls
