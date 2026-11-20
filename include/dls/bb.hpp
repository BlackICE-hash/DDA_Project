#pragma once

#include "dls/types.hpp"
#include "dls/generator.hpp"
#include <vector>
#include <cstdint>

// NOTE: "enum class BBStrategy" is an enum list providing three search strategies:
// FIFO (breadth-first queue), LIFO (depth-first stack), and LeastCost (priority queue with greedy bounds).

namespace dls {

// ============================================================================
// 1. N-QUEENS (BACKTRACKING)
// ============================================================================

struct NQueensResult {
    int totalSolutions{0};
    // Each solution is a vector of column positions for rows 0..N-1
    std::vector<std::vector<int>> solutions;
};

// WHAT:    Places N non-attacking library security sensors on an N x N archival vault grid.
// IDEA:    Places sensors row-by-row, testing that no column or diagonal is shared.
//          Backtracks immediately upon conflict to avoid searching invalid board configurations.
// EXAMPLE: solveNQueens(4, metrics) -> finds 2 valid safe configurations of 4 queens
// TIME:    best O(1) / avg O(N!) / worst O(N!)      SPACE: O(N) recursion stack
// VIVA:    For N=2 and N=3, returns 0 solutions because no safe placement exists.
NQueensResult solveNQueens(int n, Metrics& metrics);

// ============================================================================
// 2. SUBSET SUM (BACKTRACKING)
// ============================================================================

struct SubsetSumResult {
    bool found{false};
    std::vector<int> subset;
};

// WHAT:    Finds a combination of book invoice amounts that sum exactly to a grant allocation.
// IDEA:    Sorts numbers ascending, then recursively branches on include/exclude.
//          Prunes when currentSum + S[i] > target or currentSum + remainingSum < target.
// EXAMPLE: solveSubsetSum([10, 20, 30, 40], 50, metrics) -> finds subset [10, 40] or [20, 30]
// TIME:    best O(1) / avg pruned / worst O(2^n)      SPACE: O(n) recursion stack
// VIVA:    Bounding functions prune entire subtrees as soon as the sum overshoots or cannot reach target.
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

// WHAT:    Solves 0/1 Knapsack using Branch-and-Bound with fractional relaxation upper bounds.
// IDEA:    Explores a binary state-space tree. Prunes any node whose fractional upper bound
//          is less than or equal to the best integer solution found so far.
// EXAMPLE: Under budget austerity: LeastCost explores only 21 nodes vs FIFO's 879 nodes
// TIME:    best O(n log n) / avg pruned / worst O(2^n)      SPACE: O(2^n) active state nodes
// VIVA:    LeastCost uses BBMinHeap to expand the most promising node first, pruning 40x more nodes than FIFO.
BBKnapsackResult knapsackBranchAndBound(const std::vector<Item>& items, int capacity, BBStrategy strategy, Metrics& metrics);

} // namespace dls
