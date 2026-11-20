#pragma once

#include "dls/types.hpp"
#include <vector>
#include <cstdint>

// NOTE: "struct ExtGCDResult" and "struct CRTResult" are simple C++ return packages
// that hold gcd, Bezout coefficients x and y, and simultaneous congruence solutions.

namespace dls {

struct ExtGCDResult {
    int64_t gcd{1};
    int64_t x{0};
    int64_t y{0};
};

// WHAT:    Computes gcd(a, b) and Bezout coefficients x, y such that a*x + b*y = gcd(a, b).
// IDEA:    Recursive Euclidean division: when b == 0, returns (a, 1, 0).
//          Otherwise recurses on (b, a % b), updating x = y1 and y = x1 - (a / b) * y1.
// EXAMPLE: extendedGCD(30, 20, metrics) -> gcd = 10, x = 1, y = -1 (30*1 + 20*(-1) = 10)
// TIME:    best O(1) / avg O(log(min(a, b))) / worst O(log(min(a, b)))      SPACE: O(log(min(a, b))) recursion stack
// VIVA:    Worst-case inputs are consecutive Fibonacci numbers, taking the maximum recursive steps.
ExtGCDResult extendedGCD(int64_t a, int64_t b, Metrics& metrics);

// WHAT:    Computes the modular multiplicative inverse of a modulo m: (a * x) % m == 1.
// IDEA:    Runs extendedGCD(a, m). If gcd is 1, inverse is (x % m + m) % m; otherwise returns -1.
// EXAMPLE: modInverse(3, 11, metrics) -> returns 4 (since 3 * 4 = 12 ≡ 1 mod 11)
// TIME:    best O(1) / avg O(log m) / worst O(log m)      SPACE: O(log m) stack
// VIVA:    The inverse exists if and only if gcd(a, m) == 1 (a and m are coprime).
int64_t modInverse(int64_t a, int64_t m, Metrics& metrics);

struct CRTResult {
    bool hasSolution{false};
    int64_t solution{0};
    int64_t moduloProduct{0};
};

// WHAT:    Reconstructs distributed transaction sequence timestamps using the Chinese Remainder Theorem.
// IDEA:    Solves x ≡ r_i (mod m_i). Computes M = product of m_i, M_i = M / m_i, y_i = modInverse(M_i, m_i),
//          and x = sum(r_i * M_i * y_i) % M.
// EXAMPLE: x ≡ 2 mod 3, x ≡ 3 mod 5, x ≡ 2 mod 7 -> returns solution 23 (mod 105)
// TIME:    best O(k) / avg O(k log M) / worst O(k log M) where k is number of shards      SPACE: O(1)
// VIVA:    Requires all moduli to be pairwise coprime (gcd(m_i, m_j) == 1 for all i != j).
CRTResult solveCRT(const std::vector<int64_t>& remainders,
                   const std::vector<int64_t>& moduli,
                   Metrics& metrics);

} // namespace dls
