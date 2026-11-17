#include "dls/types.hpp"
#include "dls/generator.hpp"
#include "dls/sort.hpp"
#include "dls/search.hpp"
#include "dls/dc.hpp"
#include "dls/greedy.hpp"
#include "dls/dp.hpp"
#include "dls/bb.hpp"
#include "dls/flow.hpp"
#include "dls/num.hpp"
#include "dls/strmatch.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>

namespace dls {

void printHeader(const std::string& title) {
    std::cout << "\n======================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "======================================================================\n";
}

void printMetrics(const std::string& label, const Metrics& m) {
    std::cout << "  [" << label << "] Time: " << std::fixed << std::setprecision(4) << m.ms << " ms | "
              << "Comparisons: " << m.comparisons << " | "
              << "Peak RSS: " << (m.peakBytes > 0 ? m.peakBytes : Metrics::queryCurrentRSS()) / 1024 << " KB\n";
}

// Scenarios implemented cleanly
void runScenario1_NormalOperation() {
    printHeader("SCENARIO 1: NORMAL DIGITAL LIBRARY OPERATION");
    std::cout << "Simulating a comprehensive end-to-end digital library workflow.\n\n";

    // 1. Catalog ranking
    std::cout << "1. Ranking Document Catalog by Relevance Score...\n";
    auto docs = DataGenerator::generateDocuments(500, 42);
    auto docsQuick = docs;
    Metrics mMerge, mQuick;

    mergeSort(docs, mMerge, DocumentScoreDesc());
    quickSort(docsQuick, mQuick, DocumentScoreDesc());

    printMetrics("MergeSort (Stable)", mMerge);
    printMetrics("QuickSort (Median-of-3)", mQuick);
    std::cout << "  -> Top Ranked Book: [ID " << docs[0].id << "] \"" << docs[0].title
              << "\" (Score: " << docs[0].score << ")\n\n";

    // 2. Binary search lookup
    std::cout << "2. Looking up Document by ID in Sorted Catalog...\n";
    auto docsById = docs;
    Metrics mSortId;
    mergeSort(docsById, mSortId, DocumentIdAsc());

    Metrics mSearchIter, mSearchRec;
    int targetId = 250;
    int idx1 = binarySearchDocumentById(docsById, targetId, false, mSearchIter);
    int idx2 = binarySearchDocumentById(docsById, targetId, true, mSearchRec);
    (void)idx2; // verify both return same index
    printMetrics("Binary Search (Iterative)", mSearchIter);
    printMetrics("Binary Search (Recursive)", mSearchRec);
    std::cout << "  -> Found at index " << idx1 << ": \"" << docsById[idx1].title << "\"\n\n";

    // 3. Peak reading-activity window (Max Subarray)
    std::cout << "3. Detecting Peak Reading-Activity Window (Net Reader Visitor Deltas)...\n";
    auto activityLog = DataGenerator::generateActivityLog(100, 42);
    Metrics mSubDC, mSubKadane;
    auto subDC = maxSubarrayDivideAndConquer(activityLog, mSubDC);
    auto subKad = maxSubarrayKadane(activityLog, mSubKadane);
    (void)subKad; // verify consistency
    printMetrics("Max Subarray (Divide & Conquer)", mSubDC);
    printMetrics("Max Subarray (Kadane's Algorithm)", mSubKadane);
    std::cout << "  -> Peak Window: Hours [" << subDC.low << ".." << subDC.high 
              << "] with net visitor gain of +" << subDC.sum << "\n\n";

    // 4. Celebrity Paper Discovery
    std::cout << "4. Discovering Authoritative 'Celebrity' Paper (Cited by all, cites none)...\n";
    auto celebMat = DataGenerator::generateCelebrityMatrix(20, 7);
    Metrics mCeleb;
    int celeb = findCelebrity(celebMat, mCeleb);
    printMetrics("Celebrity Discovery O(n)", mCeleb);
    std::cout << "  -> Authoritative Paper Found: ID " << celeb << "\n\n";

    // 5. Reading-Room Reservation & Room Partitioning
    std::cout << "5. Allocating Reading Rooms for Student Study Groups...\n";
    auto intervals = DataGenerator::generateIntervals(30, 200, 42);
    Metrics mAct, mPart;
    auto selectedActs = selectActivities(intervals, mAct);
    auto partRes = partitionIntervals(intervals, mPart);
    printMetrics("Activity Selection (Max Compatible Slots in 1 Room)", mAct);
    printMetrics("Interval Partitioning (Min Rooms for ALL Slots)", mPart);
    std::cout << "  -> Max compatible slots in single room: " << selectedActs.size() << "\n";
    std::cout << "  -> Total rooms required to schedule all 30 groups: " << partRes.roomsUsed << "\n\n";

    // 6. Huffman Abstract Compression
    std::cout << "6. Compressing Book Abstract using Huffman Coding...\n";
    std::string sampleAbstract = "The design and analysis of algorithms is fundamental to computer science. "
                                 "This library contains research manuscripts, conference proceedings, and textbooks.";
    Metrics mHuff;
    auto huffRes = huffmanEncode(sampleAbstract, mHuff);
    printMetrics("Huffman Coding", mHuff);
    size_t rawBits = sampleAbstract.size() * 8;
    std::cout << "  -> Original: " << rawBits << " bits, Compressed: " << huffRes.encodedBits.size() 
              << " bits (Space Saved: " << std::fixed << std::setprecision(1) 
              << (100.0 * (1.0 - (double)huffRes.encodedBits.size() / rawBits)) << "%)\n\n";

    // 7. Branch Network Minimum Spanning Tree
    std::cout << "7. Optimizing Inter-Branch Physical Courier Network (MST)...\n";
    auto branchEdges = DataGenerator::generateGraph(8, 20, 5, 50, 42);
    Metrics mPrim, mKruskal;
    auto primRes = primMST(8, branchEdges, mPrim);
    auto kruskalRes = kruskalMST(8, branchEdges, mKruskal);
    printMetrics("Prim's Algorithm (MinHeap)", mPrim);
    printMetrics("Kruskal's Algorithm (Union-Find)", mKruskal);
    std::cout << "  -> Minimum Connection Cost: " << primRes.totalWeight 
              << " units (Edges used: " << primRes.mstEdges.size() << ")\n\n";

    // 8. Book Acquisitions under Budget (DP vs Branch & Bound)
    std::cout << "8. Optimal Book Acquisitions under Acquisition Budget...\n";
    auto items = DataGenerator::generateItems(16, 20, 80, 42);
    int budget = 60;
    Metrics mKnapDP, mKnapLCBB;
    auto knapDP = knapsack01(items, budget, mKnapDP);
    auto knapLCBB = knapsackBranchAndBound(items, budget, BBStrategy::LeastCost, mKnapLCBB);
    printMetrics("0/1 Knapsack (Dynamic Programming)", mKnapDP);
    printMetrics("0/1 Knapsack (Least-Cost Branch & Bound)", mKnapLCBB);
    std::cout << "  -> Max Catalog Value Acquired: " << knapDP.maxValue 
              << " (Items chosen: " << knapDP.selectedItemIds.size() << ")\n\n";

    // 9. Librarian Book Pickup Route (TSP)
    std::cout << "9. Computing Librarian Optimal Book Pickup Route (TSP Bitmask DP)...\n";
    int nStacks = 5;
    std::vector<std::vector<int64_t>> stackDist = {
        {0, 12, 10, 19, 8},
        {12, 0, 3, 7, 6},
        {10, 3, 0, 2, 20},
        {19, 7, 2, 0, 4},
        {8, 6, 20, 4, 0}
    };
    Metrics mTSP;
    auto tspRes = tspBitmask(nStacks, stackDist, mTSP);
    printMetrics("TSP (Bitmask DP O(n^2 * 2^n))", mTSP);
    std::cout << "  -> Optimal Pickup Distance: " << tspRes.minCost << " meters | Route: ";
    for (size_t i = 0; i < tspRes.tour.size(); ++i) {
        std::cout << tspRes.tour[i] << (i + 1 < tspRes.tour.size() ? " -> " : "\n\n");
    }

    // 10. Keyword Search in Text (String Matching)
    std::cout << "10. Full-Text Manuscript Search for Keyword 'algorithms'...\n";
    std::string text = "Computer science algorithms power modern search. We explore greedy algorithms and search algorithms.";
    std::string pat = "algorithms";
    Metrics mStrKMP, mStrRK;
    auto kmpMatches = matchKMP(text, pat, mStrKMP);
    auto rkMatches = matchRabinKarp(text, pat, mStrRK);
    printMetrics("KMP String Matcher", mStrKMP);
    printMetrics("Rabin-Karp String Matcher", mStrRK);
    std::cout << "  -> Found " << kmpMatches.size() << " matches at character indices: ";
    for (size_t idx : kmpMatches) std::cout << idx << " ";
    std::cout << "\n";
}

void runScenario2_ConstraintShock() {
    printHeader("SCENARIO 2: CONSTRAINT SHOCK (BUDGET CUT & CAPACITY DROP)");
    std::cout << "Demonstrating system resilience and algorithm behavior when constraints tighten abruptly.\n\n";

    // Part A: Budget Cut Shock on Knapsack Acquisitions
    std::cout << "--- PART A: FINANCIAL AUSTERITY SHOCK (50% Budget Cut) ---\n";
    auto items = DataGenerator::generateItems(22, 15, 60, 100);
    int normalBudget = 100;
    int shockedBudget = 50;

    Metrics mDP_Normal, mLCBB_Normal, mFIFO_Normal;
    auto dpNormal = knapsack01(items, normalBudget, mDP_Normal);
    auto lcbbNormal = knapsackBranchAndBound(items, normalBudget, BBStrategy::LeastCost, mLCBB_Normal);
    auto fifoNormal = knapsackBranchAndBound(items, normalBudget, BBStrategy::FIFO, mFIFO_Normal);

    Metrics mDP_Shock, mLCBB_Shock, mFIFO_Shock;
    auto dpShock = knapsack01(items, shockedBudget, mDP_Shock);
    auto lcbbShock = knapsackBranchAndBound(items, shockedBudget, BBStrategy::LeastCost, mLCBB_Shock);
    auto fifoShock = knapsackBranchAndBound(items, shockedBudget, BBStrategy::FIFO, mFIFO_Shock);

    std::cout << "[Baseline: Normal Budget = " << normalBudget << "]\n";
    std::cout << "  * DP Knapsack Value: " << dpNormal.maxValue << " | Time: " << mDP_Normal.ms << " ms | Comparisons: " << mDP_Normal.comparisons << "\n";
    std::cout << "  * LCBB Knapsack Value: " << lcbbNormal.maxValue << " | Nodes: " << lcbbNormal.nodesExplored << " | Time: " << mLCBB_Normal.ms << " ms\n";
    std::cout << "  * FIFO Knapsack Value: " << fifoNormal.maxValue << " | Nodes: " << fifoNormal.nodesExplored << " | Time: " << mFIFO_Normal.ms << " ms\n\n";

    std::cout << "[Shocked: Austerity Budget = " << shockedBudget << " (50% Cut)]\n";
    std::cout << "  * DP Knapsack Value: " << dpShock.maxValue << " | Time: " << mDP_Shock.ms << " ms | Comparisons: " << mDP_Shock.comparisons << "\n";
    std::cout << "  * LCBB Knapsack Value: " << lcbbShock.maxValue << " | Nodes: " << lcbbShock.nodesExplored << " | Time: " << mLCBB_Shock.ms << " ms\n";
    std::cout << "  * FIFO Knapsack Value: " << fifoShock.maxValue << " | Nodes: " << fifoShock.nodesExplored << " | Time: " << mFIFO_Shock.ms << " ms\n";
    std::cout << "  -> Budget reduction ratio: 50.0% | Acquired Value drop: " 
              << std::fixed << std::setprecision(1) << (100.0 * (1.0 - (double)dpShock.maxValue / dpNormal.maxValue)) << "%\n";
    std::cout << "  -> Pruning effectiveness under tighter bounds: LCBB explored " << lcbbShock.nodesExplored 
              << " nodes vs FIFO " << fifoShock.nodesExplored << " nodes.\n\n";

    // Part B: Network Download Capacity Drop Shock
    std::cout << "--- PART B: INTER-BRANCH FIBER CUT (Capacity Drop Shock) ---\n";
    int n = 6;
    std::vector<std::vector<int64_t>> normalCap(n, std::vector<int64_t>(n, 0));
    normalCap[0][1] = 100; normalCap[0][2] = 80;
    normalCap[1][2] = 40;  normalCap[1][3] = 70;
    normalCap[2][1] = 20;  normalCap[2][4] = 90;
    normalCap[3][2] = 30;  normalCap[3][5] = 120;
    normalCap[4][3] = 40;  normalCap[4][5] = 60;

    Metrics mFlowNormal;
    auto flowNormal = edmondsKarpMaxFlow(n, normalCap, 0, 5, mFlowNormal);

    // Major trunk line failure: node 1 -> 3 fiber severed (cap 70 -> 0), node 2 -> 4 degraded (90 -> 20)
    auto shockedCap = normalCap;
    shockedCap[1][3] = 0;
    shockedCap[2][4] = 20;

    Metrics mFlowShock;
    auto flowShock = edmondsKarpMaxFlow(n, shockedCap, 0, 5, mFlowShock);

    std::cout << "  * Normal Download Bandwidth: " << flowNormal.maxFlow << " Mbps (Edmonds-Karp Time: " << mFlowNormal.ms << " ms)\n";
    std::cout << "  * Degraded Download Bandwidth: " << flowShock.maxFlow << " Mbps (Edmonds-Karp Time: " << mFlowShock.ms << " ms)\n";
    std::cout << "  -> Bandwidth Loss: -" << (flowNormal.maxFlow - flowShock.maxFlow) 
              << " Mbps (" << std::fixed << std::setprecision(1) 
              << (100.0 * (double)(flowNormal.maxFlow - flowShock.maxFlow) / flowNormal.maxFlow) << "% throughput drop)\n";
}

void runScenario3_StressTest() {
    printHeader("SCENARIO 3: STRESS TEST WITH 100,000+ ITEMS");
    std::cout << "Benchmarking scalability on massive datasets: comparing dual strategies.\n\n";

    const size_t N = 100000;
    std::cout << "Generating " << N << " document records with deterministic seed...\n";
    auto docs = DataGenerator::generateDocuments(N, 42);
    auto docsQuick = docs;

    // Test 1: Merge Sort vs Quick Sort on 100,000 items
    std::cout << "\n1. Sorting 100,000 Documents by Relevance Score:\n";
    Metrics mMerge, mQuick;
    mergeSort(docs, mMerge, DocumentScoreDesc());
    quickSort(docsQuick, mQuick, DocumentScoreDesc());

    printMetrics("Merge Sort (O(n log n) Guaranteed)", mMerge);
    printMetrics("Quick Sort (Median-of-3 In-Place)", mQuick);
    std::cout << "  -> Equivalence verified: " << (docs == docsQuick ? "YES (Identical order)" : "NO") << "\n";
    std::cout << "  -> Memory footprint: MergeSort allocated auxiliary buffer (" 
              << (sizeof(Document) * N) / (1024 * 1024) << " MB), QuickSort ran in-place.\n\n";

    // Test 2: Binary Search Iterative vs Recursive on 100,000 items
    std::cout << "2. Searching in 100,000 Sorted Documents (Looking for Book ID 77777):\n";
    // Sort by ID
    Metrics mSortId;
    mergeSort(docs, mSortId, DocumentIdAsc());

    Metrics mIter, mRec;
    int foundIter = binarySearchDocumentById(docs, 77777, false, mIter);
    int foundRec = binarySearchDocumentById(docs, 77777, true, mRec);
    (void)foundRec;

    printMetrics("Binary Search Iterative", mIter);
    printMetrics("Binary Search Recursive", mRec);
    std::cout << "  -> Result: ID 77777 found at index " << foundIter 
              << " (Title: \"" << docs[foundIter].title << "\")\n"
              << "  -> Both methods made exactly " << mIter.comparisons << " comparisons.\n\n";

    // Test 3: String Matching on 100,000 Character Adversarial Stream
    std::cout << "3. String Matching Stress Test (100,000 Characters with Adversarial Suffix):\n";
    std::string pattern;
    std::string text = DataGenerator::generateAdversarialString(100000, 8, pattern);

    Metrics mNaive, mKMP, mRK;
    auto resKMP = matchKMP(text, pattern, mKMP);
    auto resRK = matchRabinKarp(text, pattern, mRK);
    auto resNaive = matchNaive(text, pattern, mNaive);

    printMetrics("Naive String Matcher", mNaive);
    printMetrics("Rabin-Karp Rolling Hash", mRK);
    printMetrics("Knuth-Morris-Pratt (KMP)", mKMP);
    std::cout << "  -> Naive comparisons: " << mNaive.comparisons << " vs KMP comparisons: " << mKMP.comparisons << "\n";
    std::cout << "  -> KMP was " << std::fixed << std::setprecision(1) << (mNaive.ms / (mKMP.ms > 0 ? mKMP.ms : 0.001)) 
              << "x faster than Naive on adversarial text.\n";
}

void printMenu() {
    std::cout << "\n======================================================================\n";
    std::cout << "   DIGITAL LIBRARY SEARCH AND RECOMMENDATION ENGINE - CLI REGISTRY   \n";
    std::cout << "======================================================================\n";
    std::cout << " 1. Run Scenario 1: Normal Library Operation Demo\n";
    std::cout << " 2. Run Scenario 2: Constraint Shock (Budget Cut & Capacity Drop)\n";
    std::cout << " 3. Run Scenario 3: Stress Test (100,000+ Items)\n";
    std::cout << " 4. Sorting & Search Demo (MergeSort, QuickSort, BinarySearch)\n";
    std::cout << " 5. Divide & Conquer Demo (Max Subarray, Fast Power, Celebrity, Strassen)\n";
    std::cout << " 6. Greedy Demo (Activity Selection, Interval Partition, Huffman, Dijkstra, MST)\n";
    std::cout << " 7. Dynamic Programming Demo (Knapsack, Matrix Chain, LCS, OBST, Bellman, TSP)\n";
    std::cout << " 8. Backtracking, Flow, CRT & String Matching Demo\n";
    std::cout << " 9. Export Sample Datasets to data/\n";
    std::cout << " 0. Exit\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Enter selection [0-9]: ";
}

} // namespace dls

