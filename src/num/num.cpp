#include "dls/num.hpp"

namespace dls {

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
