#include "dls/dp.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

void testKnapsack01() {
    std::cout << "[RUNNING] testKnapsack01...\n";
    dls::Metrics m;

    // Edge case: Empty / 0 capacity
    assert(dls::knapsack01({}, 10, m).maxValue == 0);
    assert(dls::knapsack01({{1, 5, 10}}, 0, m).maxValue == 0);

    // Textbook problem:
    // Item 1: w=2, v=3
    // Item 2: w=3, v=4
    // Item 3: w=4, v=5
    // Item 4: w=5, v=6
    // Capacity = 5
    // Optimal: Items 1 and 2 (w = 2+3 = 5, v = 3+4 = 7)
    std::vector<dls::Item> items = {
        {1, 2, 3}, {2, 3, 4}, {3, 4, 5}, {4, 5, 6}
    };
    auto res = dls::knapsack01(items, 5, m);
    assert(res.maxValue == 7);
    assert(res.selectedItemIds.size() == 2);
    assert(res.selectedItemIds[0] == 1 && res.selectedItemIds[1] == 2);

    std::cout << "[PASS] testKnapsack01 passed (maxValue=" << res.maxValue << ")\n";
}

void testMatrixChain() {
    std::cout << "[RUNNING] testMatrixChain...\n";
    dls::Metrics m;

    // Matrices A1(10x100), A2(100x5), A3(5x50)
    // Dim array: 10, 100, 5, 50
    // ((A1 x A2) x A3) cost = 10*100*5 + 10*5*50 = 5000 + 2500 = 7500
    std::vector<int> dims = {10, 100, 5, 50};
    auto res = dls::matrixChainOrder(dims, m);

    assert(res.minCost == 7500);
    assert(res.parenthesization == "((A1 x A2) x A3)");

    std::cout << "[PASS] testMatrixChain passed (minCost=" << res.minCost 
              << ", expr=" << res.parenthesization << ")\n";
}

void testLCS() {
    std::cout << "[RUNNING] testLCS...\n";
    dls::Metrics m;

    // Edge case: empty
    assert(dls::longestCommonSubsequence("", "ABC", m).length == 0);

    // Textbook: "ABCBDAB" vs "BDCABA"
    // LCS length = 4 (e.g. "BCBA", "BDAB")
    auto res = dls::longestCommonSubsequence("ABCBDAB", "BDCABA", m);
    assert(res.length == 4);
    assert(res.sequence.size() == 4);

    // No common characters
    auto disjoint = dls::longestCommonSubsequence("XYZ", "ABC", m);
    assert(disjoint.length == 0 && disjoint.sequence.empty());

    std::cout << "[PASS] testLCS passed (lcs=\"" << res.sequence << "\")\n";
}

void testOBST() {
    std::cout << "[RUNNING] testOBST...\n";
    dls::Metrics m;

    // CLRS textbook example (Section 15.5):
    // 5 keys, p = [0.15, 0.10, 0.05, 0.10, 0.20]
    // 6 dummy keys, q = [0.05, 0.10, 0.05, 0.05, 0.05, 0.10]
    // Expected optimal search cost = 2.75
    std::vector<double> p = {0.15, 0.10, 0.05, 0.10, 0.20};
    std::vector<double> q = {0.05, 0.10, 0.05, 0.05, 0.05, 0.10};

    auto res = dls::optimalBST(p, q, 5, m);
    assert(std::abs(res.minCost - 2.75) < 1e-4);

    std::cout << "[PASS] testOBST passed (minCost=" << res.minCost << ")\n";
}

