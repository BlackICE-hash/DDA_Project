#pragma once

#include "dls/types.hpp"
#include <vector>

// NOTE: "class DisjointSet" is just a struct that keeps its internal parent and rank arrays
// private so outside code cannot corrupt tree pointers, exposing only find() and unite().
// "explicit" on the constructor prevents accidental integer-to-DisjointSet conversions.

namespace dls {

// CLASS: DisjointSet
// DATA STORED:
//   - parent_: std::vector<int> storing the parent index of each element in the forest.
//   - rank_: std::vector<int> storing the approximate tree depth of each root for balancing.
//   - metrics_: Metrics* optional pointer tracking comparison telemetry.
// PUBLIC METHODS:
//   - explicit DisjointSet(int n, Metrics* m = nullptr): Initializes n singleton sets (parent[i] = i); O(n).
//   - void setMetrics(Metrics* m): Attaches telemetry tracker; O(1).
//   - int find(int i): Finds root representative of element i with path compression; nearly O(1) amortized.
//   - bool unite(int i, int j): Merges sets containing i and j by rank, returning false if already connected; nearly O(1).
//   - bool connected(int i, int j): Returns true if i and j share the same root; nearly O(1).
class DisjointSet {
private:
    std::vector<int> parent_;
    std::vector<int> rank_;
    Metrics* metrics_{nullptr};

public:
    // WHAT:    Initializes n disjoint sets where each element is its own root.
    // IDEA:    Allocates parent and rank vectors of size n, setting parent_[i] = i and rank_[i] = 0.
    // EXAMPLE: DisjointSet(3) -> parent = [0, 1, 2], rank = [0, 0, 0]
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n) auxiliary space
    // VIVA:    Setting parent_[i] = 0 instead of i would falsely join all elements into set 0.
    explicit DisjointSet(int n, Metrics* m = nullptr)
        : parent_(n), rank_(n, 0), metrics_(m) {
        for (int i = 0; i < n; ++i) {
            parent_[i] = i;
        }
    }

    // WHAT:    Attaches or updates the metrics tracker.
    // IDEA:    Assigns internal pointer metrics_ to m.
    // EXAMPLE: setMetrics(&metrics) -> enables comparison tracking during Kruskal's MST
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Passing nullptr turns off comparison counting without errors.
    void setMetrics(Metrics* m) {
        metrics_ = m;
    }

    // WHAT:    Finds the root representative of element i with Path Compression.
    // IDEA:    Recurses up parent pointers to find the root, then points every visited node
    //          directly to that root, flattening the tree for future queries.
    // EXAMPLE: Tree 3 -> 2 -> 1 -> 0: find(3) returns 0 and repoints 3 and 2 directly to 0.
    // TIME:    best O(1) / avg O(alpha(n)) / worst O(log n)      SPACE: O(h) recursion stack
    // VIVA:    Omitting 'parent_[i] = find(parent_[i])' removes path compression, degrading find to O(n).
    int find(int i) {
        if (parent_[i] == i) {
            return i;
        }
        if (metrics_) metrics_->comparisons++;
        // Path compression: flatten tree directly to root
        parent_[i] = find(parent_[i]);
        return parent_[i];
    }

    // WHAT:    Merges the sets containing elements i and j using Union by Rank.
    // IDEA:    Finds roots of both elements. If roots differ, attaches the shorter tree
    //          under the root of the taller tree; increments rank only if ranks are equal.
    // EXAMPLE: unite(0, 1) attaches root 1 under root 0, returns true (component merged).
    // TIME:    best O(1) / avg O(alpha(n)) / worst O(log n)      SPACE: O(1)
    // VIVA:    Returning false when rootI == rootJ is how Kruskal's algorithm detects cycles.
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

    // WHAT:    Checks if elements i and j belong to the same set / connected component.
    // IDEA:    Calls find(i) and find(j) and checks whether both representatives are equal.
    // EXAMPLE: connected(1, 2) -> returns true if 1 and 2 share the same root
    // TIME:    best O(1) / avg O(alpha(n)) / worst O(log n)      SPACE: O(1)
    // VIVA:    Equivalent to 'find(i) == find(j)'; used in reachability tests.
    bool connected(int i, int j) {
        return find(i) == find(j);
    }
};

} // namespace dls