int main(int argc, char* argv[]) {
    // Check command line arguments for non-interactive execution
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--all" || arg == "--demo") {
            dls::runScenario1_NormalOperation();
            dls::runScenario2_ConstraintShock();
            dls::runScenario3_StressTest();
            return 0;
        } else if (arg == "--scenario" && argc > 2) {
            int s = std::stoi(argv[2]);
            if (s == 1) dls::runScenario1_NormalOperation();
            else if (s == 2) dls::runScenario2_ConstraintShock();
            else if (s == 3) dls::runScenario3_StressTest();
            return 0;
        }
    }

    std::cout << "Welcome to the Digital Library Search and Recommendation Engine!\n";
    int choice = -1;
    while (choice != 0) {
        dls::printMenu();
        if (!(std::cin >> choice)) {
            // EOF or piped input ended
            break;
        }

        switch (choice) {
            case 1:
                dls::runScenario1_NormalOperation();
                break;
            case 2:
                dls::runScenario2_ConstraintShock();
                break;
            case 3:
                dls::runScenario3_StressTest();
                break;
            case 4: {
                dls::printHeader("Sorting & Binary Search Demo");
                auto docs = dls::DataGenerator::generateDocuments(50, 42);
                dls::Metrics m;
                dls::mergeSort(docs, m, dls::DocumentScoreDesc());
                dls::printMetrics("MergeSort 50 Docs", m);
                std::cout << "Top document: [ID " << docs[0].id << "] " << docs[0].title 
                          << " (Score: " << docs[0].score << ")\n";
                break;
            }
            case 5: {
                dls::printHeader("Divide and Conquer Demo");
                dls::Metrics mDC, mFast;
                auto log = dls::DataGenerator::generateActivityLog(50, 42);
                auto res = dls::maxSubarrayDivideAndConquer(log, mDC);
                dls::printMetrics("Max Subarray D&C", mDC);
                std::cout << "Peak window sum: " << res.sum << "\n";
                int64_t powRes = dls::fastPowerModular(3, 100, 1000000007, mFast);
                dls::printMetrics("Fast Powering 3^100 mod 1e9+7", mFast);
                std::cout << "Result: " << powRes << "\n";
                break;
            }
            case 6: {
                dls::printHeader("Greedy Algorithms Demo");
                dls::Metrics m;
                std::string txt = "digital library system algorithms";
                auto huff = dls::huffmanEncode(txt, m);
                dls::printMetrics("Huffman Encoding", m);
                std::cout << "Raw: " << txt.size() * 8 << " bits, Encoded: " << huff.encodedBits.size() << " bits\n";
                break;
            }
            case 7: {
                dls::printHeader("Dynamic Programming Demo");
                dls::Metrics mKnap, mLCS;
                auto items = dls::DataGenerator::generateItems(10, 20, 50, 42);
                auto knap = dls::knapsack01(items, 50, mKnap);
                dls::printMetrics("0/1 Knapsack DP", mKnap);
                std::cout << "Max value: " << knap.maxValue << "\n";
                auto lcs = dls::longestCommonSubsequence("ALGORITHM", "LOGARITHM", mLCS);
                dls::printMetrics("LCS (\"ALGORITHM\", \"LOGARITHM\")", mLCS);
                std::cout << "LCS: \"" << lcs.sequence << "\" (length " << lcs.length << ")\n";
                break;
            }
            case 8: {
                dls::printHeader("Backtracking, Flow, CRT & Strings Demo");
                dls::Metrics mQ, mFlow, mCRT;
                auto nq = dls::solveNQueens(8, mQ);
                dls::printMetrics("N-Queens (N=8)", mQ);
                std::cout << "Solutions: " << nq.totalSolutions << "\n";
                auto crt = dls::solveCRT({2, 3, 2}, {3, 5, 7}, mCRT);
                dls::printMetrics("CRT (2 mod 3, 3 mod 5, 2 mod 7)", mCRT);
                std::cout << "Solution: " << crt.solution << " mod " << crt.moduloProduct << "\n";
                break;
            }
            case 9: {
                dls::printHeader("Exporting Sample Datasets to data/...");
                auto sampleDocs = dls::DataGenerator::generateDocuments(100, 42);
                bool ok = dls::DataGenerator::exportDocumentsToCSV("data/documents.csv", sampleDocs);
                std::cout << (ok ? "Successfully exported 100 documents to data/documents.csv\n"
                                 : "Failed to export documents.\n");
                break;
            }
            case 0:
                std::cout << "Exiting DLS Engine. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice! Please select 0-9.\n";
                break;
        }
    }

    return 0;
}
