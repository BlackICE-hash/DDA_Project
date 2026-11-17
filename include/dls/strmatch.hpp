#pragma once

#include "dls/types.hpp"
#include <vector>
#include <string>

namespace dls {

/**
 * @brief Naive String Matching.
 * 
 * IDEA:
 * Tests every possible shift s in [0..n-m] by character-by-character comparison.
 * 
 * TIME COMPLEXITY:
 * Best: O(n), Worst: O((n - m + 1) * m).
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space.
 * 
 * WHEN IT FAILS:
 * Degrades on adversarial repetitive texts (e.g. "aaaa...ab").
 */
std::vector<size_t> matchNaive(const std::string& text, const std::string& pattern, Metrics& metrics);

/**
 * @brief Rabin-Karp String Matching via Rolling Hash.
 * 
 * IDEA:
 * Compares hash values of pattern and sliding window in O(1) time per shift using
 * polynomial rolling hash. Verifies characters on hash match.
 * 
 * TIME COMPLEXITY:
 * Average: O(n + m), Worst: O(n * m) on frequent hash collisions.
 * 
 * SPACE COMPLEXITY:
 * O(1) auxiliary space.
 * 
 * WHEN IT FAILS:
 * Spurious collisions if modulus is poorly chosen or small.
 */
std::vector<size_t> matchRabinKarp(const std::string& text, const std::string& pattern, Metrics& metrics);

/**
 * @brief Finite Automaton String Matching.
 * 
 * IDEA:
 * Constructs a DFA state transition table where states correspond to matched prefix lengths.
 * Processes each character of text in strict O(1) time.
 * 
 * TIME COMPLEXITY:
 * Preprocessing: O(m^3 * |Sigma|) or O(m * |Sigma|).
 * Matching: O(n) strictly linear.
 * 
 * SPACE COMPLEXITY:
 * O(m * |Sigma|) state table.
 * 
 * WHEN IT FAILS:
 * Large alphabet sizes |Sigma| consume high memory for transition table.
 */
std::vector<size_t> matchFiniteAutomaton(const std::string& text, const std::string& pattern, Metrics& metrics);

/**
 * @brief Knuth-Morris-Pratt (KMP) String Matching.
 * 
 * IDEA:
 * Precomputes prefix function pi[0..m-1]. On mismatch, skips shifts by jumping to
 * pi[q-1] avoiding redundant text comparisons.
 * 
 * TIME COMPLEXITY:
 * O(n + m) worst case.
 * 
 * SPACE COMPLEXITY:
 * O(m) for prefix table.
 * 
 * WHEN IT FAILS:
 * Handles all inputs in guaranteed O(n + m) time.
 */
std::vector<size_t> matchKMP(const std::string& text, const std::string& pattern, Metrics& metrics);

// Exposed helper for testing prefix table
std::vector<int> computeKMPPrefixTable(const std::string& pattern, Metrics& metrics);

} // namespace dls
