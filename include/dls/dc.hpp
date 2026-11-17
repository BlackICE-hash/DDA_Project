#pragma once

#include "dls/types.hpp"
#include <vector>
#include <cstdint>

namespace dls {

// Result for Maximum Subarray problem
struct SubarrayResult {
    int low{0};
    int high{0};
    int64_t sum{0};

    bool operator==(const SubarrayResult& other) const {
        return low == other.low && high == other.high && sum == other.sum;
    }
};

/**
 * @brief Maximum Subarray using Divide and Conquer.
 * 
 * IDEA:
 * Split array into left and right halves. The maximal contiguous subarray either
 * resides wholly in the left half, wholly in the right half, or crosses the midpoint.
 * 
 * TIME COMPLEXITY:
 * T(n) = 2T(n/2) + O(n) => O(n log n).
 * 
 * SPACE COMPLEXITY:
 * O(log n) call stack space.
 * 
 * WHEN IT FAILS:
 * If the input array is empty (handled by returning sum 0 and invalid indices).
 */
SubarrayResult maxSubarrayDivideAndConquer(const std::vector<int>& arr, Metrics& metrics);

/**
 * Linear O(n) Kadane's algorithm provided for comparison and oracle verification.
 */
SubarrayResult maxSubarrayKadane(const std::vector<int>& arr, Metrics& metrics);

/**
 * @brief Fast Powering (Modular Exponentiation) using Divide and Conquer.
 * 
 * IDEA:
 * Compute base^exp mod m by binary exponentiation:
 * If exp is even: base^exp = (base^(exp/2))^2
 * If exp is odd:  base^exp = base * (base^(exp/2))^2
 * 
 * TIME COMPLEXITY:
 * O(log exp).
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space (iterative) or O(log exp) stack space (recursive).
 * 
 * WHEN IT FAILS:
 * When modulus m <= 0, or exp < 0.
 */
int64_t fastPowerModular(int64_t base, int64_t exp, int64_t mod, Metrics& metrics);

/**
 * Pure integer fast powering: computes base^exp.
 */
int64_t fastPower(int64_t base, int64_t exp, Metrics& metrics);

/**
 * @brief Celebrity Discovery in Citation Matrix.
 * 
 * IDEA:
 * A celebrity paper is cited by all other n-1 papers and cites none.
 * Divide & conquer / elimination:
 * Compare paper A and B. If A cites B, A cannot be a celebrity (eliminate A).
 * If A does not cite B, B cannot be a celebrity (eliminate B).
 * After n-1 eliminations, verify the survivor in 2(n-1) checks.
 * 
 * TIME COMPLEXITY:
 * O(n) queries.
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space.
 * 
 * WHEN IT FAILS:
 * Returns -1 if no celebrity exists (e.g. cyclical citations or candidate cites someone).
 */
int findCelebrity(const std::vector<std::vector<int>>& citationMatrix, Metrics& metrics);

/**
 * Matrix representation as 2D vector of int64_t.
 */
using Matrix = std::vector<std::vector<int64_t>>;

/**
 * @brief Classical O(n^3) Matrix Multiplication.
 * 
 * IDEA:
 * Triple nested loop: C[i][j] = sum_k(A[i][k] * B[k][j]).
 * 
 * TIME COMPLEXITY:
 * O(n^3).
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary memory beyond output.
 * 
 * WHEN IT FAILS:
 * When matrix dimensions do not match (A.cols != B.rows).
 */
Matrix multiplyMatrixClassical(const Matrix& A, const Matrix& B, Metrics& metrics);

/**
 * @brief Strassen's Matrix Multiplication using Divide and Conquer.
 * 
 * IDEA:
 * Partitions matrices into four n/2 x n/2 submatrices. Uses 7 recursive multiplications
 * instead of 8:
 * T(n) = 7T(n/2) + O(n^2) => O(n^log2(7)) ≈ O(n^2.807).
 * Switches to classical multiplication for sizes below threshold.
 * 
 * TIME COMPLEXITY:
 * O(n^2.807).
 * 
 * SPACE COMPLEXITY:
 * O(n^2) auxiliary memory for submatrices at recursion levels.
 * 
 * WHEN IT FAILS:
 * Numerically less stable than classical for floating point numbers; large constant factor
 * overhead for very small matrices (mitigated by threshold cutoff).
 */
Matrix multiplyMatrixStrassen(const Matrix& A, const Matrix& B, Metrics& metrics, size_t threshold = 16);

} // namespace dls
