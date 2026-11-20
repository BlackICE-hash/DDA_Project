// NOTE: This file implements Number-Theoretic Algorithms for catalog security and timestamps:
// 1. Extended Euclidean Algorithm: finds gcd(a, b) and Bezout coefficients x, y such that ax + by = gcd.
// 2. Modular Multiplicative Inverse: finds x such that (a * x) = 1 (mod m).
// 3. Chinese Remainder Theorem (CRT): solves systems of simultaneous modular congruences.

#include "dls/num.hpp"

namespace dls {

// WHAT:    Extended Euclidean algorithm computing gcd(a, b) and Bezout coefficients (x, y)
// IDEA:    Iteratively performs division algorithm maintaining running linear combinations:
//          a_orig * x + b_orig * y = current_remainder.
//          When remainder becomes 0, previous remainder is gcd(a, b).
// EXAMPLE: a = 35, b = 15 -> gcd = 5, with x = 1, y = -2: (35 * 1 + 15 * (-2) = 5)
// TIME:    O(log(min(a, b)))        SPACE: O(1) auxiliary
// VIVA:    Examiner asks: "When does Bezout's identity ax + by = 1 have integer solutions?"
//          Answer: When and only when gcd(a, b) = 1 (i.e., a and b are coprime).
ExtGCDResult extendedGCD(int64_t a, int64_t b, Metrics& metrics) {
    int64_t x0 = 1, y0 = 0;
    int64_t x1 = 0, y1 = 1;

    // Iterative Extended Euclidean algorithm
    while (b != 0) {
        metrics.comparisons++;
        int64_t q = a / b;
        int64_t r = a % b;

        a = b;
        b = r;

        int64_t nextX = x0 - q * x1;
        int64_t nextY = y0 - q * y1;

        x0 = x1;
        y0 = y1;
        x1 = nextX;
        y1 = nextY;
    }

    return {a, x0, y0};
}

// WHAT:    Computes modular multiplicative inverse of a modulo m
// IDEA:    Calls extendedGCD(a, m). If gcd is 1, then a * x + m * y = 1.
//          Modulo m, this means a * x = 1 (mod m).
//          Normalizes x to lie in [0, m - 1].
// EXAMPLE: a = 3, m = 11 -> inverse is 4, because (3 * 4) % 11 = 12 % 11 = 1
// TIME:    O(log(min(a, m)))        SPACE: O(1) auxiliary
// VIVA:    Examiner asks: "What does it mean if this function returns -1?"
//          Answer: It means gcd(a, m) != 1, so no modular inverse exists (a and m share a factor).
int64_t modInverse(int64_t a, int64_t m, Metrics& metrics) {
    ExtGCDResult res = extendedGCD(a, m, metrics);
    metrics.comparisons++;
    // Inverse exists iff gcd(a, m) == 1
    if (res.gcd != 1) {
        return -1; // No inverse
    }

    // Ensure positive result in range [0, m - 1]
    int64_t inv = (res.x % m + m) % m;
    return inv;
}

// WHAT:    Solves a system of simultaneous modular congruences using Chinese Remainder Theorem
// IDEA:    For system x = remainders[i] (mod moduli[i]):
//          1. Verifies that all moduli are pairwise coprime via extendedGCD.
//          2. Computes total modulus product M = product(moduli[i]).
//          3. For each i, sets M_i = M / moduli[i], finds y_i = (M_i)^(-1) mod moduli[i].
//          4. Constructs unique solution x = sum(remainders[i] * M_i * y_i) mod M.
// EXAMPLE: rem=[2, 3, 2], mod=[3, 5, 7] -> unique solution x = 23 (mod 105)
// TIME:    O(k^2 * log(M)) pairwise gcd + inverses   SPACE: O(1) beyond input
// VIVA:    Examiner asks: "Why do we use __int128_t inside this function?"
//          Answer: When multiplying large 64-bit moduli and remainders (e.g., M_i * remainders[i]),
//          the product can exceed 64-bit integer limits (2^63 - 1). __int128_t prevents arithmetic overflow.
CRTResult solveCRT(const std::vector<int64_t>& remainders,
                   const std::vector<int64_t>& moduli,
                   Metrics& metrics) {
    if (remainders.empty() || remainders.size() != moduli.size()) {
        return {false, 0, 0};
    }

    metrics.startTimer();
    size_t k = remainders.size();

    // Step 1: Check pairwise coprimality of moduli
    for (size_t i = 0; i < k; ++i) {
        for (size_t j = i + 1; j < k; ++j) {
            ExtGCDResult g = extendedGCD(moduli[i], moduli[j], metrics);
            metrics.comparisons++;
            if (g.gcd != 1) {
                metrics.stopTimer();
                return {false, 0, 0}; // Moduli are not pairwise coprime
            }
        }
    }

    // Step 2: Compute overall product M = product(m_i)
    __int128_t M = 1;
    for (size_t i = 0; i < k; ++i) {
        M *= moduli[i];
    }

    // Step 3: Compute sum_i (rem[i] * M_i * y_i)
    __int128_t totalX = 0;
    for (size_t i = 0; i < k; ++i) {
        int64_t m_i = moduli[i];
        __int128_t M_i = M / m_i;

        // Compute y_i = (M_i)^(-1) mod m_i
        int64_t M_i_mod = static_cast<int64_t>(M_i % m_i);
        int64_t y_i = modInverse(M_i_mod, m_i, metrics);
        if (y_i == -1) {
            metrics.stopTimer();
            return {false, 0, 0};
        }

        __int128_t term = (__int128_t)remainders[i] * M_i % M;
        term = term * y_i % M;
        totalX = (totalX + term) % M;
    }

    int64_t finalSolution = static_cast<int64_t>((totalX % M + M) % M);
    metrics.stopTimer();
    return {true, finalSolution, static_cast<int64_t>(M)};
}

} // namespace dls
