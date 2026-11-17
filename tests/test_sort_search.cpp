#include "dls/sort.hpp"
#include "dls/search.hpp"
#include "dls/generator.hpp"
#include <iostream>
#include <cassert>
#include <algorithm> // Allowed ONLY in tests as oracle!
#include <random>

void testMergeSort() {
    std::cout << "[RUNNING] testMergeSort...\n";
    dls::Metrics m;

    // Edge case: Empty
    std::vector<int> empty;
    dls::mergeSort(empty, m);
    assert(empty.empty());

    // Edge case: Single element
    std::vector<int> single = {42};
    dls::mergeSort(single, m);
    assert(single.size() == 1 && single[0] == 42);

    // Edge case: Duplicates
    std::vector<int> dups = {5, 2, 5, 1, 2, 5, 3};
    auto dupsOracle = dups;
    std::sort(dupsOracle.begin(), dupsOracle.end());
    m.reset();
    dls::mergeSort(dups, m);
    assert(dups == dupsOracle);
    assert(m.comparisons > 0);

    // Edge case: Already sorted and reverse sorted
    std::vector<int> sortedArr = {1, 2, 3, 4, 5, 6, 7};
    auto sortedCopy = sortedArr;
    dls::mergeSort(sortedCopy, m);
    assert(sortedCopy == sortedArr);

    std::vector<int> revArr = {7, 6, 5, 4, 3, 2, 1};
    dls::mergeSort(revArr, m);
    assert(revArr == sortedArr);

    // Random test with std::sort oracle
    std::mt19937 rng(12345);
    std::vector<int> large(500);
    for (auto& x : large) x = rng() % 1000;
    auto largeOracle = large;
    std::sort(largeOracle.begin(), largeOracle.end());

    m.reset();
    dls::mergeSort(large, m);
    assert(large == largeOracle);

    // Documents sorting by score descending
    auto docs = dls::DataGenerator::generateDocuments(50, 42);
    auto docsOracle = docs;
    std::sort(docsOracle.begin(), docsOracle.end(), [](const dls::Document& a, const dls::Document& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.id < b.id;
    });

    m.reset();
    dls::mergeSort(docs, m, dls::DocumentScoreDesc());
    assert(docs == docsOracle);

    std::cout << "[PASS] testMergeSort passed (comparisons=" << m.comparisons << ", ms=" << m.ms << ")\n";
}

void testQuickSort() {
    std::cout << "[RUNNING] testQuickSort...\n";
    dls::Metrics m;

    // Edge case: Empty
    std::vector<int> empty;
    dls::quickSort(empty, m);
    assert(empty.empty());

    // Edge case: Single element
    std::vector<int> single = {99};
    dls::quickSort(single, m);
    assert(single.size() == 1 && single[0] == 99);

    // Duplicates
    std::vector<int> dups = {4, 1, 4, 2, 4, 3};
    auto dupsOracle = dups;
    std::sort(dupsOracle.begin(), dupsOracle.end());
    m.reset();
    dls::quickSort(dups, m);
    assert(dups == dupsOracle);

    // Random test with std::sort oracle
    std::mt19937 rng(54321);
    std::vector<int> large(500);
    for (auto& x : large) x = rng() % 1000;
    auto largeOracle = large;
    std::sort(largeOracle.begin(), largeOracle.end());

    m.reset();
    dls::quickSort(large, m);
    assert(large == largeOracle);

    // Compare LastElement pivot vs MedianOfThree on already sorted array
    std::vector<int> sortedArr(100);
    for (int i = 0; i < 100; ++i) sortedArr[i] = i;

    auto sortedArr1 = sortedArr;
    dls::Metrics mNaive;
    dls::quickSortEx(sortedArr1, mNaive, dls::PivotStrategy::LastElement);

    auto sortedArr2 = sortedArr;
    dls::Metrics mBalanced;
    dls::quickSortEx(sortedArr2, mBalanced, dls::PivotStrategy::MedianOfThree);

    assert(sortedArr1 == sortedArr);
    assert(sortedArr2 == sortedArr);
    // Naive pivot on already sorted array does O(n^2) comparisons, roughly n*(n-1)/2 = 4950
    // Median-of-three does significantly fewer comparisons on sorted input!
    assert(mNaive.comparisons > mBalanced.comparisons);
    std::cout << "  [Adversarial Sorted Input Check] Naive pivot comparisons: " << mNaive.comparisons
              << ", Median-of-3 comparisons: " << mBalanced.comparisons << "\n";

    // Documents sorting by ID ascending
    auto docs = dls::DataGenerator::generateDocuments(50, 42);
    auto docsOracle = docs;
    std::sort(docsOracle.begin(), docsOracle.end(), [](const dls::Document& a, const dls::Document& b) {
        return a.id < b.id;
    });

    m.reset();
    dls::quickSort(docs, m, dls::DocumentIdAsc());
    assert(docs == docsOracle);

    std::cout << "[PASS] testQuickSort passed\n";
}

