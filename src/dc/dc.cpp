#include "dls/dc.hpp"
#include <climits>
#include <stdexcept>
#include <cmath>

namespace dls {

// ============================================================================
// 1. MAXIMUM SUBARRAY
// ============================================================================

namespace {

/**
 * Finds maximum subarray that crosses the midpoint mid:
 * [left..mid] and [mid+1..right].
 */
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

/**
 * Recursive divide and conquer worker for Maximum Subarray.
 */
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

/**
 * @brief Maximum Subarray using Divide and Conquer.
 * 
 * IDEA:
 * Split array into left and right halves. The maximal contiguous subarray either
 * resides wholly in the left half, wholly in the right half, or crosses the midpoint.
 * 
 * TIME COMPLEXITY:
 * Best/Avg/Worst: O(n log n).
 * 
 * SPACE COMPLEXITY:
 * O(log n) call stack frames.
 * 
 * WHEN IT FAILS:
 * Empty array input (handled safely by returning { -1, -1, 0 }).
 */
SubarrayResult maxSubarrayDivideAndConquer(const std::vector<int>& arr, Metrics& metrics) {
    if (arr.empty()) {
        return {-1, -1, 0};
    }

    metrics.startTimer();
    SubarrayResult res = maxSubarrayRecursive(arr, 0, static_cast<int>(arr.size()) - 1, metrics);
    metrics.stopTimer();
    return res;
}

/**
 * @brief Kadane's algorithm (linear O(n) oracle).
 */
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

/**
 * @brief Modular fast powering via binary exponentiation.
 * 
 * IDEA:
 * Decomposes power into sum of powers of 2. In each step, if bit is 1, multiply into result;
 * square base each time: b^(2k) = (b^2)^k.
 * 
 * TIME COMPLEXITY:
 * O(log exp).
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space.
 * 
 * WHEN IT FAILS:
 * Modulus <= 0 (invalid), or exp < 0 (requires modular inverse).
 */
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

/**
 * @brief Integer fast powering.
 */
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

/**
 * @brief Celebrity discovery in citation matrix.
 * 
 * IDEA:
 * Pairwise elimination using divide and conquer / two pointers:
 * Let candidate C = 0. Iterate i = 1 to n-1:
 * If C cites i, C cannot be celebrity, so set C = i.
 * If C does not cite i, i cannot be celebrity, so C remains.
 * Survivor C is verified against all papers in O(n) checks.
 * 
 * TIME COMPLEXITY:
 * O(n) total checks (n-1 elimination queries + 2(n-1) verification queries).
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space.
 * 
 * WHEN IT FAILS:
 * Returns -1 if no paper meets both conditions (all cite it, it cites none).
 */
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

// Internal recursive Strassen worker
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

// Next power of 2 helper for matrix padding
size_t nextPowerOf2(size_t n) {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

} // anonymous namespace

/**
 * @brief Classical O(n^3) Matrix Multiplication.
 */
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

/**
 * @brief Strassen's Matrix Multiplication with auto-padding and threshold fallback.
 */
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
