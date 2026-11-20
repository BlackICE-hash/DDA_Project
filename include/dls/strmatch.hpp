#pragma once

#include "dls/types.hpp"
#include <vector>
#include <string>

// NOTE: "std::vector<size_t>" is a standard vector holding the 0-based character indices
// where the search keyword was found inside the manuscript text.

namespace dls {

// WHAT:    Searches for pattern in text using the baseline Naive brute-force algorithm.
// IDEA:    Tries every possible alignment index s from 0 to n-m, comparing characters one by one.
//          Backtracks text pointer to s+1 on every mismatch.
// EXAMPLE: matchNaive("ABABDABAB", "ABAB", metrics) -> indices [0, 5]
// TIME:    best O(n) / avg O(n) / worst O(n * m)      SPACE: O(1)
// VIVA:    Worst-case O(n * m) triggered by adversarial text like "aaaa...b" and pattern "aaab".
std::vector<size_t> matchNaive(const std::string& text, const std::string& pattern, Metrics& metrics);

// WHAT:    Searches for pattern in text using Rabin-Karp polynomial rolling hash matching.
// IDEA:    Computes hash of pattern and current text window. Advances window in O(1) using rolling hash;
//          compares full strings only when hashes match to confirm true match.
// EXAMPLE: matchRabinKarp("ABABDABAB", "ABAB", metrics) -> indices [0, 5]
// TIME:    best O(n + m) / avg O(n + m) / worst O(n * m) on collisions      SPACE: O(1)
// VIVA:    Modulo arithmetic (using large prime like 1e9+7) prevents hash value overflow.
std::vector<size_t> matchRabinKarp(const std::string& text, const std::string& pattern, Metrics& metrics);

// WHAT:    Searches for pattern in text using a Deterministic Finite Automaton (DFA).
// IDEA:    Builds state transition table delta[state][char] where states represent matched prefix length.
//          Processes each text character in exactly one state transition.
// EXAMPLE: matchFiniteAutomaton("ABABDABAB", "ABAB", metrics) -> indices [0, 5]
// TIME:    best O(n) / avg O(n) / worst O(n) after O(m * |Sigma|) preprocessing      SPACE: O(m * |Sigma|)
// VIVA:    Text scan is guaranteed strictly O(n) with 0 backtracks, but building table takes O(m * |Sigma|).
std::vector<size_t> matchFiniteAutomaton(const std::string& text, const std::string& pattern, Metrics& metrics);

// WHAT:    Searches for pattern in text using the Knuth-Morris-Pratt (KMP) linear-time algorithm.
// IDEA:    Precomputes pi prefix table in O(m). Advances text pointer monotonically; on mismatch,
//          slides pattern using pi table without ever backtracking text pointer.
// EXAMPLE: matchKMP("ABABDABAB", "ABAB", metrics) -> indices [0, 5]
// TIME:    best O(n) / avg O(n + m) / worst O(n + m) (guaranteed linear)      SPACE: O(m) prefix table
// VIVA:    Text pointer never backtracks, ensuring linear time even on adversarial repetitive inputs.
std::vector<size_t> matchKMP(const std::string& text, const std::string& pattern, Metrics& metrics);

// WHAT:    Precomputes the KMP prefix failure table (pi) for a given pattern.
// IDEA:    pi[q] stores the length of the longest proper prefix of pattern[0..q] that is also a suffix.
// EXAMPLE: computeKMPPrefixTable("ABAB", metrics) -> [0, 0, 1, 2]
// TIME:    best O(m) / avg O(m) / worst O(m)      SPACE: O(m)
// VIVA:    Used by matchKMP() to determine how many characters can be safely skipped after a mismatch.
std::vector<int> computeKMPPrefixTable(const std::string& pattern, Metrics& metrics);

} // namespace dls
