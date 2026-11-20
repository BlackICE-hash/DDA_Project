// NOTE: This file implements 4 String Matching Algorithms:
// 1. Naive: Checks every text position by comparing character-by-character.
// 2. Rabin-Karp: Compares hash values of pattern and rolling text window, verifying on collisions.
// 3. Finite Automaton: Precomputes a state transition table (DFA) based on pattern prefixes.
// 4. Knuth-Morris-Pratt (KMP): Uses a precomputed failure function (pi table) so the text
//    pointer never moves backward, achieving guaranteed linear O(n + m) time.

#include "dls/strmatch.hpp"

namespace dls {

// ============================================================================
// 1. NAIVE STRING MATCHING
// ============================================================================

// WHAT:    Finds all occurrences of pattern in text using brute-force sliding window
// IDEA:    Tries every possible starting shift s from 0 to n - m.
//          At each shift, compares characters until a mismatch or full match occurs.
// EXAMPLE: text = "AABAACAADAABAABA", pattern = "AABA" -> matches at [0, 9, 12]
// TIME:    Best: O(n) when first chars differ; Worst: O((n - m + 1) * m)   SPACE: O(1) auxiliary
// VIVA:    Examiner asks: "What is the worst-case input for Naive matching?"
//          Answer: When text is "AAAAAA" and pattern is "AAAAB", every shift compares m-1
//          matching characters before failing on the last one.
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

// WHAT:    Finds pattern occurrences by comparing rolling hash values
// IDEA:    Computes hash of pattern and initial text window of size m.
//          Slides window by subtracting old leading character and adding new trailing character
//          in O(1) time. When hashes match, verifies characters to eliminate false positives (spurious hits).
// EXAMPLE: text = "GEEKS FOR GEEKS", pattern = "GEEK" -> matches at [0, 10]
// TIME:    Average: O(n + m); Worst: O(n * m) if all hashes collide   SPACE: O(1) auxiliary
// VIVA:    Examiner asks: "Why do we verify character-by-character when hashes match (p == t)?"
//          Answer: Because different strings can produce the same hash modulo q (hash collision).
//          Character verification ensures correctness.
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

// WHAT:    Helper checking whether pattern[0..k-1] matches the last k characters of target
// IDEA:    Compares character by character from offset target.size() - k to the end.
// EXAMPLE: pattern = "ABAB", k = 2 ("AB"), target = "AAAB" -> returns true ("AB" matches suffix)
// TIME:    O(k)                     SPACE: O(1) auxiliary
// VIVA:    Examiner asks: "What does this helper represent in automaton construction?"
//          Answer: It checks if a prefix of length k can be completed by the target string suffix,
//          determining the next valid automaton state.
bool isSuffix(const std::string& pattern, size_t k, const std::string& target) {
    if (k > target.size()) return false;
    size_t offset = target.size() - k;
    for (size_t i = 0; i < k; ++i) {
        if (pattern[i] != target[offset + i]) return false;
    }
    return true;
}

// WHAT:    Computes transition function delta[0..m][0..255] for pattern DFA
// IDEA:    For each state q (matched characters so far) and input char c, constructs
//          target = pattern[0..q-1] + c. Finds largest k such that pattern[0..k-1]
//          is a suffix of target.
// EXAMPLE: pattern = "AB", delta[1]['B'] = 2 (accepting state)
// TIME:    O(m^3 * |Sigma|)         SPACE: O(m * |Sigma|) table
// VIVA:    Examiner asks: "Why is FA text matching fast even though table construction is O(m^3 * |Sigma|)?"
//          Answer: Once the table is built, scanning the text requires exactly 1 table lookup per text character,
//          running in strict O(n) time with zero character backtracking.
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

// WHAT:    Matches pattern in text using a Deterministic Finite Automaton (DFA)
// IDEA:    Precomputes state transition table delta. Starts at state q=0.
//          For each character in text, transitions to delta[q][text[i]].
//          Whenever state q == m is reached, a full occurrence has been found.
// EXAMPLE: text = "AABA", pattern = "BA" -> transitions 0 -> 0 -> 0 -> 1 -> 2 (match at index 2)
// TIME:    O(n) text scanning after O(m^3 * |Sigma|) preprocessing   SPACE: O(m * |Sigma|)
// VIVA:    Examiner asks: "When is Finite Automaton matching preferred over KMP?"
//          Answer: When searching for the SAME pattern across many large texts or continuous streams,
//          because the DFA transition is just a single 2D array lookup with no while loops.
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
        q = delta[q][c]; // State transition: O(1) table lookup
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

// WHAT:    Computes KMP prefix (failure / border) function table pi
// IDEA:    For each prefix pattern[0..q], computes pi[q], which is the length of the longest
//          proper prefix of pattern[0..q] that is also a suffix of pattern[0..q].
//          When a mismatch occurs at pattern[q], pi[q-1] tells us the length of the longest
//          matching prefix we already have aligned, avoiding redundant character re-checks.
// EXAMPLE: pattern = "ABAB" -> pi = [0, 0, 1, 2] ("A":0, "AB":0, "ABA":1 ["A"], "ABAB":2 ["AB"])
// TIME:    O(m)                     SPACE: O(m) array for pi table
// VIVA:    Examiner asks: "Why does the loop start at q = 1 instead of q = 0?"
//          Answer: A proper prefix cannot be the entire string. For q = 0, pattern[0..0] has length 1,
//          so its only proper prefix is empty (length 0). Thus pi[0] is always 0.
std::vector<int> computeKMPPrefixTable(const std::string& pattern, Metrics& metrics) {
    size_t m = pattern.size();
    std::vector<int> pi(m, 0); // pi[0] is always 0: single char has no non-empty proper prefix
    int k = 0; // Length of current longest proper prefix which is also suffix

    // Loop through pattern characters from index 1 to m-1
    for (size_t q = 1; q < m; ++q) {
        // While mismatch and we can fall back along previous prefix chain
        while (k > 0 && pattern[k] != pattern[q]) {
            metrics.comparisons++;
            k = pi[k - 1]; // Fall back: use longest border of shorter prefix
        }
        metrics.comparisons++;
        // If characters match, we extend current matching prefix length by 1
        if (pattern[k] == pattern[q]) {
            k++;
        }
        // Store length of longest proper prefix matching suffix for pattern[0..q]
        pi[q] = k;
    }
    return pi;
}

// WHAT:    Finds all occurrences of pattern in text in linear O(n + m) time using KMP algorithm
// IDEA:    Uses the precomputed prefix table pi to shift pattern intelligently.
//          Text pointer i moves strictly forward from 0 to n-1 (never backtracks).
//          When mismatch occurs with pattern[q], the prefix table gives the next state q = pi[q - 1].
// EXAMPLE: text = "ABABDABAB", pattern = "ABAB" -> finds matches at indices [0, 5]
// TIME:    O(n + m) guaranteed      SPACE: O(m) for prefix table pi
// VIVA:    Examiner asks: "Why does KMP never move the text pointer i backwards?"
//          Answer: The prefix table pi tells us what part of the pattern already matches the text
//          up to text[i-1], so we can shift the pattern forward and continue from text[i].
std::vector<size_t> matchKMP(const std::string& text, const std::string& pattern, Metrics& metrics) {
    // Guard: empty pattern or text shorter than pattern cannot match
    if (pattern.empty() || text.size() < pattern.size()) {
        return {};
    }

    metrics.startTimer();
    std::vector<size_t> matches;
    size_t n = text.size();
    size_t m = pattern.size();

    // Step 1: Precompute prefix table pi in O(m) time
    std::vector<int> pi = computeKMPPrefixTable(pattern, metrics);

    // Step 2: Linear scan of text - text pointer i never decreases!
    int q = 0; // Number of characters matched so far
    for (size_t i = 0; i < n; ++i) {
        // Mismatch occurred: fall back pattern pointer q using failure function
        while (q > 0 && pattern[q] != text[i]) {
            metrics.comparisons++;
            q = pi[q - 1]; // Shift pattern right without moving text pointer i
        }

        metrics.comparisons++;
        // If characters match, increment count of matched characters
        if (pattern[q] == text[i]) {
            q++;
        }

        // Check if full pattern has been matched
        if (static_cast<size_t>(q) == m) {
            matches.push_back(i - m + 1); // Record 0-based start index of match
            q = pi[q - 1]; // Look for next potentially overlapping occurrence
        }
    }

    metrics.stopTimer();
    return matches;
}

} // namespace dls
