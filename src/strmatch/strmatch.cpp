#include "dls/strmatch.hpp"

namespace dls {

// ============================================================================
// 1. NAIVE STRING MATCHING
// ============================================================================

std::vector<size_t> matchNaive(const std::string& text, const std::string& pattern, Metrics& metrics) {
    if (pattern.empty() || text.size() < pattern.size()) {
        return {};
    }

    metrics.startTimer();
    std::vector<size_t> matches;
    size_t n = text.size();
    size_t m = pattern.size();

    // Slide pattern across text
    for (size_t s = 0; s <= n - m; ++s) {
        bool match = true;
        for (size_t j = 0; j < m; ++j) {
            metrics.comparisons++;
            if (text[s + j] != pattern[j]) {
                match = false;
                break; // Mismatch, advance shift
            }
        }
        if (match) {
            matches.push_back(s);
        }
    }

    metrics.stopTimer();
    return matches;
}

// ============================================================================
// 2. RABIN-KARP STRING MATCHING
// ============================================================================

std::vector<size_t> matchRabinKarp(const std::string& text, const std::string& pattern, Metrics& metrics) {
    if (pattern.empty() || text.size() < pattern.size()) {
        return {};
    }

    metrics.startTimer();
    std::vector<size_t> matches;
    size_t n = text.size();
    size_t m = pattern.size();

    const int64_t d = 256;          // Alphabet radix
    const int64_t q = 1000000007;   // Large prime modulus

    // Precompute h = d^(m-1) mod q
    int64_t h = 1;
    for (size_t i = 0; i < m - 1; ++i) {
        h = (h * d) % q;
    }

    // Step 1: Precompute initial hash of pattern and first text window
    int64_t p = 0; // Pattern hash
    int64_t t = 0; // Text window hash

    for (size_t i = 0; i < m; ++i) {
        p = (d * p + static_cast<unsigned char>(pattern[i])) % q;
        t = (d * t + static_cast<unsigned char>(text[i])) % q;
    }

    // Step 2: Slide window over text
    for (size_t s = 0; s <= n - m; ++s) {
        metrics.comparisons++;
        // Check hash match
        if (p == t) {
            // Verify character by character to rule out hash collision
            bool match = true;
            for (size_t j = 0; j < m; ++j) {
                metrics.comparisons++;
                if (text[s + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                matches.push_back(s);
            }
        }

        // Compute rolling hash for next window: t_{s+1}
        if (s < n - m) {
            int64_t leadingChar = (static_cast<unsigned char>(text[s]) * h) % q;
            t = (d * (t - leadingChar + q) + static_cast<unsigned char>(text[s + m])) % q;
        }
    }

    metrics.stopTimer();
    return matches;
}

// ============================================================================
// 3. FINITE AUTOMATON STRING MATCHING
// ============================================================================

namespace {

// Check if pattern[0..k-1] is a suffix of target string
bool isSuffix(const std::string& pattern, size_t k, const std::string& target) {
    if (k > target.size()) return false;
    size_t offset = target.size() - k;
    for (size_t i = 0; i < k; ++i) {
        if (pattern[i] != target[offset + i]) return false;
    }
    return true;
}

// Compute transition function delta[0..m][0..255]
std::vector<std::vector<int>> computeTransitionTable(const std::string& pattern, Metrics& metrics) {
    size_t m = pattern.size();
    std::vector<std::vector<int>> delta(m + 1, std::vector<int>(256, 0));

    // For each state q (number of matched characters so far)
    for (size_t q = 0; q <= m; ++q) {
        for (int c = 0; c < 256; ++c) {
            // Target string is pattern prefix of length q followed by char c
            std::string target = pattern.substr(0, q);
            target.push_back(static_cast<char>(c));

            // Find largest k <= min(m, q + 1) such that pattern[0..k-1] is suffix of target
            size_t k = (m < q + 1) ? m : q + 1;
            while (k > 0) {
                metrics.comparisons++;
                if (isSuffix(pattern, k, target)) {
                    break;
                }
                k--;
            }
            delta[q][c] = static_cast<int>(k);
        }
    }
    return delta;
}

} // anonymous namespace

std::vector<size_t> matchFiniteAutomaton(const std::string& text, const std::string& pattern, Metrics& metrics) {
    if (pattern.empty() || text.size() < pattern.size()) {
        return {};
    }

    metrics.startTimer();

    // Step 1: Build DFA transition table
    auto delta = computeTransitionTable(pattern, metrics);

    // Step 2: Process text character by character
    std::vector<size_t> matches;
    size_t n = text.size();
    size_t m = pattern.size();
    int q = 0; // Initial start state

    for (size_t i = 0; i < n; ++i) {
        metrics.comparisons++;
        unsigned char c = static_cast<unsigned char>(text[i]);
        q = delta[q][c]; // State transition
        if (static_cast<size_t>(q) == m) {
            matches.push_back(i - m + 1); // Match found ending at index i
        }
    }

    metrics.stopTimer();
    return matches;
}

// ============================================================================
// 4. KMP (KNUTH-MORRIS-PRATT) STRING MATCHING
// ============================================================================

std::vector<int> computeKMPPrefixTable(const std::string& pattern, Metrics& metrics) {
    size_t m = pattern.size();
    std::vector<int> pi(m, 0);
    int k = 0; // Length of current longest proper prefix which is also suffix

    for (size_t q = 1; q < m; ++q) {
        while (k > 0 && pattern[k] != pattern[q]) {
            metrics.comparisons++;
            k = pi[k - 1]; // Fall back along prefix chain
        }
        metrics.comparisons++;
        if (pattern[k] == pattern[q]) {
            k++;
        }
        pi[q] = k;
    }
    return pi;
}

std::vector<size_t> matchKMP(const std::string& text, const std::string& pattern, Metrics& metrics) {
    if (pattern.empty() || text.size() < pattern.size()) {
        return {};
    }

    metrics.startTimer();
    std::vector<size_t> matches;
    size_t n = text.size();
    size_t m = pattern.size();

    // Step 1: Precompute prefix table pi
    std::vector<int> pi = computeKMPPrefixTable(pattern, metrics);

    // Step 2: Linear scan of text
    int q = 0; // Number of characters matched
    for (size_t i = 0; i < n; ++i) {
        while (q > 0 && pattern[q] != text[i]) {
            metrics.comparisons++;
            q = pi[q - 1]; // Shift pattern using failure table
        }

        metrics.comparisons++;
        if (pattern[q] == text[i]) {
            q++;
        }

        if (static_cast<size_t>(q) == m) {
            matches.push_back(i - m + 1); // Match found
            q = pi[q - 1]; // Look for next overlapping occurrence
        }
    }

    metrics.stopTimer();
    return matches;
}

} // namespace dls
