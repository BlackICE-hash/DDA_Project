#pragma once

#include "dls/types.hpp"
#include <vector>

// NOTE: "template <typename T>" allows these binary search functions to search any sorted vector (ints, strings, etc.).
// "binarySearchDocumentById" is a specialized non-template wrapper that searches documents by their unique integer ID.

namespace dls {

// WHAT:    Searches a sorted array for a target value using an iterative loop.
// IDEA:    Maintains low and high pointers, repeatedly computes mid, and halves search space
//          until the target is found or boundaries cross.
// EXAMPLE: binarySearchIterative([2, 5, 8, 12, 16, 23, 38], 23) -> index 5
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
// VIVA:    Fails if the input array is unsorted; mid + 1 and mid - 1 prevent infinite loops.
template <typename T>
int binarySearchIterative(const std::vector<T>& arr, const T& target, Metrics& metrics);

// WHAT:    Searches a sorted array for a target value using recursive divide-and-conquer calls.
// IDEA:    Checks arr[mid]; if unequal, recursively calls itself on either the left half [low..mid-1]
//          or right half [mid+1..high]. Recurrence: T(n) = T(n/2) + 1.
// EXAMPLE: binarySearchRecursive([2, 5, 8, 12, 16, 23, 38], 23) -> index 5
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(log n) recursion call stack
// VIVA:    Base case 'low > high' detects when target is not present and returns -1.
template <typename T>
int binarySearchRecursive(const std::vector<T>& arr, const T& target, Metrics& metrics);

// WHAT:    Specialized search for a Document by unique ID in a catalog sorted by ID ascending.
// IDEA:    Extracts IDs from mid document and branches iterative or recursive search based on useRecursive flag.
// EXAMPLE: binarySearchDocumentById(catalog, 250, false, metrics) -> index 249
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1) iterative, O(log n) recursive
// VIVA:    Changing 'useRecursive' lets the examiner switch between iterative and recursive execution.
int binarySearchDocumentById(const std::vector<Document>& docs, int targetId, bool useRecursive, Metrics& metrics);

} // namespace dls

#include "dls/search.inl"
