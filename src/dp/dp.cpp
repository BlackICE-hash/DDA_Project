#include "dls/dp.hpp"
#include <climits>
#include <algorithm> // NOTE: Only for std::reverse on reconstructed string/tour! Let's write our own reverse to be 100% scratch!

namespace dls {

namespace {

// Helper to reverse vector without std::reverse
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

// Helper to reverse string without std::reverse
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

KnapsackResult knapsack01(const std::vector<Item>& items, int capacity, Metrics& metrics) {
    if (items.empty() || capacity <= 0) {
        return {0, {}};
    }

    metrics.startTimer();
    int n = static_cast<int>(items.size());
    // dp[i][w] = max value achievable with first i items and capacity w
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

    // Step 1: Fill DP table
    for (int i = 1; i <= n; ++i) {
        int w_i = items[i - 1].weight;
        int v_i = items[i - 1].value;

        for (int w = 0; w <= capacity; ++w) {
            // Option 1: Do not include item i
            dp[i][w] = dp[i - 1][w];

            // Option 2: Include item i if it fits
            if (w_i <= w) {
                metrics.comparisons++;
                int withItem = dp[i - 1][w - w_i] + v_i;
                if (withItem > dp[i][w]) {
                    dp[i][w] = withItem;
                }
            }
        }
    }

    // Step 2: Backtrack to find chosen items
    std::vector<int> selected;
    int currW = capacity;
    for (int i = n; i >= 1; --i) {
        if (dp[i][currW] != dp[i - 1][currW]) {
            selected.push_back(items[i - 1].id);
            currW -= items[i - 1].weight;
        }
    }
    customReverse(selected);

    metrics.stopTimer();
    return {dp[n][capacity], selected};
}

// ============================================================================
// 2. MATRIX CHAIN MULTIPLICATION
// ============================================================================

namespace {

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
