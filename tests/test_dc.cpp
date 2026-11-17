#include "dls/dc.hpp"
#include "dls/generator.hpp"
#include <iostream>
#include <cassert>
#include <random>

void testMaxSubarray() {
    std::cout << "[RUNNING] testMaxSubarray...\n";
    dls::Metrics mDC;
    dls::Metrics mKadane;

    // Edge case: Empty
    std::vector<int> empty;
    auto resEmpty = dls::maxSubarrayDivideAndConquer(empty, mDC);
    assert(resEmpty.sum == 0 && resEmpty.low == -1);

    // Edge case: Single element positive & negative
    std::vector<int> singlePos = {42};
    auto resSP = dls::maxSubarrayDivideAndConquer(singlePos, mDC);
    assert(resSP.sum == 42 && resSP.low == 0 && resSP.high == 0);

    std::vector<int> singleNeg = {-7};
    auto resSN = dls::maxSubarrayDivideAndConquer(singleNeg, mDC);
    assert(resSN.sum == -7 && resSN.low == 0 && resSN.high == 0);

    // Standard textbook array: [-2, 1, -3, 4, -1, 2, 1, -5, 4]
    // Max subarray is [4, -1, 2, 1] with sum 6
    std::vector<int> arr = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    auto resDC = dls::maxSubarrayDivideAndConquer(arr, mDC);
    auto resKadane = dls::maxSubarrayKadane(arr, mKadane);

    assert(resDC.sum == 6);
    assert(resKadane.sum == 6);
    assert(resDC.low == 3 && resDC.high == 6);

    // All negative numbers: [-8, -3, -6, -2, -5] -> max is -2
    std::vector<int> allNeg = {-8, -3, -6, -2, -5};
    auto resNegDC = dls::maxSubarrayDivideAndConquer(allNeg, mDC);
    auto resNegKadane = dls::maxSubarrayKadane(allNeg, mKadane);
    assert(resNegDC.sum == -2);
    assert(resNegKadane.sum == -2);

    // Generated library reader activity log test
    auto activityLog = dls::DataGenerator::generateActivityLog(200, 42);
    auto resLogDC = dls::maxSubarrayDivideAndConquer(activityLog, mDC);
    auto resLogKadane = dls::maxSubarrayKadane(activityLog, mKadane);
    assert(resLogDC.sum == resLogKadane.sum);

    std::cout << "[PASS] testMaxSubarray passed (D&C sum=" << resLogDC.sum 
              << ", Kadane sum=" << resLogKadane.sum << ")\n";
}

void testFastPower() {
    std::cout << "[RUNNING] testFastPower...\n";
    dls::Metrics m;

    // Integer power
    assert(dls::fastPower(2, 0, m) == 1);
    assert(dls::fastPower(2, 1, m) == 2);
    assert(dls::fastPower(2, 10, m) == 1024);
    assert(dls::fastPower(3, 5, m) == 243);
    assert(dls::fastPower(7, 3, m) == 343);

    // Modular power: 3^13 mod 1000
    // 3^13 = 1,594,323 => mod 1000 = 323
    int64_t modRes = dls::fastPowerModular(3, 13, 1000, m);
    assert(modRes == 323);

    // Large base and exponent with prime modulus
    int64_t largeMod = 1000000007;
    int64_t r1 = dls::fastPowerModular(2, 100, largeMod, m);
    assert(r1 > 0);

    // Edge cases
    assert(dls::fastPowerModular(5, 10, -5, m) == 0);
    assert(dls::fastPower(2, -3, m) == 0);

    std::cout << "[PASS] testFastPower passed (comparisons=" << m.comparisons << ")\n";
}

void testCelebrity() {
    std::cout << "[RUNNING] testCelebrity...\n";
    dls::Metrics m;

    // 1x1 matrix
    std::vector<std::vector<int>> m1 = {{0}};
    assert(dls::findCelebrity(m1, m) == 0);

    // Generated celebrity matrix with celebrity at index 3
    auto celebMat = dls::DataGenerator::generateCelebrityMatrix(8, 3);
    int celeb = dls::findCelebrity(celebMat, m);
    assert(celeb == 3);

    // Invalidate celebrity by having paper 3 cite paper 1
    celebMat[3][1] = 1;
    assert(dls::findCelebrity(celebMat, m) == -1);

    // Invalidate by having paper 2 NOT cite paper 3
    celebMat[3][1] = 0; // restore
    celebMat[2][3] = 0; // broke condition
    assert(dls::findCelebrity(celebMat, m) == -1);

    std::cout << "[PASS] testCelebrity passed\n";
}

void testMatrixMultiplication() {
    std::cout << "[RUNNING] testMatrixMultiplication (Classical vs Strassen)...\n";
    dls::Metrics mClassical;
    dls::Metrics mStrassen;

    // 2x2 test
    dls::Matrix A2 = {{1, 2}, {3, 4}};
    dls::Matrix B2 = {{5, 6}, {7, 8}};
    // Expected:
    // 1*5 + 2*7 = 19,  1*6 + 2*8 = 22
    // 3*5 + 4*7 = 43,  3*6 + 4*8 = 50
    dls::Matrix expected2 = {{19, 22}, {43, 50}};

    auto C2Class = dls::multiplyMatrixClassical(A2, B2, mClassical);
    auto C2Strass = dls::multiplyMatrixStrassen(A2, B2, mStrassen, 1);
    assert(C2Class == expected2);
    assert(C2Strass == expected2);

    // 8x8 random matrix (power of 2)
    auto matA = dls::DataGenerator::generateMatrix(8, 0, 5, 42);
    auto matB = dls::DataGenerator::generateMatrix(8, 0, 5, 84);
    dls::Matrix A8(8, std::vector<int64_t>(8));
    dls::Matrix B8(8, std::vector<int64_t>(8));
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            A8[i][j] = matA[i][j];
            B8[i][j] = matB[i][j];
        }
    }

    auto C8Class = dls::multiplyMatrixClassical(A8, B8, mClassical);
    auto C8Strass = dls::multiplyMatrixStrassen(A8, B8, mStrassen, 2);
    assert(C8Class == C8Strass);

    // 5x5 random matrix (non-power of 2, tests padding/unpadding)
    dls::Matrix A5(5, std::vector<int64_t>(5));
    dls::Matrix B5(5, std::vector<int64_t>(5));
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            A5[i][j] = (i + 1) * (j + 2);
            B5[i][j] = (i + 2) + (j + 1);
        }
    }

    auto C5Class = dls::multiplyMatrixClassical(A5, B5, mClassical);
    auto C5Strass = dls::multiplyMatrixStrassen(A5, B5, mStrassen, 2);
    assert(C5Class == C5Strass);

    std::cout << "[PASS] testMatrixMultiplication passed (Classical & Strassen matched on 2x2, 8x8, 5x5)\n";
}

int main() {
    std::cout << "=== Running Stage 3 Divide and Conquer Tests ===\n";
    testMaxSubarray();
    testFastPower();
    testCelebrity();
    testMatrixMultiplication();
    std::cout << "All Stage 3 Tests Passed Successfully!\n";
    return 0;
}
