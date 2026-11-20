#include "dls/dp.hpp"
#include <climits>
#include <algorithm> // NOTE: Only for std::reverse on reconstructed string/tour! Let's write our own reverse to be 100% scratch!

// NOTE: This file implements Dynamic Programming (DP) algorithms for digital library optimization.
// DP solves problems with overlapping subproblems by storing answers in a table to avoid redundant work.

namespace dls {

namespace {

// WHAT:    Reverses elements of a vector in-place without using <algorithm>.
// IDEA:    Uses two pointers (front i and back j), swapping elements and moving inward.
// EXAMPLE: customReverse([1, 2, 3]) -> [3, 2, 1]
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(1)
// VIVA:    Swapping from both ends in-place avoids allocating a second vector buffer.
template <typename T>
void customReverse(std::vector<T>& vec) {
    if (vec.empty()) return;
    size_t i = 0;
    size_t j = vec.size() - 1;
    while (i < j) {
        std::swap(vec[i], vec[j]);
        i++;
        j--;
    }
}

// WHAT:    Reverses characters of a string in-place without std::reverse.
// IDEA:    Swaps str[i] and str[j] while pointer i < j.
// EXAMPLE: customReverseString("ABCD") -> "DCBA"
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(1)
// VIVA:    Used when reconstructing the Longest Common Subsequence backwards from table end.
void customReverseString(std::string& str) {
    if (str.empty()) return;
    size_t i = 0;
    size_t j = str.size() - 1;
    while (i < j) {
        std::swap(str[i], str[j]);
        i++;
        j--;
    }
}

} // anonymous namespace

// ============================================================================
// 1. 0/1 KNAPSACK
// ============================================================================

// WHAT:    Solves 0/1 Knapsack problem for book acquisitions using dynamic programming.
// IDEA:    dp[i][w] = maximum utility considering first i items within weight budget w.
//          Either skips item i (dp[i-1][w]) or takes it (dp[i-1][w - w_i] + v_i), picking max.
// EXAMPLE: Items [(w:2, v:3), (w:3, v:4)], Capacity 5 -> Max value 7 (takes both items)
// TIME:    best O(nW) / avg O(nW) / worst O(nW) pseudo-polynomial      SPACE: O(nW) table
// VIVA:    Pseudo-polynomial: runs fast when W is small, but memory explodes if W is a 64-bit integer.
KnapsackResult knapsack01(const std::vector<Item>& items, int capacity, Metrics& metrics) {
    // WHY: If there are no items to acquire or zero budget, maximum possible value is 0.
    if (items.empty() || capacity <= 0) {
        return {0, {}};
    }

    metrics.startTimer(); // WHY: Start execution stopwatch
    int n = static_cast<int>(items.size()); // WHY: Number of candidate items

    // WHY: Table has (n + 1) rows for items 0..n and (capacity + 1) columns for budgets 0..W.
    // Initialized to 0 because 0 items yield 0 value, and 0 capacity holds 0 value.
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

    // WHY: Step 1: Iterate through items i from 1 to n (1-indexed row represents item i-1).
    for (int i = 1; i <= n; ++i) {
        // WHY: Extract weight and value of current item (i - 1 because vector is 0-indexed).
        int w_i = items[i - 1].weight;
        int v_i = items[i - 1].value;

        // WHY: Iterate through every possible integer capacity limit w from 0 up to capacity.
        for (int w = 0; w <= capacity; ++w) {
            // WHY: Option 1: Do not include item i; utility is identical to best value from first i-1 items.
            dp[i][w] = dp[i - 1][w];

            // WHY: Option 2: Check if item i physically fits within current weight budget w.
            if (w_i <= w) {
                metrics.comparisons++; // WHY: Count comparison against previous best value

                // WHY: Value achieved if item i is taken: its value v_i plus the optimal value
                // achieved by remaining capacity (w - w_i) using the previous (i - 1) items.
                int withItem = dp[i - 1][w - w_i] + v_i;

                // WHY: Take item i if and only if it produces strictly higher total utility.
                if (withItem > dp[i][w]) {
                    dp[i][w] = withItem; // WHY: Store optimal value in DP state
                }
            }
        }
    }

    // WHY: Step 2: Backtrack from dp[n][capacity] to recover which specific items were selected.
    std::vector<int> selected;
    int currW = capacity; // WHY: Start with full budget capacity

    // WHY: Traverse rows backwards from item n down to item 1.
    for (int i = n; i >= 1; --i) {
        // WHY: If value differs from the row above, item i MUST have been included in the optimal set.
        if (dp[i][currW] != dp[i - 1][currW]) {
            selected.push_back(items[i - 1].id); // WHY: Record item ID in selection list
            currW -= items[i - 1].weight;        // WHY: Deduct item's weight from remaining budget
        }
        // WHY: If values are equal, item i was excluded; continue to row i-1 with same currW.
    }

    // WHY: Backtracking found items in reverse order; reverse vector so IDs appear in forward order.
    customReverse(selected);

    metrics.stopTimer(); // WHY: Stop stopwatch before returning
    // WHY: dp[n][capacity] contains the globally optimal total utility value.
    return {dp[n][capacity], selected};
}

// ============================================================================
// 2. MATRIX CHAIN MULTIPLICATION
// ============================================================================

namespace {

// WHAT:    Recursive helper that constructs the optimal parenthesization string.
// IDEA:    If i == j, prints matrix "Ai". Otherwise prints "(" + left + " x " + right + ")"
//          split at s[i][j].
// EXAMPLE: s[1][3] = 1 -> "((A1 x A2) x A3)"
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n) recursion stack
// VIVA:    Split point s[i][j] records where the optimal parenthesization cut was placed.
void buildParenthesization(int i, int j, const std::vector<std::vector<int>>& s, std::string& out) {
    if (i == j) {
        out += "A" + std::to_string(i);
        return;
    }
    out += "(";
    buildParenthesization(i, s[i][j], s, out);
    out += " x ";
    buildParenthesization(s[i][j] + 1, j, s, out);
    out += ")";
}

} // anonymous namespace

