#pragma once

#include "dls/types.hpp"
#include <vector>

namespace dls {

/**
 * @brief Disjoint Set Union (Union-Find) with Path Compression and Union by Rank.
 * 
 * IDEA:
 * Represents partition of elements into disjoint trees.
 * Path compression flattens the tree during find(i), pointing nodes directly to root.
 * Union by rank attaches smaller-rank tree under root of higher-rank tree.
 * 
 * TIME COMPLEXITY:
 * Nearly linear O(m * alpha(n)) where alpha is the inverse Ackermann function (alpha <= 4).
 * 
 * SPACE COMPLEXITY:
 * O(n) for parent and rank arrays.
 */
class DisjointSet {
private:
    std::vector<int> parent_;
    std::vector<int> rank_;
    Metrics* metrics_{nullptr};

public:
    explicit DisjointSet(int n, Metrics* m = nullptr)
        : parent_(n), rank_(n, 0), metrics_(m) {
        for (int i = 0; i < n; ++i) {
            parent_[i] = i;
        }
    }

    void setMetrics(Metrics* m) {
        metrics_ = m;
    }

    // Find root representative with Path Compression
    int find(int i) {
        if (parent_[i] == i) {
            return i;
        }
        if (metrics_) metrics_->comparisons++;
        // Path compression: flatten tree directly to root
        parent_[i] = find(parent_[i]);
        return parent_[i];
    }

    // Union two components by Rank
    bool unite(int i, int j) {
        int rootI = find(i);
        int rootJ = find(j);

        if (rootI == rootJ) {
            return false; // Already in same component, edge would create cycle
        }

        if (metrics_) metrics_->comparisons++;
        // Union by rank: attach shorter tree under taller tree root
        if (rank_[rootI] < rank_[rootJ]) {
            parent_[rootI] = rootJ;
        } else if (rank_[rootI] > rank_[rootJ]) {
            parent_[rootJ] = rootI;
        } else {
            parent_[rootJ] = rootI;
            rank_[rootI]++;
        }
        return true;
    }

    bool connected(int i, int j) {
        return find(i) == find(j);
    }
};

} // namespace dls
