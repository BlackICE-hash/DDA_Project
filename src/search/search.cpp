#include "dls/search.hpp"
#include <functional>

namespace dls {

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
        // Lambda for recursive Document search
        std::function<int(int, int)> searchRec = [&](int low, int high) -> int {
            if (low > high) return -1;
            int mid = low + (high - low) / 2;
            metrics.comparisons++;
            if (docs[mid].id == targetId) return mid;
            metrics.comparisons++;
            if (targetId < docs[mid].id) {
                return searchRec(low, mid - 1);
            } else {
                return searchRec(mid + 1, high);
            }
        };

        int res = searchRec(0, static_cast<int>(docs.size()) - 1);
        metrics.stopTimer();
        return res;
    }
}

// Explicit template instantiations
template int binarySearchIterative<int>(const std::vector<int>&, const int&, Metrics&);
template int binarySearchRecursive<int>(const std::vector<int>&, const int&, Metrics&);

} // namespace dls
