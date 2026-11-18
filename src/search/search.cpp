#include "dls/search.hpp"

namespace dls {

/**
 * @brief Plain named recursive binary search for Document catalog by ID.
 * 
 * IDEA:
 * Inspect the middle document at index mid = lo + (hi - lo) / 2.
 * If a[mid].id == target, return mid. If target < a[mid].id, recurse on the
 * left half [lo..mid - 1]; otherwise recurse on the right half [mid + 1..hi].
 * 
 * RECURRENCE:
 * T(n) = T(n/2) + 1, with base case T(0) = 1.
 * By the Master Theorem (Case 2: a = 1, b = 2, f(n) = 1 = Theta(n^0)),
 * critical exponent is log_b(a) = log_2(1) = 0, so T(n) = Theta(log n).
 * 
 * TIME COMPLEXITY:
 * Best case: O(1) (target found at midpoint on first call).
 * Average case: O(log n).
 * Worst case: O(log n).
 * 
 * SPACE COMPLEXITY:
 * O(log n) call stack frames due to recursion depth.
 * 
 * WHEN IT FAILS:
 * Fails (returns -1 or incorrect position) if the input vector 'a' is not
 * pre-sorted in ascending order by Document ID, or if lo/hi bounds are
 * corrupted/out of range.
 */
int binarySearchRecursive(const std::vector<Document>& a, int target, int lo, int hi, Metrics& metrics) {
    if (lo > hi) return -1;
    int mid = lo + (hi - lo) / 2;
    metrics.comparisons++;
    if (a[mid].id == target) return mid;
    metrics.comparisons++;
    if (target < a[mid].id) {
        return binarySearchRecursive(a, target, lo, mid - 1, metrics);
    } else {
        return binarySearchRecursive(a, target, mid + 1, hi, metrics);
    }
}

int binarySearchDocumentById(const std::vector<Document>& docs, int targetId, bool useRecursive, Metrics& metrics) {
    if (docs.empty()) return -1;

    metrics.startTimer();
    if (!useRecursive) {
        int low = 0;
        int high = static_cast<int>(docs.size()) - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            metrics.comparisons++;
            if (docs[mid].id == targetId) {
                metrics.stopTimer();
                return mid;
            }
            metrics.comparisons++;
            if (docs[mid].id < targetId) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        metrics.stopTimer();
        return -1;
    } else {
        int res = binarySearchRecursive(docs, targetId, 0, static_cast<int>(docs.size()) - 1, metrics);
        metrics.stopTimer();
        return res;
    }
}

// Explicit template instantiations
template int binarySearchIterative<int>(const std::vector<int>&, const int&, Metrics&);
template int binarySearchRecursive<int>(const std::vector<int>&, const int&, Metrics&);

} // namespace dls