// WHAT:    Finds optimal parenthesization of matrix chain to minimize scalar multiplications.
// IDEA:    Computes m[i][j] for chain lengths L from 2 to n. Tries all split points k:
//          m[i][j] = min_k { m[i][k] + m[k+1][j] + p_{i-1} * p_k * p_j }.
// EXAMPLE: Matrices [10, 100, 5, 50] -> minimum cost 7500 with ((A1 x A2) x A3)
// TIME:    best O(n^3) / avg O(n^3) / worst O(n^3)      SPACE: O(n^2) tables
// VIVA:    Outer loop must iterate by chain length L (subproblem size) to ensure sub-chains are solved first.
MatrixChainResult matrixChainOrder(const std::vector<int>& p, Metrics& metrics) {
    if (p.size() < 2) {
        return {0, ""};
    }

    metrics.startTimer();
    int n = static_cast<int>(p.size()) - 1; // Number of matrices

    // m[i][j] = minimum scalar multiplications for Ai..Aj (1-indexed)
    const int64_t INF = LLONG_MAX / 4;
    std::vector<std::vector<int64_t>> m(n + 1, std::vector<int64_t>(n + 1, 0));
    std::vector<std::vector<int>> s(n + 1, std::vector<int>(n + 1, 0));

    // Chain length L from 2 to n
    for (int L = 2; L <= n; ++L) {
        for (int i = 1; i <= n - L + 1; ++i) {
            int j = i + L - 1;
            m[i][j] = INF;

            // Try every possible split point k between i and j-1
            for (int k = i; k <= j - 1; ++k) {
                int64_t cost = m[i][k] + m[k + 1][j] + (int64_t)p[i - 1] * p[k] * p[j];
                metrics.comparisons++;
                if (cost < m[i][j]) {
                    m[i][j] = cost;
                    s[i][j] = k;
                }
            }
        }
    }

    std::string expr;
    buildParenthesization(1, n, s, expr);

    metrics.stopTimer();
    return {m[1][n], expr};
}

