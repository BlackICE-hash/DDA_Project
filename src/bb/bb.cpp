// NOTE: This file implements state-space tree search algorithms:
// 1. Backtracking (N-Queens, Subset Sum): explores depth-first and backtracks when constraints fail.
// 2. Branch and Bound (0/1 Knapsack): explores nodes using FIFO (queue), LIFO (stack),
//    or Least-Cost (priority queue / min-heap), pruning branches whose upper bound <= current best.

#include "dls/bb.hpp"
#include "dls/sort.hpp"
#include "dls/heap.hpp"
#include <vector>

namespace dls {

// ============================================================================
// 1. N-QUEENS (BACKTRACKING)
// ============================================================================

namespace {

// WHAT:    Recursive helper that places one queen per row using backtracking
// IDEA:    Try placing a queen in columns 0 to n-1. Use boolean flag arrays
//          to check in O(1) if column or diagonals are already under attack.
//          If safe, place queen and recurse. If recursion returns, unmark (backtrack).
// EXAMPLE: row=0, n=4 -> tries col 0, recurses to row 1, finds valid placement [1, 3, 0, 2]
// TIME:    O(N!) worst case         SPACE: O(N) recursion stack and boolean arrays
// VIVA:    Examiner asks: "Why 2*n-1 for diagonal arrays?"
//          Answer: On an n x n board, there are 2n - 1 major diagonals (row-col+n-1)
//          and 2n - 1 minor diagonals (row+col).
void solveNQueensRecursive(int row, int n, std::vector<int>& currentCols,
                           std::vector<bool>& colUsed,
                           std::vector<bool>& diag1,
                           std::vector<bool>& diag2,
                           NQueensResult& result,
                           Metrics& metrics) {
    // Base case: placed all N queens successfully (one in every row from 0 to n-1)
    if (row == n) {
        result.totalSolutions++;
        result.solutions.push_back(currentCols);
        return;
    }

    // Try placing queen in each column of the current row
    for (int col = 0; col < n; ++col) {
        // Map (row, col) to diagonal indices
        int d1 = row - col + n - 1; // Major diagonal index (top-left to bottom-right)
        int d2 = row + col;         // Minor diagonal index (top-right to bottom-left)

        metrics.comparisons++;
        // Check safety constraints: column and both diagonals must be free
        if (!colUsed[col] && !diag1[d1] && !diag2[d2]) {
            // Place queen at (row, col)
            currentCols[row] = col;
            colUsed[col] = true;
            diag1[d1] = true;
            diag2[d2] = true;

            // Recurse to place queen in next row
            solveNQueensRecursive(row + 1, n, currentCols, colUsed, diag1, diag2, result, metrics);

            // Backtrack: remove queen and unmark flags so other paths can try this cell
            colUsed[col] = false;
            diag1[d1] = false;
            diag2[d2] = false;
        }
    }
}

} // anonymous namespace

// WHAT:    Solves the N-Queens puzzle, finding all safe arrangements of N queens on N x N board
// IDEA:    Initializes tracker arrays and calls the recursive backtracking solver starting at row 0.
// EXAMPLE: n = 4 -> finds 2 distinct safe configurations
// TIME:    O(N!) worst case         SPACE: O(N) auxiliary space
// VIVA:    Examiner asks: "What is the smallest N > 1 with solutions?"
//          Answer: N = 4 (for N=2 and N=3, no solutions exist).
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

// WHAT:    Recursive backtracking helper to find a subset of numbers summing to target
// IDEA:    At each element, make 2 choices: include it or exclude it.
//          Prune early if current sum + element exceeds target, or if remaining sum is too small.
// EXAMPLE: numbers=[2,3,7], target=5 -> includes 2, then 3 -> sum=5, returns true
// TIME:    O(2^n) worst case        SPACE: O(n) recursion stack
// VIVA:    Examiner asks: "Why sort numbers before calling this?"
//          Answer: Sorting allows early bounding: if currentSum + numbers[index] > target,
//          all subsequent elements are also too large, so we can prune immediately.
bool subsetSumRecursive(const std::vector<int>& numbers, int index, int currentSum,
                        int remainingSum, int target, std::vector<int>& currentSubset,
                        SubsetSumResult& result, Metrics& metrics) {
    metrics.comparisons++;
    // Found exact target sum: record answer and terminate search
    if (currentSum == target) {
        result.found = true;
        result.subset = currentSubset;
        return true;
    }

    // Base condition or bounding check:
    // 1. index exhausted: no more numbers left
    // 2. currentSum + numbers[index] > target: since sorted, all later numbers are even larger
    // 3. currentSum + remainingSum < target: even taking all remaining numbers cannot reach target
    if (index >= static_cast<int>(numbers.size()) || currentSum + numbers[index] > target || currentSum + remainingSum < target) {
        return false;
    }

    // Branch 1: Include numbers[index]
    currentSubset.push_back(numbers[index]);
    if (subsetSumRecursive(numbers, index + 1, currentSum + numbers[index],
                           remainingSum - numbers[index], target, currentSubset, result, metrics)) {
        return true;
    }

    // Backtrack: Exclude numbers[index] by popping it
    currentSubset.pop_back();

    // Branch 2: Exclude numbers[index]
    if (subsetSumRecursive(numbers, index + 1, currentSum,
                           remainingSum - numbers[index], target, currentSubset, result, metrics)) {
        return true;
    }

    return false;
}

} // anonymous namespace