void testBellmanFord() {
    std::cout << "[RUNNING] testBellmanFord...\n";
    dls::Metrics m;

    // Graph with negative weight edge but NO negative cycle:
    // 0 -> 1 (w=4), 0 -> 2 (w=5)
    // 1 -> 2 (w=-2), 1 -> 3 (w=3)
    // 2 -> 3 (w=1)
    std::vector<dls::Edge> edges = {
        {0, 1, 4}, {0, 2, 5},
        {1, 2, -2}, {1, 3, 3},
        {2, 3, 1}
    };

    auto res = dls::bellmanFord(4, edges, 0, m);
    assert(!res.hasNegativeCycle);
    assert(res.distances[0] == 0);
    assert(res.distances[1] == 4);
    assert(res.distances[2] == 2); // 0 -> 1 -> 2 = 4 + (-2) = 2
    assert(res.distances[3] == 3); // 0 -> 1 -> 2 -> 3 = 2 + 1 = 3

    // Graph WITH negative cycle:
    // 1 -> 2 (w=1), 2 -> 3 (w=-5), 3 -> 1 (w=2) => cycle sum = -2
    std::vector<dls::Edge> cycleEdges = {
        {0, 1, 1},
        {1, 2, 1}, {2, 3, -5}, {3, 1, 2}
    };
    auto cycleRes = dls::bellmanFord(4, cycleEdges, 0, m);
    assert(cycleRes.hasNegativeCycle);

    std::cout << "[PASS] testBellmanFord passed (negative weights & cycle detection verified)\n";
}

void testFloydWarshall() {
    std::cout << "[RUNNING] testFloydWarshall...\n";
    dls::Metrics m;

    // 4 vertices:
    // 0 -> 1 (w=3), 0 -> 2 (w=8)
    // 1 -> 2 (w=2), 1 -> 3 (w=5)
    // 2 -> 3 (w=1)
    // 3 -> 0 (w=2)
    std::vector<dls::Edge> edges = {
        {0, 1, 3}, {0, 2, 8},
        {1, 2, 2}, {1, 3, 5},
        {2, 3, 1},
        {3, 0, 2}
    };

    auto res = dls::floydWarshall(4, edges, m);
    assert(!res.hasNegativeCycle);
    // Path 0 -> 2: direct is 8, via 1 is 3 + 2 = 5
    assert(res.distances[0][2] == 5);
    // Path 0 -> 3: 0 -> 1 -> 2 -> 3 = 3 + 2 + 1 = 6
    assert(res.distances[0][3] == 6);
    // Path 1 -> 0: 1 -> 2 -> 3 -> 0 = 2 + 1 + 2 = 5
    assert(res.distances[1][0] == 5);

    std::cout << "[PASS] testFloydWarshall passed\n";
}

void testTSPBitmask() {
    std::cout << "[RUNNING] testTSPBitmask...\n";
    dls::Metrics m;

    // 4 vertices TSP:
    // Complete graph with known distances:
    //       0   1   2   3
    //  0 [  0, 10, 15, 20 ]
    //  1 [ 10,  0, 35, 25 ]
    //  2 [ 15, 35,  0, 30 ]
    //  3 [ 20, 25, 30,  0 ]
    //
    // Possible cycles:
    // 0-1-3-2-0: 10 + 25 + 30 + 15 = 80
    // Optimal cycle: 0 -> 1 -> 3 -> 2 -> 0 has cost 80
    std::vector<std::vector<int64_t>> dist = {
        { 0, 10, 15, 20 },
        { 10,  0, 35, 25 },
        { 15, 35,  0, 30 },
        { 20, 25, 30,  0 }
    };

    auto res = dls::tspBitmask(4, dist, m);
    assert(res.minCost == 80);
    assert(res.tour.front() == 0 && res.tour.back() == 0);
    assert(res.tour.size() == 5);

    std::cout << "[PASS] testTSPBitmask passed (minCost=" << res.minCost << ")\n";
}

int main() {
    std::cout << "=== Running Stage 5 Dynamic Programming Tests ===\n";
    testKnapsack01();
    testMatrixChain();
    testLCS();
    testOBST();
    testBellmanFord();
    testFloydWarshall();
    testTSPBitmask();
    std::cout << "All Stage 5 Tests Passed Successfully!\n";
    return 0;
}