// ============================================================================
// 3. LONGEST COMMON SUBSEQUENCE (LCS)
// ============================================================================

// WHAT:    Computes the Longest Common Subsequence between two manuscript strings.
// IDEA:    If X[i-1] == Y[j-1], dp[i][j] = dp[i-1][j-1] + 1; otherwise max of neighbors.
//          Backtracks from bottom-right of grid to reconstruct the matched subsequence.
// EXAMPLE: "ALGORITHM" and "LOGARITHM" -> LCS is "LGORITHM" with length 8
// TIME:    best O(m*n) / avg O(m*n) / worst O(m*n)      SPACE: O(m*n) table
// VIVA:    Backtracking moves diagonally on character match, or toward the larger adjacent entry.
LCSResult longestCommonSubsequence(const std::string& X, const std::string& Y, Metrics& metrics) {
    metrics.startTimer();
    int m = static_cast<int>(X.size());
    int n = static_cast<int>(Y.size());

    // dp[i][j] = LCS length of X[0..i-1] and Y[0..j-1]
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // Step 1: Fill table
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            metrics.comparisons++;
            if (X[i - 1] == Y[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                metrics.comparisons++;
                dp[i][j] = (dp[i - 1][j] >= dp[i][j - 1]) ? dp[i - 1][j] : dp[i][j - 1];
            }
        }
    }

    // Step 2: Reconstruct LCS string
    std::string lcsStr;
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (X[i - 1] == Y[j - 1]) {
            lcsStr.push_back(X[i - 1]);
            i--;
            j--;
        } else if (dp[i - 1][j] >= dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }
    customReverseString(lcsStr);

    metrics.stopTimer();
    return {dp[m][n], lcsStr};
}

// ============================================================================
// 4. OPTIMAL BINARY SEARCH TREE (OBST)
// ============================================================================

// WHAT:    Constructs an optimal binary search tree for non-uniform query frequencies.
// IDEA:    Computes e[i][j] = min_r { e[i][r-1] + e[r+1][j] + w[i][j] } over subtree lengths L from 1 to n.
//          root[i][j] records which key was chosen as root to minimize expected access cost.
// EXAMPLE: Keys with frequencies -> builds tree minimizing expected comparison cost
// TIME:    best O(n^3) / avg O(n^3) / worst O(n^3)      SPACE: O(n^2) tables
// VIVA:    w[i][j] represents total probability mass of keys and dummy gaps in the subproblem.
OBSTResult optimalBST(const std::vector<double>& p, const std::vector<double>& q, int n, Metrics& metrics) {
    metrics.startTimer();

    // e[i][j] = expected search cost for keys ki..kj (1-indexed)
    // w[i][j] = sum of probabilities
    // root[i][j] = optimal root index
    std::vector<std::vector<double>> e(n + 2, std::vector<double>(n + 1, 0.0));
    std::vector<std::vector<double>> w(n + 2, std::vector<double>(n + 1, 0.0));
    std::vector<std::vector<int>> root(n + 1, std::vector<int>(n + 1, 0));

    // Base cases for single dummy subtrees
    for (int i = 1; i <= n + 1; ++i) {
        e[i][i - 1] = q[i - 1];
        w[i][i - 1] = q[i - 1];
    }

    // Subtree length L from 1 to n
    for (int L = 1; L <= n; ++L) {
        for (int i = 1; i <= n - L + 1; ++i) {
            int j = i + L - 1;
            e[i][j] = 1e18; // infinity
            w[i][j] = w[i][j - 1] + p[j - 1] + q[j];

            // Try each key r as the root
            for (int r = i; r <= j; ++r) {
                double t = e[i][r - 1] + e[r + 1][j] + w[i][j];
                metrics.comparisons++;
                if (t < e[i][j]) {
                    e[i][j] = t;
                    root[i][j] = r;
                }
            }
        }
    }

    metrics.stopTimer();
    return {e[1][n], root};
}