// WHAT:    Finds whether any subset of numbers sums exactly to targetSum
// IDEA:    Sorts numbers ascending, then explores include/exclude decisions with bounding pruning.
// EXAMPLE: numbers = [3, 34, 4, 12, 5, 2], target = 9 -> found = true, subset = [2, 3, 4]
// TIME:    O(2^n) worst case, much faster with pruning  SPACE: O(n) recursion depth
// VIVA:    Examiner asks: "What is the relationship between Subset Sum and 0/1 Knapsack?"
//          Answer: Subset Sum is a special case of 0/1 Knapsack where each item's value equals its weight.
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

// WHAT:    Calculates the upper bound on max profit obtainable from subtree rooted at node u
// IDEA:    Uses the greedy Fractional Knapsack relaxation:
//          1. Start with profit already accumulated in node u.
//          2. Greedily take entire items from level+1 onwards while remaining capacity permits.
//          3. Take a fractional portion of the next item that doesn't fit entirely.
//          Because fractional knapsack >= 0/1 knapsack, this provides a valid upper bound.
// EXAMPLE: node has value=10, weight=2, remaining capacity=3. Next item has value=12, weight=4.
//          Fractional bound = 10 + (3/4)*12 = 19.
// TIME:    O(n) per node            SPACE: O(1) auxiliary
// VIVA:    Examiner asks: "Why does the fractional knapsack give an upper bound?"
//          Answer: Relaxing the integer constraint allows taking high value-to-weight fractions,
//          which can never yield less profit than integer 0/1 restrictions.
double calculateBound(const BBNode& u, int n, int W, const std::vector<Item>& sortedItems, Metrics& metrics) {
    // If current node weight already exceeds capacity, this node is invalid (bound = 0)
    if (u.weight >= W) return 0.0;

    // Start with profit earned so far at this node
    double bound = u.value;
    int totalW = u.weight;
    int j = u.level + 1;

    // Greedily take whole items while remaining capacity allows
    while (j < n && totalW + sortedItems[j].weight <= W) {
        metrics.comparisons++;
        totalW += sortedItems[j].weight; // Accumulate weight
        bound += sortedItems[j].value;   // Accumulate value
        j++;                             // Move to next item
    }

    // If items remain and knapsack is not full, add fractional part of next item
    if (j < n) {
        metrics.comparisons++;
        // Fraction is (remaining capacity / item weight) * item value
        bound += (static_cast<double>(W - totalW) / sortedItems[j].weight) * sortedItems[j].value;
    }

    return bound;
}

} // anonymous namespace

