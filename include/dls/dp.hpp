#pragma once

#include "dls/types.hpp"
#include "dls/generator.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace dls {

// ============================================================================
// 1. 0/1 KNAPSACK
// ============================================================================

struct KnapsackResult {
    int maxValue{0};
    std::vector<int> selectedItemIds;
};

/**
 * @brief 0/1 Knapsack via Dynamic Programming.
 * 
 * IDEA:
 * dp[i][w] = max value from first i items with weight limit w.
 * If item i fits: dp[i][w] = max(dp[i-1][w], dp[i-1][w - w_i] + v_i), else dp[i-1][w].
 * 
 * TIME COMPLEXITY:
 * O(n * W) pseudo-polynomial.
 * 
 * SPACE COMPLEXITY:
 * O(n * W) for table and item reconstruction.
 * 
 * WHEN IT FAILS:
 * When capacity W is extremely large or weights are fractional.
 */
KnapsackResult knapsack01(const std::vector<Item>& items, int capacity, Metrics& metrics);

// ============================================================================
// 2. MATRIX CHAIN MULTIPLICATION
// ============================================================================

struct MatrixChainResult {
    int64_t minCost{0};
    std::string parenthesization;
};

/**
 * @brief Matrix Chain Multiplication.
 * 
 * IDEA:
 * Computes optimal parenthesization to minimize scalar multiplications:
 * m[i][j] = min_{i<=k<j} { m[i][k] + m[k+1][j] + p_{i-1} * p_k * p_j }.
 * 
 * TIME COMPLEXITY:
 * O(n^3).
 * 
 * SPACE COMPLEXITY:
 * O(n^2).
 * 
 * WHEN IT FAILS:
 * Incompatible dimensions (vector size < 2).
 */
MatrixChainResult matrixChainOrder(const std::vector<int>& dimensions, Metrics& metrics);

// ============================================================================
// 3. LONGEST COMMON SUBSEQUENCE (LCS)
// ============================================================================

struct LCSResult {
    int length{0};
    std::string sequence;
};

/**
 * @brief Longest Common Subsequence (LCS).
 * 
 * IDEA:
 * If X[i] == Y[j]: dp[i][j] = dp[i-1][j-1] + 1
 * Else: dp[i][j] = max(dp[i-1][j], dp[i][j-1]).
 * Backtrack from dp[m][n] to construct string.
 * 
 * TIME COMPLEXITY:
 * O(m * n).
 * 
 * SPACE COMPLEXITY:
 * O(m * n).
 * 
 * WHEN IT FAILS:
 * Excessive memory when sequences exceed hundreds of thousands of characters.
 */
LCSResult longestCommonSubsequence(const std::string& X, const std::string& Y, Metrics& metrics);

// ============================================================================
// 4. OPTIMAL BINARY SEARCH TREE (OBST)
// ============================================================================

struct OBSTResult {
    double minCost{0.0};
    std::vector<std::vector<int>> root;
};

/**
 * @brief Optimal Binary Search Tree (OBST).
 * 
 * IDEA:
 * Builds BST with minimal expected search cost given key frequencies p and dummy keys q.
 * e[i][j] = min_{i<=r<=j} { e[i][r-1] + e[r+1][j] + w[i][j] }.
 * 
 * TIME COMPLEXITY:
 * O(n^3).
 * 
 * SPACE COMPLEXITY:
 * O(n^2).
 * 
 * WHEN IT FAILS:
 * Negative or unnormalized probabilities, or keys not pre-sorted.
 */
OBSTResult optimalBST(const std::vector<double>& p, const std::vector<double>& q, int n, Metrics& metrics);

// ============================================================================
// 5. BELLMAN-FORD
// ============================================================================

struct BellmanFordResult {
    std::vector<int64_t> distances;
    std::vector<int> predecessors;
    bool hasNegativeCycle{false};
};

/**
 * @brief Bellman-Ford Single-Source Shortest Path with Negative Cycle Detection.
 * 
 * IDEA:
 * Relaxes all edges V-1 times. A V-th iteration detects negative-weight cycles.
 * 
 * TIME COMPLEXITY:
 * O(V * E).
 * 
 * SPACE COMPLEXITY:
 * O(V).
 * 
 * WHEN IT FAILS:
 * Returns hasNegativeCycle = true if a negative cycle is reachable from source.
 */
BellmanFordResult bellmanFord(int numVertices, const std::vector<Edge>& edges, int source, Metrics& metrics);

// ============================================================================
// 6. FLOYD-WARSHALL
// ============================================================================

struct FloydWarshallResult {
    std::vector<std::vector<int64_t>> distances;
    std::vector<std::vector<int>> nextHop;
    bool hasNegativeCycle{false};
};

/**
 * @brief Floyd-Warshall All-Pairs Shortest Path.
 * 
 * IDEA:
 * Dynamic programming over intermediate vertices k:
 * D[i][j] = min(D[i][j], D[i][k] + D[k][j]).
 * Negative cycle exists if any D[i][i] < 0.
 * 
 * TIME COMPLEXITY:
 * O(V^3).
 * 
 * SPACE COMPLEXITY:
 * O(V^2).
 * 
 * WHEN IT FAILS:
 * Graphs with negative cycles.
 */
FloydWarshallResult floydWarshall(int numVertices, const std::vector<Edge>& edges, Metrics& metrics);

// ============================================================================
// 7. TRAVELING SALESPERSON PROBLEM (TSP) VIA BITMASK DP
// ============================================================================

struct TSPResult {
    int64_t minCost{0};
    std::vector<int> tour;
};

/**
 * @brief Held-Karp Bitmask DP for TSP (n <= 15).
 * 
 * IDEA:
 * dp[mask][u] = min cost visiting subset `mask` ending at vertex u.
 * Transitions: dp[mask | (1 << v)][v] = min(dp[mask | (1 << v)][v], dp[mask][u] + dist[u][v]).
 * 
 * TIME COMPLEXITY:
 * O(n^2 * 2^n).
 * 
 * SPACE COMPLEXITY:
 * O(n * 2^n).
 * 
 * WHEN IT FAILS:
 * Infeasible for n > 20 due to exponential memory and compute growth.
 */
TSPResult tspBitmask(int n, const std::vector<std::vector<int64_t>>& distMatrix, Metrics& metrics);

} // namespace dls
