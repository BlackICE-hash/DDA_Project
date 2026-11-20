#include "dls/dc.hpp"
#include <climits>
#include <stdexcept>
#include <cmath>

// NOTE: This file implements Divide-and-Conquer algorithms for library systems.
// "Divide and Conquer" means: 1) Divide into smaller subproblems, 2) Conquer recursively, 3) Combine solutions.

namespace dls {

// ============================================================================
// 1. MAXIMUM SUBARRAY
// ============================================================================

namespace {

// WHAT:    Finds maximum contiguous subarray crossing the midpoint mid.
// IDEA:    Expands left from mid to find left maximum sum, expands right from mid+1 to find right maximum sum,
//          and returns the combined span with sum leftSum + rightSum.
// EXAMPLE: [-2, 1, -3, 4] with mid=3 -> left max sum 4, right max sum 3 -> cross sum 7
// TIME:    best O(n) / avg O(n) / worst O(n) where n = high - low + 1      SPACE: O(1)
// VIVA:    Linear O(n) crossing scan makes the D&C recurrence T(n) = 2T(n/2) + O(n), giving O(n log n).
SubarrayResult findMaxCrossingSubarray(const std::vector<int>& arr, int low, int mid, int high, Metrics& metrics) {
    // Step 1: Find max subarray in left half ending at mid
    int64_t leftSum = LLONG_MIN;
    int64_t sum = 0;
    int maxLeft = mid;

    for (int i = mid; i >= low; --i) {
        sum += arr[i];
        metrics.comparisons++;
        if (sum > leftSum) {
            leftSum = sum;
            maxLeft = i;
        }
    }

    // Step 2: Find max subarray in right half starting at mid + 1
    int64_t rightSum = LLONG_MIN;
    sum = 0;
    int maxRight = mid + 1;

    for (int j = mid + 1; j <= high; ++j) {
        sum += arr[j];
        metrics.comparisons++;
        if (sum > rightSum) {
            rightSum = sum;
            maxRight = j;
        }
    }

    // Step 3: Combine cross sum
    return {maxLeft, maxRight, leftSum + rightSum};
}

// WHAT:    Recursive worker function for Maximum Subarray divide-and-conquer.
// IDEA:    Base case: 1 element. Divides at mid, recursively conquers left and right halves,
//          computes crossing sum, and returns the maximum of the three.
// EXAMPLE: arr=[-1, 4, -2, 5] -> splits to [-1, 4] and [-2, 5], crossing returns [4, -2, 5]=7
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(log n) recursion stack
// VIVA:    Base case 'low == high' returns single element directly without recursion.
SubarrayResult maxSubarrayRecursive(const std::vector<int>& arr, int low, int high, Metrics& metrics) {
    // Base case: exactly one element
    if (low == high) {
        return {low, high, static_cast<int64_t>(arr[low])};
    }

    // Step 1: Divide range into two halves
    int mid = low + (high - low) / 2;

    // Step 2: Conquer left and right subproblems
    SubarrayResult leftRes = maxSubarrayRecursive(arr, low, mid, metrics);
    SubarrayResult rightRes = maxSubarrayRecursive(arr, mid + 1, high, metrics);

    // Step 3: Find crossing subarray
    SubarrayResult crossRes = findMaxCrossingSubarray(arr, low, mid, high, metrics);

    // Step 4: Combine - return the maximum of the three possibilities
    metrics.comparisons++;
    if (leftRes.sum >= rightRes.sum && leftRes.sum >= crossRes.sum) {
        return leftRes;
    }
    metrics.comparisons++;
    if (rightRes.sum >= leftRes.sum && rightRes.sum >= crossRes.sum) {
        return rightRes;
    }
    return crossRes;
}

} // anonymous namespace

// WHAT:    Finds peak visitor reading surge window using Divide and Conquer.
// IDEA:    Starts timer, handles empty input edge case, invokes maxSubarrayRecursive, and stops timer.
// EXAMPLE: turnstile deltas [-2, 1, -3, 4, -1, 2, 1, -5, 4] -> sum 6 in window [3..6]
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(log n) recursion stack
// VIVA:    Empty array check prevents undefined behavior and returns {-1, -1, 0}.
SubarrayResult maxSubarrayDivideAndConquer(const std::vector<int>& arr, Metrics& metrics) {
    if (arr.empty()) {
        return {-1, -1, 0};
    }

    metrics.startTimer();
    SubarrayResult res = maxSubarrayRecursive(arr, 0, static_cast<int>(arr.size()) - 1, metrics);
    metrics.stopTimer();
    return res;
}

// WHAT:    Kadane's linear-time dynamic programming algorithm for maximum subarray.
// IDEA:    Maintains current running sum; if currentSum + arr[i] < arr[i], starts a fresh window at i.
//          Updates overall maximum sum whenever current running sum exceeds maxSoFar.
// EXAMPLE: [-2, 1, -3, 4, -1, 2, 1, -5, 4] -> sum 6 in [3..6]
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(1) auxiliary space
// VIVA:    Runs in O(n) time, serving as an independent oracle to verify Divide-and-Conquer.
SubarrayResult maxSubarrayKadane(const std::vector<int>& arr, Metrics& metrics) {
    if (arr.empty()) return {-1, -1, 0};

    metrics.startTimer();
    int64_t maxSoFar = arr[0];
    int64_t currentMax = arr[0];
    int start = 0, end = 0, s = 0;

    for (size_t i = 1; i < arr.size(); ++i) {
        metrics.comparisons++;
        if (arr[i] > currentMax + arr[i]) {
            currentMax = arr[i];
            s = static_cast<int>(i);
        } else {
            currentMax += arr[i];
        }

        metrics.comparisons++;
        if (currentMax > maxSoFar) {
            maxSoFar = currentMax;
            start = s;
            end = static_cast<int>(i);
        }
    }

    metrics.stopTimer();
    return {start, end, maxSoFar};
}

// ============================================================================
// 2. FAST POWERING
// ============================================================================

// WHAT:    Computes (base^exp) mod m using binary exponentiation.
// IDEA:    Inspects lowest bit of exp: if 1, multiplies base into result.
//          Squares base on every step and right-shifts exp by 1 (exp /= 2).
// EXAMPLE: fastPowerModular(2, 10, 1000, metrics) -> 24 (1024 % 1000)
// TIME:    best O(1) / avg O(log exp) / worst O(log exp)      SPACE: O(1)
// VIVA:    Using __int128_t intermediate casting prevents 64-bit overflow during modular multiplication.
int64_t fastPowerModular(int64_t base, int64_t exp, int64_t mod, Metrics& metrics) {
    if (mod <= 0) return 0; // Invalid modulus
    if (exp < 0) return 0;   // Negative exponent not supported directly

    metrics.startTimer();
    int64_t result = 1;
    base = base % mod;

    // Binary exponentiation loop
    while (exp > 0) {
        metrics.comparisons++; // Track loop iteration condition check
        if (exp & 1) {
            // Current lowest bit is 1: multiply base into accumulator
            result = (__int128_t)result * base % mod;
        }
        // Square the base for next power of two
        base = (__int128_t)base * base % mod;
        exp >>= 1; // Divide exponent by 2
    }

    metrics.stopTimer();
    return result;
}

// WHAT:    Computes base^exp in integer arithmetic without modulo.
// IDEA:    Repeatedly squares base and multiplies into result whenever lowest bit of exp is 1.
// EXAMPLE: fastPower(2, 8, metrics) -> 256
// TIME:    best O(1) / avg O(log exp) / worst O(log exp)      SPACE: O(1)
// VIVA:    Takes only log2(exp) multiplications instead of exp multiplications.
int64_t fastPower(int64_t base, int64_t exp, Metrics& metrics) {
    if (exp < 0) return 0;

    metrics.startTimer();
    int64_t result = 1;

    while (exp > 0) {
        metrics.comparisons++;
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }

    metrics.stopTimer();
    return result;
}

// ============================================================================
// 3. CELEBRITY DISCOVERY
// ============================================================================

// WHAT:    Discovers an authoritative paper in an n x n citation matrix in O(n) queries.
// IDEA:    Compares candidate C against each paper i: if C cites i, C is eliminated (C = i).
//          If C doesn't cite i, i is eliminated. Verifies surviving candidate in 2(n-1) checks.
// EXAMPLE: Paper 7 cited by all 19 peers and citing no one -> returns 7 in 57 queries
// TIME:    best O(n) / avg O(n) / worst O(n) queries      SPACE: O(1)
// VIVA:    Elimination reduces candidates from n to 1 in n-1 queries; verification confirms both conditions.
int findCelebrity(const std::vector<std::vector<int>>& mat, Metrics& metrics) {
    int n = static_cast<int>(mat.size());
    if (n == 0) return -1;
    if (n == 1) return 0;

    metrics.startTimer();

    // Step 1: Elimination phase to identify sole possible candidate
    int candidate = 0;
    for (int i = 1; i < n; ++i) {
        metrics.comparisons++;
        // Does candidate cite paper i?
        if (mat[candidate][i] == 1) {
            // candidate cites i, so candidate cannot be celebrity. i might be.
            candidate = i;
        } else {
            // candidate does not cite i, so i cannot be celebrity (everyone must cite celebrity).
            // candidate remains.
        }
    }

    // Step 2: Verification phase
    for (int i = 0; i < n; ++i) {
        if (i == candidate) continue;

        metrics.comparisons++;
        // Condition A: Candidate must cite nobody
        if (mat[candidate][i] != 0) {
            metrics.stopTimer();
            return -1; // Failed: candidate cited paper i
        }

        metrics.comparisons++;
        // Condition B: Every paper must cite candidate
        if (mat[i][candidate] != 1) {
            metrics.stopTimer();
            return -1; // Failed: paper i did not cite candidate
        }
    }

    metrics.stopTimer();
    return candidate; // Verified celebrity
}

// ============================================================================
// 4. MATRIX MULTIPLICATION: CLASSICAL VS STRASSEN
// ============================================================================

namespace {

// WHAT:    Adds two square matrices element-wise: C = A + B.
// IDEA:    Iterates over all rows and columns and sets C[i][j] = A[i][j] + B[i][j].
// EXAMPLE: [[1, 2], [3, 4]] + [[5, 6], [7, 8]] -> [[6, 8], [10, 12]]
// TIME:    best O(n^2) / avg O(n^2) / worst O(n^2)      SPACE: O(n^2) for result matrix
// VIVA:    Helper used in Strassen's submatrix combination steps.
Matrix addMatrix(const Matrix& A, const Matrix& B) {
    size_t n = A.size();
    Matrix C(n, std::vector<int64_t>(n, 0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

// WHAT:    Subtracts two square matrices element-wise: C = A - B.
// IDEA:    Iterates over all rows and columns and sets C[i][j] = A[i][j] - B[i][j].
// EXAMPLE: [[5, 6], [7, 8]] - [[1, 2], [3, 4]] -> [[4, 4], [4, 4]]
// TIME:    best O(n^2) / avg O(n^2) / worst O(n^2)      SPACE: O(n^2) for result matrix
// VIVA:    Helper used in Strassen's 7 sub-product formulations.
Matrix subtractMatrix(const Matrix& A, const Matrix& B) {
    size_t n = A.size();
    Matrix C(n, std::vector<int64_t>(n, 0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
    return C;
}

// WHAT:    Recursive worker for Strassen's matrix multiplication.
// IDEA:    If size <= threshold, multiplies classically. Otherwise partitions A and B into
//          4 quadrants, computes 7 recursive products M1..M7, and combines into C11..C22.
// EXAMPLE: Multiplies two 32x32 matrices using 7 products of size 16x16
// TIME:    best O(n^2.807) / avg O(n^2.807) / worst O(n^2.807)      SPACE: O(n^2) auxiliary buffers
// VIVA:    Computing 7 sub-products instead of 8 breaks the O(n^3) barrier.
Matrix strassenRecursive(const Matrix& A, const Matrix& B, size_t threshold, Metrics& metrics) {
    size_t n = A.size();

    // Base case: switch to classical multiplication when matrix size <= threshold
    if (n <= threshold) {
        Matrix C(n, std::vector<int64_t>(n, 0));
        for (size_t i = 0; i < n; ++i) {
            for (size_t k = 0; k < n; ++k) {
                for (size_t j = 0; j < n; ++j) {
                    metrics.comparisons++; // arithmetic step count
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        return C;
    }

    size_t k = n / 2;

    // Step 1: Partition A and B into 4 submatrices of size k x k
    Matrix A11(k, std::vector<int64_t>(k)), A12(k, std::vector<int64_t>(k));
    Matrix A21(k, std::vector<int64_t>(k)), A22(k, std::vector<int64_t>(k));
    Matrix B11(k, std::vector<int64_t>(k)), B12(k, std::vector<int64_t>(k));
    Matrix B21(k, std::vector<int64_t>(k)), B22(k, std::vector<int64_t>(k));

    for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < k; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + k];
            A21[i][j] = A[i + k][j];
            A22[i][j] = A[i + k][j + k];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + k];
            B21[i][j] = B[i + k][j];
            B22[i][j] = B[i + k][j + k];
        }
    }

    // Step 2: Compute 7 Strassen products recursively
    // M1 = (A11 + A22) * (B11 + B22)
    Matrix M1 = strassenRecursive(addMatrix(A11, A22), addMatrix(B11, B22), threshold, metrics);
    // M2 = (A21 + A22) * B11
    Matrix M2 = strassenRecursive(addMatrix(A21, A22), B11, threshold, metrics);
    // M3 = A11 * (B12 - B22)
    Matrix M3 = strassenRecursive(A11, subtractMatrix(B12, B22), threshold, metrics);
    // M4 = A22 * (B21 - B11)
    Matrix M4 = strassenRecursive(A22, subtractMatrix(B21, B11), threshold, metrics);
    // M5 = (A11 + A12) * B22
    Matrix M5 = strassenRecursive(addMatrix(A11, A12), B22, threshold, metrics);
    // M6 = (A21 - A11) * (B11 + B12)
    Matrix M6 = strassenRecursive(subtractMatrix(A21, A11), addMatrix(B11, B12), threshold, metrics);
    // M7 = (A12 - A22) * (B21 + B22)
    Matrix M7 = strassenRecursive(subtractMatrix(A12, A22), addMatrix(B21, B22), threshold, metrics);

    // Step 3: Combine sub-products into result submatrices
    // C11 = M1 + M4 - M5 + M7
    Matrix C11 = addMatrix(subtractMatrix(addMatrix(M1, M4), M5), M7);
    // C12 = M3 + M5
    Matrix C12 = addMatrix(M3, M5);
    // C21 = M2 + M4
    Matrix C21 = addMatrix(M2, M4);
    // C22 = M1 - M2 + M3 + M6
    Matrix C22 = addMatrix(addMatrix(subtractMatrix(M1, M2), M3), M6);

    // Step 4: Reassemble final matrix C
    Matrix C(n, std::vector<int64_t>(n, 0));
    for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < k; ++j) {
            C[i][j] = C11[i][j];
            C[i][j + k] = C12[i][j];
            C[i + k][j] = C21[i][j];
            C[i + k][j + k] = C22[i][j];
        }
    }
    return C;
}

// WHAT:    Finds the smallest power of 2 greater than or equal to n.
// IDEA:    Repeatedly left-shifts p until p >= n.
// EXAMPLE: nextPowerOf2(5) -> 8
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1)
// VIVA:    Used to zero-pad non-power-of-2 matrices for Strassen partitioning.
size_t nextPowerOf2(size_t n) {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

} // anonymous namespace

// WHAT:    Multiplies two matrices using the classical triple nested loop algorithm.
// IDEA:    Loops i from 0..rows, k from 0..inner, j from 0..cols, accumulating C[i][j] += A[i][k] * B[k][j].
// EXAMPLE: Multiplies (2x3) by (3x2) matrix -> produces (2x2) result
// TIME:    best O(n^3) / avg O(n^3) / worst O(n^3)      SPACE: O(n^2) for product matrix
// VIVA:    The inner loop order (i, k, j) maximizes cache hit rates on row-major matrix memory.
Matrix multiplyMatrixClassical(const Matrix& A, const Matrix& B, Metrics& metrics) {
    if (A.empty() || B.empty() || A[0].size() != B.size()) {
        return {};
    }

    metrics.startTimer();
    size_t rows = A.size();
    size_t cols = B[0].size();
    size_t inner = B.size();

    Matrix C(rows, std::vector<int64_t>(cols, 0));

    // Standard triple nested loop
    for (size_t i = 0; i < rows; ++i) {
        for (size_t k = 0; k < inner; ++k) {
            int64_t aik = A[i][k];
            for (size_t j = 0; j < cols; ++j) {
                metrics.comparisons++; // Count arithmetic operation
                C[i][j] += aik * B[k][j];
            }
        }
    }

    metrics.stopTimer();
    return C;
}

// WHAT:    Multiplies two square matrices using Strassen's algorithm with zero-padding and threshold fallback.
// IDEA:    Pads matrices to nearest power of 2, calls recursive Strassen, and unpads result back to original size.
// EXAMPLE: Multiplies citation matrices in O(n^2.807) time instead of O(n^3)
// TIME:    best O(n^2.807) / avg O(n^2.807) / worst O(n^2.807)      SPACE: O(n^2)
// VIVA:    Below threshold (default 16), falls back to classical multiplication to avoid submatrix recursion overhead.
Matrix multiplyMatrixStrassen(const Matrix& A, const Matrix& B, Metrics& metrics, size_t threshold) {
    if (A.empty() || B.empty() || A[0].size() != B.size()) {
        return {};
    }

    metrics.startTimer();

    size_t origRows = A.size();
    size_t origCols = B[0].size();
    size_t origInner = B.size();

    // Find maximum dimension and pad to nearest power of 2
    size_t maxDim = origRows;
    if (origCols > maxDim) maxDim = origCols;
    if (origInner > maxDim) maxDim = origInner;
    size_t m = nextPowerOf2(maxDim);

    Matrix paddedA(m, std::vector<int64_t>(m, 0));
    Matrix paddedB(m, std::vector<int64_t>(m, 0));

    for (size_t i = 0; i < origRows; ++i) {
        for (size_t j = 0; j < origInner; ++j) {
            paddedA[i][j] = A[i][j];
        }
    }
    for (size_t i = 0; i < origInner; ++i) {
        for (size_t j = 0; j < origCols; ++j) {
            paddedB[i][j] = B[i][j];
        }
    }

    Matrix paddedC = strassenRecursive(paddedA, paddedB, threshold, metrics);

    // Unpad result back to original dimensions
    Matrix C(origRows, std::vector<int64_t>(origCols, 0));
    for (size_t i = 0; i < origRows; ++i) {
        for (size_t j = 0; j < origCols; ++j) {
            C[i][j] = paddedC[i][j];
        }
    }

    metrics.stopTimer();
    return C;
}

} // namespace dls