// WHAT:    Solves 0/1 Knapsack using Branch-and-Bound with FIFO, LIFO, or Least-Cost search
// IDEA:    1. Sort items by value/weight ratio descending.
//          2. Represent choices as a state-space tree where level i decides item i (include or exclude).
//          3. For each active node, calculate fractional upper bound.
//          4. Pruning: If a node's upper bound <= current best known solution (maxValue),
//             kill/discard that node and do not expand its children.
//          5. Container choice determines search order:
//             - FIFO uses a queue (Breadth-First Search)
//             - LIFO uses a stack (Depth-First Search)
//             - LeastCost uses a min-heap on negative bound (Best-First Search)
// EXAMPLE: items=[(w:2,v:10),(w:3,v:15)], W=4 -> explores branches, finds optimal value=15 or best combination
// TIME:    O(2^n) worst case, typically visits far fewer nodes  SPACE: O(2^n) worst case in queue/heap
// VIVA:    Examiner asks: "Which strategy explores the fewest nodes?"
//          Answer: Least-Cost (Best-First) typically explores the fewest nodes because it always
//          expands the most promising node first, raising the lower bound quickly and pruning more branches.
BBKnapsackResult knapsackBranchAndBound(const std::vector<Item>& items, int capacity, BBStrategy strategy, Metrics& metrics) {
    // Guard against empty items or non-positive capacity
    if (items.empty() || capacity <= 0) {
        return {0, {}, 0};
    }

    metrics.startTimer();
    int n = static_cast<int>(items.size());

    // Step 1: Sort items in descending order of value/weight ratio using custom mergeSort
    // WHY: Greedy fractional bound requires ratio ordering to compute the optimal upper bound quickly
    std::vector<Item> sortedItems = items;
    mergeSort(sortedItems, metrics, [](const Item& a, const Item& b) {
        double rA = static_cast<double>(a.value) / a.weight;
        double rB = static_cast<double>(b.value) / b.weight;
        return rA > rB; // Descending order
    });

    BBKnapsackResult result;
    result.maxValue = 0;
    result.nodesExplored = 0;

    // Root node represents initial state before deciding on any item
    BBNode root;
    root.level = -1;  // -1 means no items considered yet
    root.value = 0;   // 0 profit so far
    root.weight = 0;  // 0 weight consumed so far
    // Calculate fractional upper bound for root (best possible theoretical value)
    root.bound = calculateBound(root, n, capacity, sortedItems, metrics);

    // Containers for different strategies:
    // FIFO uses vector with head index (queue simulation)
    std::vector<BBNode> fifoQueue;
    size_t fifoHead = 0;

    // LIFO uses vector push_back / pop_back (stack simulation)
    std::vector<BBNode> lifoStack;

    // Least-Cost uses custom BBMinHeap (priority queue prioritizing highest upper bound)
    BBMinHeap lcHeap(&metrics);

    // Enqueue / push root into the appropriate container
    if (strategy == BBStrategy::FIFO) {
        fifoQueue.push_back(root);
    } else if (strategy == BBStrategy::LIFO) {
        lifoStack.push_back(root);
    } else {
        lcHeap.push(root);
    }

    // Helper lambda: returns true if there are more nodes to process in the active container
    auto hasMoreNodes = [&]() -> bool {
        if (strategy == BBStrategy::FIFO) return fifoHead < fifoQueue.size();
        if (strategy == BBStrategy::LIFO) return !lifoStack.empty();
        return !lcHeap.empty();
    };

    // Helper lambda: extracts the next active node according to chosen strategy
    auto popNode = [&]() -> BBNode {
        result.nodesExplored++;
        if (strategy == BBStrategy::FIFO) {
            return fifoQueue[fifoHead++]; // Extract from front (FIFO)
        } else if (strategy == BBStrategy::LIFO) {
            BBNode top = lifoStack.back(); // Extract from back/top (LIFO)
            lifoStack.pop_back();
            return top;
        } else {
            BBNode top = lcHeap.top();     // Extract node with highest bound (Least-Cost)
            lcHeap.pop();
            return top;
        }
    };

    // Helper lambda: inserts a newly generated child node into the active container
    auto pushNode = [&](const BBNode& node) {
        if (strategy == BBStrategy::FIFO) {
            fifoQueue.push_back(node);
        } else if (strategy == BBStrategy::LIFO) {
            lifoStack.push_back(node);
        } else {
            lcHeap.push(node);
        }
    };

    // State space search loop: continue while unpruned nodes exist
    while (hasMoreNodes()) {
        BBNode curr = popNode();

        // Pruning check: if this node's upper bound cannot beat our current best value, prune it!
        metrics.comparisons++;
        if (curr.bound <= result.maxValue) {
            continue; // Discard node: no solution in this subtree can beat what we already found
        }

        // Move to the next item level
        int nextLevel = curr.level + 1;
        if (nextLevel >= n) continue; // Reached beyond last item

        const Item& nextItem = sortedItems[nextLevel];

        // Branch 1: Include item at nextLevel (Left child in state-space tree)
        if (curr.weight + nextItem.weight <= capacity) {
            BBNode leftChild;
            leftChild.level = nextLevel;
            leftChild.weight = curr.weight + nextItem.weight;
            leftChild.value = curr.value + nextItem.value;
            leftChild.takenIds = curr.takenIds;
            leftChild.takenIds.push_back(nextItem.id);
            leftChild.bound = calculateBound(leftChild, n, capacity, sortedItems, metrics);

            // Update best found full solution if this feasible node has higher value
            metrics.comparisons++;
            if (leftChild.value > result.maxValue) {
                result.maxValue = leftChild.value;
                result.selectedItemIds = leftChild.takenIds;
            }

            // Only explore left child further if its upper bound beats best known value
            metrics.comparisons++;
            if (leftChild.bound > result.maxValue) {
                pushNode(leftChild);
            }
        }

        // Branch 2: Exclude item at nextLevel (Right child in state-space tree)
        BBNode rightChild;
        rightChild.level = nextLevel;
        rightChild.weight = curr.weight;   // Weight unchanged
        rightChild.value = curr.value;     // Value unchanged
        rightChild.takenIds = curr.takenIds;
        rightChild.bound = calculateBound(rightChild, n, capacity, sortedItems, metrics);

        // Only explore right child further if its upper bound beats best known value
        metrics.comparisons++;
        if (rightChild.bound > result.maxValue) {
            pushNode(rightChild);
        }
    }

    metrics.stopTimer();
    return result;
}

} // namespace dls
