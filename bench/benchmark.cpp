// NOTE: This file is the standalone benchmark suite executable (dls_bench).
// It runs automated comparative benchmarks across sorting, searching, string matching,
// minimum spanning trees, and knapsack algorithms, printing timings and comparison counts,
// and exporting results to data/benchmark_results.csv.

#include "dls/types.hpp"
#include "dls/generator.hpp"
#include "dls/sort.hpp"
#include "dls/search.hpp"
#include "dls/dp.hpp"
#include "dls/bb.hpp"
#include "dls/greedy.hpp"
#include "dls/strmatch.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

namespace dls {

// Structure holding one row of benchmark performance data
struct BenchmarkRow {
    std::string category;
    std::string algorithm;
    size_t inputSize;
    double timeMs;
    uint64_t comparisons;
    std::string auxSpaceTheory;
};

// WHAT:    Executes automated benchmark suite across 5 algorithmic categories and exports CSV
// IDEA:    Generates controlled deterministic datasets (seed 42), runs competing algorithm pairs
//          with Metrics tracking, prints formatted console tables, and writes rows to CSV.
// EXAMPLE: Runs MergeSort vs QuickSort on N=1k, 10k, 100k -> records elapsed ms and comparisons
// TIME:    O(N log N) sorting + O(N) search + O(N) string match + O(nW) DP
// SPACE:   O(N) for generated document vectors
// VIVA:    Examiner asks: "Why do we run benchmarks with a fixed seed (42)?"
//          Answer: A fixed seed ensures reproducibility so algorithm comparisons run on identical inputs.
void runFullBenchmarks() {
    std::cout << "======================================================================\n";
    std::cout << "            DIGITAL LIBRARY ENGINE: AUTOMATED BENCHMARKS              \n";
    std::cout << "======================================================================\n";

    std::vector<BenchmarkRow> rows;

    // 1. Sorting Benchmarks (1,000, 10,000, 100,000)
    std::vector<size_t> sortSizes = {1000, 10000, 100000};
    for (size_t n : sortSizes) {
        auto docs1 = DataGenerator::generateDocuments(n, 42);
        auto docs2 = docs1;

        Metrics mMerge, mQuick;
        mergeSort(docs1, mMerge, DocumentScoreDesc());
        quickSort(docs2, mQuick, DocumentScoreDesc());

        rows.push_back({"Sort", "MergeSort", n, mMerge.ms, mMerge.comparisons, "O(n)"});
        rows.push_back({"Sort", "QuickSort", n, mQuick.ms, mQuick.comparisons, "O(log n) stack"});

        std::cout << "[Sort N=" << std::setw(6) << n << "] MergeSort: " << std::setw(8) << mMerge.ms 
                  << " ms (" << mMerge.comparisons << " comps) | QuickSort: " 
                  << std::setw(8) << mQuick.ms << " ms (" << mQuick.comparisons << " comps)\n";
    }

    // 2. Binary Search Benchmarks (100,000 items)
    {
        size_t n = 100000;
        auto docs = DataGenerator::generateDocuments(n, 42);
        Metrics mSort;
        mergeSort(docs, mSort, DocumentIdAsc());

        Metrics mIter, mRec;
        int target = 88888;
        binarySearchDocumentById(docs, target, false, mIter);
        binarySearchDocumentById(docs, target, true, mRec);

        rows.push_back({"Search", "BinarySearchIterative", n, mIter.ms, mIter.comparisons, "O(1)"});
        rows.push_back({"Search", "BinarySearchRecursive", n, mRec.ms, mRec.comparisons, "O(log n) stack"});

        std::cout << "\n[Search N=100000] Iterative: " << mIter.ms << " ms (" << mIter.comparisons 
                  << " comps) | Recursive: " << mRec.ms << " ms (" << mRec.comparisons << " comps)\n";
    }

    // 3. String Matching Benchmarks (Adversarial, 100,000 chars)
    {
        size_t n = 100000;
        std::string pattern;
        std::string text = DataGenerator::generateAdversarialString(n, 8, pattern);

        Metrics mNaive, mKMP, mRK;
        matchKMP(text, pattern, mKMP);
        matchRabinKarp(text, pattern, mRK);
        matchNaive(text, pattern, mNaive);

        rows.push_back({"StringMatch", "Naive", n, mNaive.ms, mNaive.comparisons, "O(1)"});
        rows.push_back({"StringMatch", "RabinKarp", n, mRK.ms, mRK.comparisons, "O(1)"});
        rows.push_back({"StringMatch", "KMP", n, mKMP.ms, mKMP.comparisons, "O(m)"});

        std::cout << "\n[StringMatch N=100000] Naive: " << mNaive.ms << " ms (" << mNaive.comparisons 
                  << " comps) | KMP: " << mKMP.ms << " ms (" << mKMP.comparisons 
                  << " comps) | RK: " << mRK.ms << " ms (" << mRK.comparisons << " comps)\n";
    }

    // 4. MST Benchmarks (Prim vs Kruskal)
    {
        int v = 50;
        int e = 300;
        auto graph = DataGenerator::generateGraph(v, e, 1, 100, 42);

        Metrics mPrim, mKruskal;
        auto pRes = primMST(v, graph, mPrim);
        auto kRes = kruskalMST(v, graph, mKruskal);

        rows.push_back({"MST", "Prim", static_cast<size_t>(v), mPrim.ms, mPrim.comparisons, "O(V + E)"});
        rows.push_back({"MST", "Kruskal", static_cast<size_t>(v), mKruskal.ms, mKruskal.comparisons, "O(V + E)"});

        std::cout << "\n[MST V=50, E=300] Prim: " << mPrim.ms << " ms (Weight: " << pRes.totalWeight 
                  << ") | Kruskal: " << mKruskal.ms << " ms (Weight: " << kRes.totalWeight << ")\n";
    }

    // 5. Knapsack DP vs Branch-and-Bound
    {
        auto items = DataGenerator::generateItems(20, 15, 60, 42);
        int cap = 60;

        Metrics mDP, mLCBB, mFIFO;
        auto dpRes = knapsack01(items, cap, mDP);
        auto lcbbRes = knapsackBranchAndBound(items, cap, BBStrategy::LeastCost, mLCBB);
        auto fifoRes = knapsackBranchAndBound(items, cap, BBStrategy::FIFO, mFIFO);

        rows.push_back({"Knapsack", "DP", 20, mDP.ms, mDP.comparisons, "O(nW)"});
        rows.push_back({"Knapsack", "LCBB", 20, mLCBB.ms, mLCBB.comparisons, "O(2^n) queue"});
        rows.push_back({"Knapsack", "FIFO-BB", 20, mFIFO.ms, mFIFO.comparisons, "O(2^n) queue"});

        std::cout << "\n[Knapsack N=20, W=60] DP: " << mDP.ms << " ms (Val: " << dpRes.maxValue 
                  << ") | LCBB: " << mLCBB.ms << " ms (Nodes: " << lcbbRes.nodesExplored 
                  << ") | FIFO-BB: " << mFIFO.ms << " ms (Nodes: " << fifoRes.nodesExplored << ")\n";
    }

    // Export benchmark CSV
    std::ofstream out("data/benchmark_results.csv");
    if (out.is_open()) {
        out << "category,algorithm,input_size,time_ms,comparisons,aux_space_theory\n";
        for (const auto& r : rows) {
            out << r.category << "," << r.algorithm << "," << r.inputSize << ","
                << r.timeMs << "," << r.comparisons << "," << r.auxSpaceTheory << "\n";
        }
        std::cout << "\nSuccessfully exported benchmark metrics to data/benchmark_results.csv\n";
    }
}

} // namespace dls

// WHAT:    Entry point for the benchmark executable (dls_bench)
// IDEA:    Calls runFullBenchmarks() to measure and compare algorithms across categories.
// EXAMPLE: ./build/dls_bench -> prints benchmark tables and updates data/benchmark_results.csv
// TIME:    Total runtime ~0.5 - 1.5 seconds
// SPACE:   O(N) for benchmark datasets
// VIVA:    Examiner asks: "How do you run the benchmarks from the command line?"
//          Answer: Run `./build/dls_bench` from the project root.
int main() {
    dls::runFullBenchmarks();
    return 0;
}
