#include "dls/bb.hpp"
#include "dls/flow.hpp"
#include "dls/num.hpp"
#include "dls/strmatch.hpp"
#include "dls/dp.hpp"
#include "dls/generator.hpp"
#include <iostream>
#include <cassert>

void testNQueens() {
    std::cout << "[RUNNING] testNQueens...\n";
    dls::Metrics m;

    assert(dls::solveNQueens(1, m).totalSolutions == 1);
    assert(dls::solveNQueens(2, m).totalSolutions == 0);
    assert(dls::solveNQueens(3, m).totalSolutions == 0);
    assert(dls::solveNQueens(4, m).totalSolutions == 2);
    // Classic 8-queens problem has exactly 92 distinct solutions
    auto res8 = dls::solveNQueens(8, m);
    assert(res8.totalSolutions == 92);
    assert(res8.solutions.size() == 92);

    std::cout << "[PASS] testNQueens passed (N=8 solutions: 92)\n";
}

void testSubsetSum() {
    std::cout << "[RUNNING] testSubsetSum...\n";
    dls::Metrics m;

    std::vector<int> nums = {3, 34, 4, 12, 5, 2};
    auto res9 = dls::solveSubsetSum(nums, 9, m);
    assert(res9.found);
    int sum = 0;
    for (int x : res9.subset) sum += x;
    assert(sum == 9);

    auto resImpossible = dls::solveSubsetSum(nums, 100, m);
    assert(!resImpossible.found);

    std::cout << "[PASS] testSubsetSum passed\n";
}

void testKnapsackBranchAndBound() {
    std::cout << "[RUNNING] testKnapsackBranchAndBound (FIFO vs LIFO vs LCBB)...\n";
    dls::Metrics mFIFO, mLIFO, mLCBB, mDP;

    std::vector<dls::Item> items = {
        {1, 2, 40}, {2, 3, 50}, {3, 1, 100}, {4, 5, 95}, {5, 3, 30}
    };
    int capacity = 7;

    auto dpRes = dls::knapsack01(items, capacity, mDP);
    auto fifoRes = dls::knapsackBranchAndBound(items, capacity, dls::BBStrategy::FIFO, mFIFO);
    auto lifoRes = dls::knapsackBranchAndBound(items, capacity, dls::BBStrategy::LIFO, mLIFO);
    auto lcbbRes = dls::knapsackBranchAndBound(items, capacity, dls::BBStrategy::LeastCost, mLCBB);

    assert(fifoRes.maxValue == dpRes.maxValue);
    assert(lifoRes.maxValue == dpRes.maxValue);
    assert(lcbbRes.maxValue == dpRes.maxValue);

    std::cout << "  Optimal Value: " << lcbbRes.maxValue << "\n";
    std::cout << "  Nodes Explored -> FIFO: " << fifoRes.nodesExplored
              << ", LIFO: " << lifoRes.nodesExplored
              << ", LeastCost: " << lcbbRes.nodesExplored << "\n";

    std::cout << "[PASS] testKnapsackBranchAndBound passed (all B&B matched DP)\n";
}

void testMaxFlowAndMatching() {
    std::cout << "[RUNNING] testMaxFlowAndMatching...\n";
    dls::Metrics mFlow;

    // Classic 6-vertex network (CLRS Figure 26.1):
    // 0: s, 5: t
    int n = 6;
    std::vector<std::vector<int64_t>> cap(n, std::vector<int64_t>(n, 0));
    cap[0][1] = 16; cap[0][2] = 13;
    cap[1][2] = 10; cap[1][3] = 12;
    cap[2][1] = 4;  cap[2][4] = 14;
    cap[3][2] = 9;  cap[3][5] = 20;
    cap[4][3] = 7;  cap[4][5] = 4;

    auto flowRes = dls::edmondsKarpMaxFlow(n, cap, 0, 5, mFlow);
    assert(flowRes.maxFlow == 23);

    // Bipartite Matching: 4 readers, 4 book copies
    // Reader 0 likes Copy 1, 2
    // Reader 1 likes Copy 0, 1
    // Reader 2 likes Copy 2
    // Reader 3 likes Copy 2, 3
    std::vector<std::pair<int, int>> bipEdges = {
        {0, 1}, {0, 2},
        {1, 0}, {1, 1},
        {2, 2},
        {3, 2}, {3, 3}
    };
    dls::Metrics mMatch;
    auto matchRes = dls::maxBipartiteMatching(4, 4, bipEdges, mMatch);
    assert(matchRes.maxMatching == 4);
    assert(matchRes.pairs.size() == 4);

    std::cout << "[PASS] testMaxFlowAndMatching passed (MaxFlow=23, BipartiteMatching=4)\n";
}

