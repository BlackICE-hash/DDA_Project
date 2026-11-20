#pragma once

#include "dls/types.hpp"
#include "dls/generator.hpp"
#include <vector>
#include <string>
#include <cstdint>

// NOTE: Structs like "KnapsackResult", "MatrixChainResult", and "TSPResult" are simple C++ bundles
// that return optimal values along with reconstructed solution paths (e.g. selected items or tour sequence).

namespace dls {

// ============================================================================
// 1. 0/1 KNAPSACK
// ============================================================================

struct KnapsackResult {
    int maxValue{0};
    std::vector<int> selectedItemIds;
};

// WHAT:    Solves the 0/1 Knapsack problem for book acquisition using Dynamic Programming.
// IDEA:    Builds a 2D table dp[i][w] storing maximum utility using first i items within weight w.
//          Either includes item i (dp[i-1][w - w_i] + v_i) or excludes it (dp[i-1][w]).
// EXAMPLE: Items [(w:2, v:3), (w:3, v:4)], Capacity 5 -> Value 7 with both items
// TIME:    best O(nW) / avg O(nW) / worst O(nW) pseudo-polynomial      SPACE: O(nW) table
// VIVA:    O(nW) is pseudo-polynomial because it is exponential in input bit length k = log2(W).
KnapsackResult knapsack01(const std::vector<Item>& items, int capacity, Metrics& metrics);

// ============================================================================
// 2. MATRIX CHAIN MULTIPLICATION
// ============================================================================

struct MatrixChainResult {
    int64_t minCost{0};
    std::string parenthesization;
};

// WHAT:    Determines the optimal parenthesization order for multiplying a chain of catalog matrices.
// IDEA:    Uses bottom-up DP to compute minimum scalar multiplications m[i][j] = min_k { m[i][k] + m[k+1][j] + p_{i-1} p_k p_j }
//          and reconstructs parentheses using split table s[i][j].
// EXAMPLE: Matrices A1(10x100), A2(100x5), A3(5x50) -> minCost 7500 with ((A1 A2) A3)
// TIME:    best O(n^3) / avg O(n^3) / worst O(n^3)      SPACE: O(n^2) tables
// VIVA:    Matrix multiplication is associative but not commutative; optimal order drastically saves operations.
MatrixChainResult matrixChainOrder(const std::vector<int>& dimensions, Metrics& metrics);

// ============================================================================
// 3. LONGEST COMMON SUBSEQUENCE (LCS)
// ============================================================================

struct LCSResult {
    int length{0};
    std::string sequence;
};

// WHAT:    Finds the longest common subsequence of characters between two manuscript texts.
// IDEA:    If X[i] == Y[j], dp[i][j] = dp[i-1][j-1] + 1; otherwise max(dp[i-1][j], dp[i][j-1]).
//          Backtracks from dp[m][n] to reconstruct the matched text sequence.
// EXAMPLE: "ALGORITHM" and "LOGARITHM" -> LCS is "LGORITHM" with length 8
// TIME:    best O(m*n) / avg O(m*n) / worst O(m*n)      SPACE: O(m*n) DP grid
// VIVA:    Backtracking follows arrows diagonally when characters match, or toward the larger neighbor.
LCSResult longestCommonSubsequence(const std::string& X, const std::string& Y, Metrics& metrics);

// ============================================================================
// 4. OPTIMAL BINARY SEARCH TREE (OBST)
// ============================================================================

struct OBSTResult {
    double minCost{0.0};
    std::vector<std::vector<int>> root;
};

// WHAT:    Constructs an optimal binary search tree for keywords with known query probabilities.
// IDEA:    Minimizes expected search depth: e[i][j] = min_r { e[i][r-1] + e[r+1][j] + w[i][j] }.
//          Computes optimal roots for all subtrees using dynamic programming.
// EXAMPLE: Keys [k1, k2, k3] with frequencies -> builds tree minimizing total search comparisons
// TIME:    best O(n^3) / avg O(n^3) / worst O(n^3)      SPACE: O(n^2) tables
// VIVA:    Keys with highest search probability are placed near the root to minimize weighted depth.
OBSTResult optimalBST(const std::vector<double>& p, const std::vector<double>& q, int n, Metrics& metrics);

// ============================================================================
// 5. BELLMAN-FORD
// ============================================================================

struct BellmanFordResult {
    std::vector<int64_t> distances;
    std::vector<int> predecessors;
    bool hasNegativeCycle{false};
};

// WHAT:    Computes shortest delivery paths with arbitrary edge weights and detects negative cycles.
// IDEA:    Relaxes all edges V-1 times. If any edge can still be relaxed on a V-th pass,
//          a reachable negative-weight cycle exists.
// EXAMPLE: Source 0 with 4 vertices -> distances [0, 5, 8, 2], hasNegativeCycle = false
// TIME:    best O(V*E) / avg O(V*E) / worst O(V*E)      SPACE: O(V) distances
// VIVA:    A simple path has at most V-1 edges; any path needing V relaxations contains a cycle.
BellmanFordResult bellmanFord(int numVertices, const std::vector<Edge>& edges, int source, Metrics& metrics);

// ============================================================================
// 6. FLOYD-WARSHALL
// ============================================================================

struct FloydWarshallResult {
    std::vector<std::vector<int64_t>> distances;
    std::vector<std::vector<int>> nextHop;
    bool hasNegativeCycle{false};
};

// WHAT:    Computes all-pairs shortest transit distances between every pair of library branches.
// IDEA:    Considers intermediate vertices k from 0 to V-1: D[i][j] = min(D[i][j], D[i][k] + D[k][j]).
//          If any diagonal entry D[i][i] < 0, a negative cycle exists.
// EXAMPLE: 4 library branches -> 4x4 table of shortest inter-branch travel distances
// TIME:    best O(V^3) / avg O(V^3) / worst O(V^3)      SPACE: O(V^2) distance matrix
// VIVA:    The triple loop order MUST place intermediate vertex k on the outside.
FloydWarshallResult floydWarshall(int numVertices, const std::vector<Edge>& edges, Metrics& metrics);

// ============================================================================
// 7. TRAVELING SALESPERSON PROBLEM (TSP) VIA BITMASK DP
// ============================================================================

struct TSPResult {
    int64_t minCost{0};
    std::vector<int> tour;
};

// WHAT:    Computes the shortest walking tour for retrieving books across floor stacks (TSP).
// IDEA:    Held-Karp bitmask DP: dp[mask][u] = min cost to visit subset `mask` ending at vertex u.
//          Uses integer bit manipulation to represent visited vertex subsets.
// EXAMPLE: 5 floor stacks -> tour visiting 0 -> 4 -> 3 -> 2 -> 1 -> 0 of length 29 meters
// TIME:    best O(n^2 * 2^n) / avg O(n^2 * 2^n) / worst O(n^2 * 2^n)      SPACE: O(n * 2^n) table
// VIVA:    Much faster than brute-force O(n!): for n=15, 14! ≈ 8.7e10 operations vs 7.37e6 for DP.
TSPResult tspBitmask(int n, const std::vector<std::vector<int64_t>>& distMatrix, Metrics& metrics);

} // namespace dls
