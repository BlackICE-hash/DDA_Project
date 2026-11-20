#include "dls/search.hpp"

// NOTE: This file implements concrete recursive and iterative binary search functions
// specialized for Document catalog lookups by unique book ID.

namespace dls {

// WHAT:    Recursive binary search on a sorted vector of Document records by ID.
// IDEA:    Checks midpoint document; if its ID matches target, returns mid.
//          Otherwise recurses on left half [lo..mid-1] or right half [mid+1..hi].
// EXAMPLE: binarySearchRecursive(docs, 250, 0, 499, metrics) -> returns index 249
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(log n) call stack frames
// VIVA:    The recurrence T(n) = T(n/2) + 1 solves to Theta(log n) by Master Theorem (Case 2: a=1, b=2).
int binarySearchRecursive(const std::vector<Document>& a, int target, int lo, int hi, Metrics& metrics) {
    // WHY: If lo crosses hi, the search range is empty; target ID does not exist in catalog.
    if (lo > hi) return -1;

    // WHY: Compute midpoint avoiding integer overflow: lo + (hi - lo) / 2 prevents overflow if lo+hi > 2^31 - 1.
    int mid = lo + (hi - lo) / 2;

    metrics.comparisons++; // WHY: Count equality check against target ID
    // WHY: Check if middle document's ID matches the requested target ID.
    if (a[mid].id == target) return mid; // Found match at index mid

    metrics.comparisons++; // WHY: Count ordering comparison against target ID
    // WHY: If target ID is smaller than middle document ID, target must reside in the left half [lo..mid-1].
    if (target < a[mid].id) {
        // WHY: mid - 1 because mid was already tested and did not match.
        return binarySearchRecursive(a, target, lo, mid - 1, metrics);
    } else {
        // WHY: mid + 1 because mid was already tested; search in right half [mid+1..hi].
        return binarySearchRecursive(a, target, mid + 1, hi, metrics);
    }
}

// WHAT:    Searches for a Document by ID in a catalog sorted by ID ascending, choosing iterative or recursive mode.
// IDEA:    Starts stopwatch, executes iterative loop or recursive helper based on useRecursive flag, and stops timer.
// EXAMPLE: binarySearchDocumentById(docs, 145, false, metrics) -> returns index of book ID 145
// TIME:    best O(1) / avg O(log n) / worst O(log n)      SPACE: O(1) iterative, O(log n) recursive
// VIVA:    Toggle 'useRecursive' true/false to demonstrate equivalence between loop and recursion.
int binarySearchDocumentById(const std::vector<Document>& docs, int targetId, bool useRecursive, Metrics& metrics) {
    // WHY: If catalog is empty, no document can be found; return -1 immediately.
    if (docs.empty()) return -1;

    metrics.startTimer(); // WHY: Start execution timer for telemetry

    if (!useRecursive) {
        // WHY: Iterative branch initializes low and high pointers to span the full vector.
        int low = 0;
        int high = static_cast<int>(docs.size()) - 1;

        // WHY: Loop while candidate search window [low..high] has at least one document.
        while (low <= high) {
            // WHY: Midpoint calculation safe against 32-bit signed overflow.
            int mid = low + (high - low) / 2;

            metrics.comparisons++; // WHY: Count equality check against target ID
            // WHY: If middle document has the requested ID, we found our target.
            if (docs[mid].id == targetId) {
                metrics.stopTimer(); // WHY: Stop stopwatch before returning
                return mid;          // Found document at index mid
            }

            metrics.comparisons++; // WHY: Count ordering comparison against target ID
            // WHY: If middle ID is smaller than targetId, search in right half.
            if (docs[mid].id < targetId) {
                // WHY: mid + 1 because mid was already checked; discard left half and mid.
                low = mid + 1;
            } else {
                // WHY: mid - 1 because mid was already checked; discard right half and mid.
                high = mid - 1;
            }
        }
        metrics.stopTimer(); // WHY: Stop stopwatch if not found
        return -1;           // Target document ID not present in catalog
    } else {
        // WHY: Recursive branch calls named recursive helper over full index range [0..size-1].
        int res = binarySearchRecursive(docs, targetId, 0, static_cast<int>(docs.size()) - 1, metrics);
        metrics.stopTimer(); // WHY: Stop stopwatch
        return res;
    }
}

// Explicit template instantiations
template int binarySearchIterative<int>(const std::vector<int>&, const int&, Metrics&);
template int binarySearchRecursive<int>(const std::vector<int>&, const int&, Metrics&);

} // namespace dls