void testCRT() {
    std::cout << "[RUNNING] testCRT...\n";
    dls::Metrics m;

    // Extended GCD: 30 * x + 20 * y = 10
    auto egcd = dls::extendedGCD(30, 20, m);
    assert(egcd.gcd == 10);
    assert(30 * egcd.x + 20 * egcd.y == 10);

    // Mod inverse: 3 * x = 1 mod 11 => x = 4 (3 * 4 = 12 = 1 mod 11)
    assert(dls::modInverse(3, 11, m) == 4);

    // CRT:
    // x = 2 mod 3
    // x = 3 mod 5
    // x = 2 mod 7
    // Expected solution: 23 mod 105
    std::vector<int64_t> rems = {2, 3, 2};
    std::vector<int64_t> mods = {3, 5, 7};
    auto crtRes = dls::solveCRT(rems, mods, m);
    assert(crtRes.hasSolution);
    assert(crtRes.solution == 23);
    assert(crtRes.moduloProduct == 105);

    // Non-coprime moduli check: 4 and 6 have gcd = 2
    std::vector<int64_t> badMods = {4, 6};
    std::vector<int64_t> badRems = {1, 3};
    auto badRes = dls::solveCRT(badRems, badMods, m);
    assert(!badRes.hasSolution);

    std::cout << "[PASS] testCRT passed (solution=23 mod 105)\n";
}

void testStringMatching() {
    std::cout << "[RUNNING] testStringMatching (Naive, Rabin-Karp, FA, KMP)...\n";
    dls::Metrics mNaive, mRK, mFA, mKMP;

    // Standard string with multiple overlapping occurrences:
    // "AABAACAADAABAABA" searching for "AABA"
    // Matches at indices: 0, 9, 12
    std::string text = "AABAACAADAABAABA";
    std::string pattern = "AABA";
    std::vector<size_t> expected = {0, 9, 12};

    auto resNaive = dls::matchNaive(text, pattern, mNaive);
    auto resRK = dls::matchRabinKarp(text, pattern, mRK);
    auto resFA = dls::matchFiniteAutomaton(text, pattern, mFA);
    auto resKMP = dls::matchKMP(text, pattern, mKMP);

    assert(resNaive == expected);
    assert(resRK == expected);
    assert(resFA == expected);
    assert(resKMP == expected);

    // Adversarial input test: "aaaa...ab" searching for "aaab"
    std::string advPattern;
    std::string advText = dls::DataGenerator::generateAdversarialString(500, 10, advPattern);

    mNaive.reset();
    mKMP.reset();
    auto advNaive = dls::matchNaive(advText, advPattern, mNaive);
    auto advKMP = dls::matchKMP(advText, advPattern, mKMP);

    assert(advNaive == advKMP);
    assert(advNaive.size() == 1);
    assert(advNaive[0] == 490);

    // On adversarial input, Naive makes ~4900 comparisons, while KMP does linear comparisons!
    assert(mNaive.comparisons > mKMP.comparisons);
    std::cout << "  [Adversarial String Match Check] Naive comparisons: " << mNaive.comparisons
              << ", KMP comparisons: " << mKMP.comparisons << "\n";

    std::cout << "[PASS] testStringMatching passed (all 4 algorithms matched)\n";
}

int main() {
    std::cout << "=== Running Stage 6 Backtracking, Flow, CRT & Strings Tests ===\n";
    testNQueens();
    testSubsetSum();
    testKnapsackBranchAndBound();
    testMaxFlowAndMatching();
    testCRT();
    testStringMatching();
    std::cout << "All Stage 6 Tests Passed Successfully!\n";
    return 0;
}
