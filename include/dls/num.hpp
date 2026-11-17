#pragma once

#include "dls/types.hpp"
#include <vector>
#include <cstdint>

namespace dls {

struct ExtGCDResult {
    int64_t gcd{1};
    int64_t x{0};
    int64_t y{0};
};

/**
 * @brief Extended Euclidean Algorithm from scratch.
 * Computes gcd(a, b) and Bezout coefficients x, y such that a*x + b*y = gcd(a, b).
 */
ExtGCDResult extendedGCD(int64_t a, int64_t b, Metrics& metrics);

/**
 * @brief Modular Multiplicative Inverse: computes x such that (a * x) % m == 1.
 * Returns -1 if modular inverse does not exist (i.e. gcd(a, m) != 1).
 */
int64_t modInverse(int64_t a, int64_t m, Metrics& metrics);

struct CRTResult {
    bool hasSolution{false};
    int64_t solution{0};
    int64_t moduloProduct{0};
};

/**
 * @brief Chinese Remainder Theorem (CRT).
 * 
 * IDEA:
 * Solves system of simultaneous modular congruences:
 * x ≡ rem[i] (mod mod[i]) for all i.
 * Computes M = product(mod[i]), M_i = M / mod[i], y_i = modInverse(M_i, mod[i]),
 * x = sum(rem[i] * M_i * y_i) mod M.
 * 
 * TIME COMPLEXITY:
 * O(k * log(M)) where k is number of equations.
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space beyond inputs.
 * 
 * WHEN IT FAILS:
 * When moduli are not pairwise coprime or inputs are empty.
 */
CRTResult solveCRT(const std::vector<int64_t>& remainders,
                   const std::vector<int64_t>& moduli,
                   Metrics& metrics);

} // namespace dls
