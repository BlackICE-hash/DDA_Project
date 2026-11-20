#pragma once

#include "dls/types.hpp"
#include <vector>
#include <cstdint>

// NOTE: "using Matrix = std::vector<std::vector<int64_t>>" is a type alias (nickname).
// It means Matrix is just a 2D vector (a grid of rows and columns of numbers).

namespace dls {

// Result for Maximum Subarray problem
struct SubarrayResult {
    int low{0};
    int high{0};
    int64_t sum{0};

    // WHAT:    Checks equality between two SubarrayResult structs.
    // IDEA:    Returns true if low index, high index, and total sum all match.
    // EXAMPLE: res1(low=1, high=3, sum=15) == res2(low=1, high=3, sum=15) -> true
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Used in test suites to verify that D&C and Kadane's algorithm give identical results.
    bool operator==(const SubarrayResult& other) const {
        return low == other.low && high == other.high && sum == other.sum;
    }
};

// WHAT:    Finds the contiguous reading window with peak visitor surge using Divide and Conquer.
// IDEA:    Recursively splits the array into left and right halves. The max subarray is either
//          wholly in the left half, wholly in the right half, or crosses the midpoint.
// EXAMPLE: maxSubarrayDivideAndConquer([-2, 1, -3, 4, -1, 2, 1, -5, 4], metrics) -> sum 6 in [3..6]
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(log n) recursion stack
// VIVA:    The recurrence T(n) = 2T(n/2) + O(n) solves to O(n log n) via Master Theorem Case 2.
SubarrayResult maxSubarrayDivideAndConquer(const std::vector<int>& arr, Metrics& metrics);

// WHAT:    Finds the maximum contiguous subarray in linear time using Kadane's algorithm.
// IDEA:    Maintains a running sum, extending the window if positive and resetting to 0 if negative.
// EXAMPLE: maxSubarrayKadane([5, -2, 8], metrics) -> sum 11 in [0..2]
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(1)
// VIVA:    Kadane runs in O(n) time, serving as an independent oracle to verify Divide-and-Conquer.
SubarrayResult maxSubarrayKadane(const std::vector<int>& arr, Metrics& metrics);

// WHAT:    Computes (base^exp) mod m using binary modular exponentiation.
// IDEA:    Repeatedly squares the base and halves the exponent: if exp is odd, multiplies result by base.
// EXAMPLE: fastPowerModular(2, 10, 1000, metrics) -> 1024 mod 1000 = 24
// TIME:    best O(1) / avg O(log exp) / worst O(log exp)      SPACE: O(1)
// VIVA:    Applying modulo after every multiplication prevents 64-bit integer overflow.
int64_t fastPowerModular(int64_t base, int64_t exp, int64_t mod, Metrics& metrics);

// WHAT:    Computes base^exp using binary exponentiation without modulo.
// IDEA:    Halves exponent and squares base recursively/iteratively in logarithmic steps.
// EXAMPLE: fastPower(3, 4, metrics) -> 81 in 2 multiplications
// TIME:    best O(1) / avg O(log exp) / worst O(log exp)      SPACE: O(1)
// VIVA:    Reduces multiplication operations from O(exp) down to O(log exp).
int64_t fastPower(int64_t base, int64_t exp, Metrics& metrics);

// WHAT:    Finds an authoritative paper in a citation network in linear queries.
// IDEA:    A celebrity is cited by all and cites none. Compares pairs (A, B): if A cites B,
//          eliminate A; otherwise eliminate B. Verifies the single survivor with 2(n-1) checks.
// EXAMPLE: Paper 7 cited by all 19 peers and citing none -> returns 7 in exactly 57 queries
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(1)
// VIVA:    Avoids inspecting all n x n entries (O(n^2)), finding the celebrity in only 3(n-1) checks.
int findCelebrity(const std::vector<std::vector<int>>& citationMatrix, Metrics& metrics);

/**
 * Matrix representation as 2D vector of int64_t.
 */
using Matrix = std::vector<std::vector<int64_t>>;

// WHAT:    Multiplies two matrices using the classical triple-nested loop algorithm.
// IDEA:    Computes each entry C[i][j] as the dot product of row A[i] and column B[j].
// EXAMPLE: [[1, 2], [3, 4]] * [[5, 6], [7, 8]] -> [[19, 22], [43, 50]]
// TIME:    best O(n^3) / avg O(n^3) / worst O(n^3)      SPACE: O(n^2) for result matrix
// VIVA:    Performs exactly n^3 scalar multiplications and n^3 - n^2 additions.
Matrix multiplyMatrixClassical(const Matrix& A, const Matrix& B, Metrics& metrics);

// WHAT:    Multiplies two square matrices using Strassen's sub-cubic divide-and-conquer method.
// IDEA:    Partitions matrices into 4 quadrants and computes 7 recursive products instead of 8.
//          Switches to classical multiplication below threshold for efficiency.
// EXAMPLE: Multiplies two 64x64 matrices faster than classical multiplication
// TIME:    best O(n^2.807) / avg O(n^2.807) / worst O(n^2.807)      SPACE: O(n^2) auxiliary buffers
// VIVA:    The recurrence T(n) = 7T(n/2) + O(n^2) yields O(n^log2(7)) ≈ O(n^2.807).
Matrix multiplyMatrixStrassen(const Matrix& A, const Matrix& B, Metrics& metrics, size_t threshold = 16);

} // namespace dls
