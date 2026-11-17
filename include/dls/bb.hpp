#pragma once

#include "dls/types.hpp"
#include "dls/generator.hpp"
#include <vector>
#include <cstdint>

namespace dls {

// ============================================================================
// 1. N-QUEENS (BACKTRACKING)
// ============================================================================

struct NQueensResult {
    int totalSolutions{0};
    // Each solution is a vector of column positions for rows 0..N-1
    std::vector<std::vector<int>> solutions;
};

/**
 * @brief N-Queens Problem via Backtracking.
 * 
 * IDEA:
 * Places queens row-by-row, verifying no column or diagonal conflicts.
 * Backtracks when no column in the current row is safe.
 * 
 * TIME COMPLEXITY:
 * O(N!).
 * 
 * SPACE COMPLEXITY:
 * O(N) auxiliary space.
 * 
 * WHEN IT FAILS:
 * No solutions exist for N = 2 and N = 3.
 */
NQueensResult solveNQueens(int n, Metrics& metrics);

// ============================================================================
// 2. SUBSET SUM (BACKTRACKING)
// ============================================================================

struct SubsetSumResult {
    bool found{false};
    std::vector<int> subset;
};

/**
 * @brief Subset Sum Problem via Backtracking with Bounding.
 * 
 * IDEA:
 * Explores inclusion/exclusion of elements with two bounding functions:
 * Prunes if currentSum + S[i] > target or if currentSum + remainingSum < target.
 * 
 * TIME COMPLEXITY:
 * O(2^n) worst case, pruned heavily.
 * 
 * SPACE COMPLEXITY:
 * O(n) recursion stack.
 * 
 * WHEN IT FAILS:
 * Returns found = false if no combination equals target sum.
 */
SubsetSumResult solveSubsetSum(std::vector<int> numbers, int targetSum, Metrics& metrics);

// ============================================================================
// 3. 0/1 KNAPSACK BRANCH-AND-BOUND (FIFO, LIFO, LEAST-COST)
// ============================================================================

struct BBKnapsackResult {
    int maxValue{0};
    std::vector<int> selectedItemIds;
    uint64_t nodesExplored{0};
};

enum class BBStrategy {
    FIFO,       // Breadth-First search using queue
    LIFO,       // Depth-First search using stack
    LeastCost   // Best-First search using priority queue / max bound
};

/**
 * @brief 0/1 Knapsack Branch-and-Bound.
 * 
 * IDEA:
 * Prunes subtrees using fractional knapsack relaxation as an upper bound.
 * Explores state space via FIFO queue, LIFO stack, or LCBB priority queue.
 * 
 * TIME COMPLEXITY:
 * O(2^n) worst case; dramatically fewer nodes explored in practice.
 * 
 * SPACE COMPLEXITY:
 * O(2^n) queue/stack state nodes in worst case.
 * 
 * WHEN IT FAILS:
 * Degrades toward exponential complexity if all item ratios are identical and capacity is large.
 */
BBKnapsackResult knapsackBranchAndBound(const std::vector<Item>& items, int capacity, BBStrategy strategy, Metrics& metrics);

} // namespace dls
