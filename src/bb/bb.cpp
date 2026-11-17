#include "dls/bb.hpp"
#include "dls/sort.hpp"
#include "dls/heap.hpp"
#include <vector>

namespace dls {

// ============================================================================
// 1. N-QUEENS (BACKTRACKING)
// ============================================================================

namespace {

void solveNQueensRecursive(int row, int n, std::vector<int>& currentCols,
                           std::vector<bool>& colUsed,
                           std::vector<bool>& diag1,
                           std::vector<bool>& diag2,
                           NQueensResult& result,
                           Metrics& metrics) {
    // Base case: placed all N queens successfully
    if (row == n) {
        result.totalSolutions++;
        result.solutions.push_back(currentCols);
        return;
    }

    // Try placing queen in each column of the current row
    for (int col = 0; col < n; ++col) {
        int d1 = row - col + n - 1; // Major diagonal index
        int d2 = row + col;         // Minor diagonal index

        metrics.comparisons++;
        // Check safety constraints
        if (!colUsed[col] && !diag1[d1] && !diag2[d2]) {
            // Place queen
            currentCols[row] = col;
            colUsed[col] = true;
            diag1[d1] = true;
            diag2[d2] = true;

            // Recurse to next row
            solveNQueensRecursive(row + 1, n, currentCols, colUsed, diag1, diag2, result, metrics);

            // Backtrack: remove queen
            colUsed[col] = false;
            diag1[d1] = false;
            diag2[d2] = false;
        }
    }
}

} // anonymous namespace

NQueensResult solveNQueens(int n, Metrics& metrics) {
    if (n <= 0) return {0, {}};

    metrics.startTimer();
    NQueensResult result;
    std::vector<int> currentCols(n, 0);
    std::vector<bool> colUsed(n, false);
    std::vector<bool> diag1(2 * n - 1, false);
    std::vector<bool> diag2(2 * n - 1, false);

    solveNQueensRecursive(0, n, currentCols, colUsed, diag1, diag2, result, metrics);

    metrics.stopTimer();
    return result;
}

// ============================================================================
// 2. SUBSET SUM (BACKTRACKING)
// ============================================================================

namespace {

bool subsetSumRecursive(const std::vector<int>& numbers, int index, int currentSum,
                        int remainingSum, int target, std::vector<int>& currentSubset,
                        SubsetSumResult& result, Metrics& metrics) {
    metrics.comparisons++;
    // Found exact target sum
    if (currentSum == target) {
        result.found = true;
        result.subset = currentSubset;
        return true;
    }

    // Base condition or bounding check:
    // If we exhausted elements, or current sum exceeds target,
    // or even taking ALL remaining elements cannot reach target
    if (index >= static_cast<int>(numbers.size()) || currentSum + numbers[index] > target || currentSum + remainingSum < target) {
        return false;
    }

    // Branch 1: Include numbers[index]
    currentSubset.push_back(numbers[index]);
    if (subsetSumRecursive(numbers, index + 1, currentSum + numbers[index],
                           remainingSum - numbers[index], target, currentSubset, result, metrics)) {
        return true;
    }

    // Backtrack: Exclude numbers[index]
    currentSubset.pop_back();

    // Branch 2: Exclude numbers[index]
    if (subsetSumRecursive(numbers, index + 1, currentSum,
                           remainingSum - numbers[index], target, currentSubset, result, metrics)) {
        return true;
    }

    return false;
}

} // anonymous namespace

SubsetSumResult solveSubsetSum(std::vector<int> numbers, int targetSum, Metrics& metrics) {
    metrics.startTimer();
    SubsetSumResult result;

    // Step 1: Sort numbers ascending using custom mergeSort
    mergeSort(numbers, metrics, [](int a, int b) { return a < b; });

    int totalSum = 0;
    for (int x : numbers) totalSum += x;

    std::vector<int> currentSubset;
    subsetSumRecursive(numbers, 0, 0, totalSum, targetSum, currentSubset, result, metrics);

    metrics.stopTimer();
    return result;
}

// ============================================================================
// 3. 0/1 KNAPSACK BRANCH-AND-BOUND
// ============================================================================

namespace {

struct BBNode {
    int level{-1};              // Level of node in decision tree (corresponds to item index)
    int value{0};               // Total value accumulated so far
    int weight{0};              // Total weight accumulated so far
    double bound{0.0};          // Fractional upper bound
    std::vector<int> takenIds;  // IDs of items taken in this state
};

// Comparator for Least-Cost BB: higher upper bound has higher priority
struct LCBBComp {
    bool operator()(const BBNode& a, const BBNode& b) const {
        return a.bound > b.bound; // Max-heap behavior
    }
};

// Calculate fractional upper bound for node u
double calculateBound(const BBNode& u, int n, int W, const std::vector<Item>& sortedItems, Metrics& metrics) {
    if (u.weight >= W) return 0.0;

    double bound = u.value;
    int totalW = u.weight;
    int j = u.level + 1;

    // Greedily take whole items while capacity allows
    while (j < n && totalW + sortedItems[j].weight <= W) {
        metrics.comparisons++;
        totalW += sortedItems[j].weight;
        bound += sortedItems[j].value;
        j++;
    }

    // If items remain, add fractional part of next item
    if (j < n) {
        metrics.comparisons++;
        bound += (static_cast<double>(W - totalW) / sortedItems[j].weight) * sortedItems[j].value;
    }

    return bound;
}

} // anonymous namespace

BBKnapsackResult knapsackBranchAndBound(const std::vector<Item>& items, int capacity, BBStrategy strategy, Metrics& metrics) {
    if (items.empty() || capacity <= 0) {
        return {0, {}, 0};
    }

    metrics.startTimer();
    int n = static_cast<int>(items.size());

    // Step 1: Sort items in descending order of value/weight ratio using custom mergeSort
    std::vector<Item> sortedItems = items;
    mergeSort(sortedItems, metrics, [](const Item& a, const Item& b) {
        double rA = static_cast<double>(a.value) / a.weight;
        double rB = static_cast<double>(b.value) / b.weight;
        return rA > rB;
    });

    BBKnapsackResult result;
    result.maxValue = 0;
    result.nodesExplored = 0;

    // Root node
    BBNode root;
    root.level = -1;
    root.value = 0;
    root.weight = 0;
    root.bound = calculateBound(root, n, capacity, sortedItems, metrics);

    // Containers for different strategies
    std::vector<BBNode> fifoQueue;
    size_t fifoHead = 0;

    std::vector<BBNode> lifoStack;

    MinHeap<BBNode, LCBBComp> lcHeap(LCBBComp(), &metrics);

    if (strategy == BBStrategy::FIFO) {
        fifoQueue.push_back(root);
    } else if (strategy == BBStrategy::LIFO) {
        lifoStack.push_back(root);
    } else {
        lcHeap.push(root);
    }

    auto hasMoreNodes = [&]() -> bool {
        if (strategy == BBStrategy::FIFO) return fifoHead < fifoQueue.size();
        if (strategy == BBStrategy::LIFO) return !lifoStack.empty();
        return !lcHeap.empty();
    };

    auto popNode = [&]() -> BBNode {
        result.nodesExplored++;
        if (strategy == BBStrategy::FIFO) {
            return fifoQueue[fifoHead++];
        } else if (strategy == BBStrategy::LIFO) {
            BBNode top = lifoStack.back();
            lifoStack.pop_back();
            return top;
        } else {
            BBNode top = lcHeap.top();
            lcHeap.pop();
            return top;
        }
    };

    auto pushNode = [&](const BBNode& node) {
        if (strategy == BBStrategy::FIFO) {
            fifoQueue.push_back(node);
        } else if (strategy == BBStrategy::LIFO) {
            lifoStack.push_back(node);
        } else {
            lcHeap.push(node);
        }
    };

    // State space search loop
    while (hasMoreNodes()) {
        BBNode curr = popNode();

        // Pruning check: if upper bound cannot exceed current best value, discard
        metrics.comparisons++;
        if (curr.bound <= result.maxValue) {
            continue;
        }

        int nextLevel = curr.level + 1;
        if (nextLevel >= n) continue;

        const Item& nextItem = sortedItems[nextLevel];

        // Branch 1: Include item at nextLevel (Left child)
        if (curr.weight + nextItem.weight <= capacity) {
            BBNode leftChild;
            leftChild.level = nextLevel;
            leftChild.weight = curr.weight + nextItem.weight;
            leftChild.value = curr.value + nextItem.value;
            leftChild.takenIds = curr.takenIds;
            leftChild.takenIds.push_back(nextItem.id);
            leftChild.bound = calculateBound(leftChild, n, capacity, sortedItems, metrics);

            metrics.comparisons++;
            if (leftChild.value > result.maxValue) {
                result.maxValue = leftChild.value;
                result.selectedItemIds = leftChild.takenIds;
            }

            metrics.comparisons++;
            if (leftChild.bound > result.maxValue) {
                pushNode(leftChild);
            }
        }

        // Branch 2: Exclude item at nextLevel (Right child)
        BBNode rightChild;
        rightChild.level = nextLevel;
        rightChild.weight = curr.weight;
        rightChild.value = curr.value;
        rightChild.takenIds = curr.takenIds;
        rightChild.bound = calculateBound(rightChild, n, capacity, sortedItems, metrics);

        metrics.comparisons++;
        if (rightChild.bound > result.maxValue) {
            pushNode(rightChild);
        }
    }

    metrics.stopTimer();
    return result;
}

} // namespace dls
