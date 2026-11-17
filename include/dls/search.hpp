#pragma once

#include "dls/types.hpp"
#include <vector>

namespace dls {

/**
 * @brief Iterative Binary Search.
 * 
 * IDEA:
 * Maintain search boundaries [low, high]. In each iteration, inspect the middle element.
 * If target matches, return the index. If target is less than mid, narrow to left half;
 * otherwise narrow to right half.
 * 
 * TIME COMPLEXITY:
 * Best: O(1), Average: O(log n), Worst: O(log n).
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space.
 * 
 * WHEN IT FAILS:
 * Fails (returns incorrect result or -1) if the input array is not sorted.
 */
template <typename T>
int binarySearchIterative(const std::vector<T>& arr, const T& target, Metrics& metrics);

/**
 * @brief Recursive Binary Search.
 * 
 * IDEA:
 * Divide search space in half at each recursive call:
 * T(n) = T(n/2) + O(1).
 * 
 * TIME COMPLEXITY:
 * Best: O(1), Average: O(log n), Worst: O(log n).
 * 
 * SPACE COMPLEXITY:
 * O(log n) call stack frames.
 * 
 * WHEN IT FAILS:
 * Fails if array is unsorted; can trigger stack overflow on extreme recursion if corrupted.
 */
template <typename T>
int binarySearchRecursive(const std::vector<T>& arr, const T& target, Metrics& metrics);

/**
 * Specialized search for Document by ID in a vector sorted by ID ascending.
 */
int binarySearchDocumentById(const std::vector<Document>& docs, int targetId, bool useRecursive, Metrics& metrics);

} // namespace dls

#include "dls/search.inl"