// ============================================================================
// 5. BELLMAN-FORD
// ============================================================================

// WHAT:    Finds single-source shortest paths on graphs with arbitrary weights and detects negative cycles.
// IDEA:    Relaxes all edges V-1 times. A V-th pass tests whether any edge can still be relaxed,
//          which proves the existence of a reachable negative-weight cycle.
// EXAMPLE: Source 0 with 4 vertices -> distances [0, 5, 8, 2], hasNegativeCycle = false
// TIME:    best O(V*E) / avg O(V*E) / worst O(V*E)      SPACE: O(V) distances
// VIVA:    Early exit stops if an entire pass relaxes no edges, speeding up average runtime.
BellmanFordResult bellmanFord(int numVertices, const std::vector<Edge>& edges, int source, Metrics& metrics) {
    metrics.startTimer();
    const int64_t INF = LLONG_MAX / 4;
    std::vector<int64_t> dist(numVertices, INF);
    std::vector<int> pred(numVertices, -1);
    dist[source] = 0;

    // Step 1: Relax all edges |V| - 1 times
    for (int i = 1; i <= numVertices - 1; ++i) {
        bool anyUpdate = false;
        for (const auto& e : edges) {
            if (dist[e.u] != INF) {
                metrics.comparisons++;
                if (dist[e.u] + e.weight < dist[e.v]) {
                    dist[e.v] = dist[e.u] + e.weight;
                    pred[e.v] = e.u;
                    anyUpdate = true;
                }
            }
        }
        // Early stop if no edge was relaxed
        if (!anyUpdate) break;
    }

    // Step 2: Check for negative-weight cycles
    bool hasCycle = false;
    for (const auto& e : edges) {
        if (dist[e.u] != INF) {
            metrics.comparisons++;
            if (dist[e.u] + e.weight < dist[e.v]) {
                hasCycle = true;
                break;
            }
        }
    }

    metrics.stopTimer();
    return {dist, pred, hasCycle};
}

// ============================================================================
// 6. FLOYD-WARSHALL
// ============================================================================

// WHAT:    Computes all-pairs shortest transit paths between every pair of vertices.
// IDEA:    Triple loop with intermediate vertex k on the outside: D[i][j] = min(D[i][j], D[i][k] + D[k][j]).
//          If any diagonal element D[i][i] < 0, a negative cycle exists.
// EXAMPLE: 4 vertices -> 4x4 table of shortest path distances
// TIME:    best O(V^3) / avg O(V^3) / worst O(V^3)      SPACE: O(V^2) distance table
// VIVA:    The nextHop table allows reconstructing the actual shortest path between any two vertices.
FloydWarshallResult floydWarshall(int numVertices, const std::vector<Edge>& edges, Metrics& metrics) {
    metrics.startTimer();
    const int64_t INF = LLONG_MAX / 4;

    std::vector<std::vector<int64_t>> dist(numVertices, std::vector<int64_t>(numVertices, INF));
    std::vector<std::vector<int>> nextHop(numVertices, std::vector<int>(numVertices, -1));

    // Diagonal distance is 0
    for (int i = 0; i < numVertices; ++i) {
        dist[i][i] = 0;
        nextHop[i][i] = i;
    }

    // Direct edges
    for (const auto& e : edges) {
        if (e.weight < dist[e.u][e.v]) {
            dist[e.u][e.v] = e.weight;
            nextHop[e.u][e.v] = e.v;
        }
    }

    // Dynamic programming over intermediate vertex k
    for (int k = 0; k < numVertices; ++k) {
        for (int i = 0; i < numVertices; ++i) {
            for (int j = 0; j < numVertices; ++j) {
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    metrics.comparisons++;
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        nextHop[i][j] = nextHop[i][k];
                    }
                }
            }
        }
    }

    // Check for negative cycle (negative diagonal)
    bool hasNegCycle = false;
    for (int i = 0; i < numVertices; ++i) {
        if (dist[i][i] < 0) {
            hasNegCycle = true;
            break;
        }
    }

    metrics.stopTimer();
    return {dist, nextHop, hasNegCycle};
}