void testBinarySearch() {
    std::cout << "[RUNNING] testBinarySearch...\n";
    dls::Metrics mIter;
    dls::Metrics mRec;

    // Edge case: Empty
    std::vector<int> empty;
    assert(dls::binarySearchIterative(empty, 10, mIter) == -1);
    assert(dls::binarySearchRecursive(empty, 10, mRec) == -1);

    // Edge case: Single element
    std::vector<int> single = {42};
    assert(dls::binarySearchIterative(single, 42, mIter) == 0);
    assert(dls::binarySearchRecursive(single, 42, mRec) == 0);
    assert(dls::binarySearchIterative(single, 10, mIter) == -1);
    assert(dls::binarySearchRecursive(single, 10, mRec) == -1);

    // Standard sorted vector
    std::vector<int> arr = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for (size_t i = 0; i < arr.size(); ++i) {
        int target = arr[i];
        int idxIter = dls::binarySearchIterative(arr, target, mIter);
        int idxRec = dls::binarySearchRecursive(arr, target, mRec);
        assert(idxIter == static_cast<int>(i));
        assert(idxRec == static_cast<int>(i));
    }

    // Missing elements
    assert(dls::binarySearchIterative(arr, 5, mIter) == -1);   // smaller than min
    assert(dls::binarySearchRecursive(arr, 5, mRec) == -1);
    assert(dls::binarySearchIterative(arr, 150, mIter) == -1); // larger than max
    assert(dls::binarySearchRecursive(arr, 150, mRec) == -1);
    assert(dls::binarySearchIterative(arr, 35, mIter) == -1);  // between elements
    assert(dls::binarySearchRecursive(arr, 35, mRec) == -1);

    // Document search by ID
    auto docs = dls::DataGenerator::generateDocuments(100, 42);
    dls::Metrics mSort;
    dls::mergeSort(docs, mSort, dls::DocumentIdAsc());

    dls::Metrics mDocSearch;
    int foundIdx1 = dls::binarySearchDocumentById(docs, 42, false, mDocSearch);
    int foundIdx2 = dls::binarySearchDocumentById(docs, 42, true, mDocSearch);
    assert(foundIdx1 != -1 && docs[foundIdx1].id == 42);
    assert(foundIdx2 == foundIdx1);

    int notFoundIdx = dls::binarySearchDocumentById(docs, 9999, false, mDocSearch);
    assert(notFoundIdx == -1);

    std::cout << "[PASS] testBinarySearch passed (Iterative & Recursive verified)\n";
}

int main() {
    std::cout << "=== Running Stage 2 Sorting & Searching Tests ===\n";
    testMergeSort();
    testQuickSort();
    testBinarySearch();
    std::cout << "All Stage 2 Tests Passed Successfully!\n";
    return 0;
}