// ============================================================================
// 7. TRAVELING SALESPERSON PROBLEM (TSP) VIA BITMASK DP
// ============================================================================

// WHAT:    Solves the Traveling Salesperson Problem for book-stacks inventory collection.
// IDEA:    dp[mask][u] = min cost to visit subset `mask` ending at vertex u.
//          Extends mask by visiting unvisited neighbor v: nextMask = mask | (1 << v).
// EXAMPLE: 5 floor stacks -> walking tour 0 -> 4 -> 3 -> 2 -> 1 -> 0 of distance 29 meters
// TIME:    best O(n^2 * 2^n) / avg O(n^2 * 2^n) / worst O(n^2 * 2^n)      SPACE: O(n * 2^n) table
// VIVA:    The i-th bit of `mask` is 1 if vertex i was visited, 0 otherwise.
TSPResult tspBitmask(int n, const std::vector<std::vector<int64_t>>& distMatrix, Metrics& metrics) {
    if (n <= 1) {
        return {0, {0}};
    }
    if (n > 15) {
        return {-1, {}}; // Guaranteed n <= 15
    }

    metrics.startTimer();
    const int64_t INF = LLONG_MAX / 4;
    int numStates = 1 << n;

    // dp[mask][u] = min cost to visit subset `mask`, currently at vertex `u`
    std::vector<std::vector<int64_t>> dp(numStates, std::vector<int64_t>(n, INF));
    // parent[mask][u] = vertex prior to u in optimal path
    std::vector<std::vector<int>> parent(numStates, std::vector<int>(n, -1));

    // Base case: at start vertex 0 with mask (1 << 0)
    dp[1][0] = 0;

    // Iterate over all subsets
    for (int mask = 1; mask < numStates; ++mask) {
        for (int u = 0; u < n; ++u) {
            if (!(mask & (1 << u))) continue;
            if (dp[mask][u] == INF) continue;

            // Try to visit unvisited neighbor v
            for (int v = 0; v < n; ++v) {
                if (mask & (1 << v)) continue; // Already visited

                int nextMask = mask | (1 << v);
                int64_t cost = dp[mask][u] + distMatrix[u][v];

                metrics.comparisons++;
                if (cost < dp[nextMask][v]) {
                    dp[nextMask][v] = cost;
                    parent[nextMask][v] = u;
                }
            }
        }
    }

    // Find best transition to return to vertex 0
    int allVisited = (1 << n) - 1;
    int64_t minTotalCost = INF;
    int lastVertex = -1;

    for (int u = 1; u < n; ++u) {
        if (dp[allVisited][u] != INF) {
            int64_t totalCost = dp[allVisited][u] + distMatrix[u][0];
            metrics.comparisons++;
            if (totalCost < minTotalCost) {
                minTotalCost = totalCost;
                lastVertex = u;
            }
        }
    }

    // Reconstruct tour path
    std::vector<int> tour;
    if (lastVertex != -1) {
        int currMask = allVisited;
        int currV = lastVertex;
        while (currV != -1) {
            tour.push_back(currV);
            int prevV = parent[currMask][currV];
            currMask ^= (1 << currV);
            currV = prevV;
        }
        customReverse(tour);
        tour.push_back(0); // Complete round trip back to stack 0
    }

    metrics.stopTimer();
    return {minTotalCost, tour};
}

} // namespace dls
