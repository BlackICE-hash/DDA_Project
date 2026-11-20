# DLS Engine: Comprehensive Algorithm Study Notes & Viva Guide

This study guide contains theoretical foundations, correctness proofs, hand-traceable numerical examples, and viva questions for every algorithm implemented in the **Digital Library Search and Recommendation Engine**.

---

## Table of Contents
1. [Sorting & Searching](#1-sorting--searching)
   - 1.1 [Merge Sort](#11-merge-sort)
   - 1.2 [Quick Sort](#12-quick-sort)
   - 1.3 [Binary Search (Iterative & Recursive)](#13-binary-search-iterative--recursive)
2. [Divide and Conquer](#2-divide-and-conquer)
   - 2.1 [Maximum Subarray](#21-maximum-subarray)
   - 2.2 [Fast Powering (Modular Exponentiation)](#22-fast-powering-modular-exponentiation)
   - 2.3 [Celebrity Discovery](#23-celebrity-discovery)
   - 2.4 [Strassen's Matrix Multiplication](#24-strassens-matrix-multiplication)
3. [Greedy Algorithms](#3-greedy-algorithms)
   - 3.1 [Activity Selection](#31-activity-selection)
   - 3.2 [Interval Partitioning](#32-interval-partitioning)
   - 3.3 [Huffman Coding](#33-huffman-coding)
   - 3.4 [Dijkstra's Algorithm](#34-dijkstras-algorithm)
   - 3.5 [Prim's Algorithm](#35-prims-algorithm)
   - 3.6 [Kruskal's Algorithm (with Union-Find)](#36-kruskals-algorithm-with-union-find)
4. [Dynamic Programming](#4-dynamic-programming)
   - 4.1 [0/1 Knapsack (Acquisitions under Budget)](#41-01-knapsack-acquisitions-under-budget)
   - 4.2 [Matrix Chain Multiplication](#42-matrix-chain-multiplication)
   - 4.3 [Longest Common Subsequence (LCS)](#43-longest-common-subsequence-lcs)
   - 4.4 [Optimal Binary Search Tree (OBST)](#44-optimal-binary-search-tree-obst)
   - 4.5 [Bellman-Ford Algorithm](#45-bellman-ford-algorithm)
   - 4.6 [Floyd-Warshall Algorithm](#46-floyd-warshall-algorithm)
   - 4.7 [Traveling Salesperson Problem (TSP via Bitmask DP)](#47-traveling-salesperson-problem-tsp-via-bitmask-dp)
5. [Backtracking & Branch-and-Bound](#5-backtracking--branch-and-bound)
   - 5.1 [N-Queens Problem](#51-n-queens-problem)
   - 5.2 [Subset Sum Problem](#52-subset-sum-problem)
   - 5.3 [0/1 Knapsack Branch-and-Bound (FIFO, LIFO, Least-Cost)](#53-01-knapsack-branch-and-bound-fifo-lifo-least-cost)
6. [Flow and Matching](#6-flow-and-matching)
   - 6.1 [Edmonds-Karp Maximum Flow](#61-edmonds-karp-maximum-flow)
   - 6.2 [Maximum Bipartite Matching via Max Flow](#62-maximum-bipartite-matching-via-max-flow)
7. [Number Theory](#7-number-theory)
   - 7.1 [Chinese Remainder Theorem (with Extended Euclidean)](#71-chinese-remainder-theorem-with-extended-euclidean)
8. [String Matching](#8-string-matching)
   - 8.1 [Naive String Matching](#81-naive-string-matching)
   - 8.2 [Rabin-Karp Algorithm](#82-rabin-karp-algorithm)
   - 8.3 [Finite Automaton String Matching](#83-finite-automaton-string-matching)
   - 8.4 [Knuth-Morris-Pratt (KMP) Algorithm](#84-knuth-morris-pratt-kmp-algorithm)

---

## 1. Sorting & Searching

### 1.1 Merge Sort
* **Problem Solved:** Stable sorting of library catalog documents by relevance score descending.
* **Core Idea:** Divide the list into two halves, recursively sort each half, and merge the sorted halves using a temporary buffer.
* **Hand-Traceable Example:** Input: `[38, 27, 43, 3, 9]`
  1. Divide into `[38, 27]` and `[43, 3, 9]`.
  2. Sub-divide into `[38]`, `[27]` and `[43]`, `[3, 9]`.
  3. Merge `[38]` and `[27]` $\to$ `[27, 38]`.
  4. Merge `[3]` and `[9]` $\to$ `[3, 9]`, then with `[43]` $\to$ `[3, 9, 43]`.
  5. Merge `[27, 38]` and `[3, 9, 43]`: compare 27 vs 3 $\to$ 3; 27 vs 9 $\to$ 9; 27 vs 43 $\to$ 27; 38 vs 43 $\to$ 38; remainder $\to$ 43. Final: `[3, 9, 27, 38, 43]`.
* **Proof of Correctness (Loop Invariant):** At the start of each iteration of the merge step, the merged subarray `arr[left..k-1]` contains the $k - left$ smallest elements of `buffer[left..mid]` and `buffer[mid+1..right]` in strictly sorted order.
* **Recurrence & Complexity:**
  $$T(n) = 2T(n/2) + \Theta(n) \implies \Theta(n \log n) \text{ across Best, Average, and Worst cases.}$$
  $$\text{Space Complexity: } O(n) \text{ auxiliary buffer.}$$
* **Failure Cases:** Memory-constrained environments where an extra $O(n)$ allocation cannot fit in RAM.
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why is Merge Sort preferred for linked lists and stable sorting?*  
     *A:* Merging linked lists requires $O(1)$ extra space (pointer manipulation), and equal elements maintain their relative initial order because ties are broken in favor of the left subarray.
  2. *Q: Can Merge Sort be made in-place?*  
     *A:* In-place merging is possible but increases time complexity to $O(n \log^2 n)$ or incurs large constant factors, negating practical advantages.
  3. *Q: What is the exact number of comparisons in the merge step?*  
     *A:* Between $n/2$ (best case when all elements in one half are smaller than the other) and $n - 1$ (worst case when elements strictly alternate).

---

### 1.2 Quick Sort
* **Problem Solved:** Fast in-place sorting of document records by ID ascending.
* **Core Idea:** Select a pivot (e.g. median-of-three), partition elements into $< \text{pivot}$ on the left and $\ge \text{pivot}$ on the right, then recursively sort partitions.
* **Hand-Traceable Example:** Input: `[9, 2, 7, 4, 5]`, Pivot = 5 (at end)
  1. $i = -1$.
  2. $j=0$ (9): $9 \not< 5$.
  3. $j=1$ (2): $2 < 5 \implies i=0$, swap 9 and 2 $\to$ `[2, 9, 7, 4, 5]`.
  4. $j=2$ (7): $7 \not< 5$.
  5. $j=3$ (4): $4 < 5 \implies i=1$, swap 9 and 4 $\to$ `[2, 4, 7, 9, 5]`.
  6. Swap pivot (5) with $i+1$ (7) $\to$ `[2, 4, 5, 9, 7]`. Pivot 5 is at index 2.
  7. Recurse on `[2, 4]` and `[9, 7]`.
* **Proof of Correctness:** Lomuto's invariant: at step $j$, all elements in `arr[low..i]` are $\le \text{pivot}$ and all elements in `arr[i+1..j-1]` are $> \text{pivot}$.
* **Recurrence & Complexity:**
  - Best/Average: $T(n) = 2T(n/2) + \Theta(n) \implies \Theta(n \log n)$.
  - Worst: $T(n) = T(n-1) + \Theta(n) \implies \Theta(n^2)$ (unbalanced partition).
  - Space: $O(\log n)$ average recursion stack frames, $O(n)$ worst.
* **Failure Cases:** Naive pivot choice (first or last element) on already sorted or all-identical arrays yields $O(n^2)$ time and call-stack overflow.
* **3 Likely Viva Questions & Answers:**
  1. *Q: How does Median-of-Three pivot selection prevent $O(n^2)$ degradation?*  
     *A:* Inspecting first, middle, and last elements and choosing their median guarantees that the pivot is never the extreme minimum or maximum of a sorted/reverse-sorted array.
  2. *Q: Why is Quick Sort faster than Merge Sort in practice?*  
     *A:* Likely due to data movement and cache behaviour (sequential memory scans without buffer reallocation) and smaller constant factors.
  3. *Q: How can worst-case recursion depth be restricted to $O(\log n)$?*  
     *A:* Tail-call optimization: always recurse on the smaller partition first and iterate on the larger partition.

---

### 1.3 Binary Search (Iterative & Recursive)
* **Problem Solved:** Instant $O(\log n)$ retrieval of a document by unique ID in a pre-sorted catalog.
* **Core Idea:** Halve the search space each step by comparing target with middle element.
* **Hand-Traceable Example:** Sorted array `[10, 25, 40, 55, 70]`, target = 55
  1. $low = 0, high = 4 \implies mid = 2$, `arr[2] = 40`. $40 < 55 \implies low = 3$.
  2. $low = 3, high = 4 \implies mid = 3$, `arr[3] = 55`. Match found at index 3.
* **Proof of Correctness:** Invariant: if the target key exists in the array, it must reside within `arr[low..high]`. Because $mid$ is discarded when unequal, range strictly contracts.
* **Recurrence & Complexity:**
  $$T(n) = T(n/2) + \Theta(1) \implies \Theta(\log n) \text{ time.}$$
  $$\text{Space: } O(1) \text{ iterative, } O(\log n) \text{ call stack recursive.}$$
* **Failure Cases:** The input array is **unsorted** (will return incorrect $-1$ or wrong index).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why write `mid = low + (high - low)/2` instead of `(low + high)/2`?*  
     *A:* To prevent signed 32-bit integer overflow when $low + high > 2^{31} - 1$.
  2. *Q: Iterative vs Recursive: which is better in production?*  
     *A:* Iterative is preferred because it runs in $O(1)$ memory without risking call stack overflow.
  3. *Q: What is the maximum number of comparisons to search 1,000,000 items?*  
     *A:* $\lfloor \log_2(1,000,000) \rfloor + 1 = 20$ comparisons.

---

## 2. Divide and Conquer

### 2.1 Maximum Subarray
* **Problem Solved:** Identifying the peak reading-activity window in a continuous stream of visitor net deltas.
* **Core Idea:** The maximum subarray either lies entirely in the left half, entirely in the right half, or crosses the midpoint.
* **Hand-Traceable Example:** `[-2, 1, -3, 4, -1, 2, 1, -5, 4]`
  - Left half: `[-2, 1, -3, 4]`, Right half: `[-1, 2, 1, -5, 4]`.
  - Crossing midpoint: max left-sum ending at mid + max right-sum starting after mid:
    Left suffix max: $4 + (-3) + 1 = 2 \dots \to 4$. Right prefix max: $(-1) + 2 + 1 = 2$.
    Cross sum = $4 + 2 = 6$ corresponding to subarray `[4, -1, 2, 1]`.
* **Proof of Correctness:** Exhaustive case coverage: any contiguous range $[i..j]$ in an array divided at $mid$ satisfies $j \le mid$ (left), $i > mid$ (right), or $i \le mid < j$ (cross).
* **Recurrence & Complexity:**
  $$T(n) = 2T(n/2) + \Theta(n) \implies \Theta(n \log n) \text{ time, } O(\log n) \text{ stack space.}$$
  *(Note: Kadane's algorithm achieves $O(n)$ time and $O(1)$ space).*
* **Failure Cases:** All-negative numbers (algorithm correctly returns the single least-negative element if non-empty requirement holds).
* **3 Likely Viva Questions & Answers:**
  1. *Q: How does the crossing subarray helper run in $O(n)$ time?*  
     *A:* By scanning linearly backward from $mid$ to $low$ and forward from $mid+1$ to $high$.
  2. *Q: Why study the $O(n \log n)$ D&C approach if Kadane's algorithm is $O(n)$?*  
     *A:* The D&C approach parallelizes naturally across distributed nodes / map-reduce architectures.
  3. *Q: What is the Master Theorem case?*  
     *A:* $a = 2, b = 2, f(n) = n \implies n^{\log_2 2} = n \implies \text{Case 2 with } k = 0 \implies \Theta(n \log n)$.

---

### 2.2 Fast Powering (Modular Exponentiation)
* **Problem Solved:** Computing exponential citation decay metrics and modular inverses in cryptosystems: $b^e \pmod m$.
* **Core Idea:** Binary exponentiation: square the base at each bit position and multiply into result when the exponent bit is 1.
* **Hand-Traceable Example:** $3^{13} \pmod{1000}$, $13 = 1101_2$
  1. Bit 0 (1): $res = (1 \cdot 3) = 3$; $base = 3^2 = 9$.
  2. Bit 1 (0): $res = 3$; $base = 9^2 = 81$.
  3. Bit 2 (1): $res = (3 \cdot 81) = 243$; $base = 81^2 = 6561 \equiv 561$.
  4. Bit 3 (1): $res = (243 \cdot 561) = 136323 \equiv 323$.
  Final Answer: 323.
* **Proof of Correctness:** Mathematical induction on binary representation of $e = \sum_{i=0}^k b_i 2^i$. $x^e = \prod_{b_i=1} x^{2^i}$.
* **Complexity:** Time: $O(\log e)$ multiplications. Space: $O(1)$ auxiliary.
* **Failure Cases:** Modulus $m \le 0$ or negative exponents without modular inverse.
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why cast to 128-bit integers during modular multiplication?*  
     *A:* If $a, b \approx 10^9$, their product $\approx 10^{18}$, which fits in signed 64-bit (`int64_t`), but if modulus is larger, product overflows standard 64-bit words before modulo.
  2. *Q: How many multiplications does $x^{1024}$ take?*  
     *A:* Exactly $\log_2(1024) = 10$ squarings.
  3. *Q: How does fast powering relate to Fermat's Little Theorem?*  
     *A:* Modular inverse $a^{-1} \equiv a^{p-2} \pmod p$ for prime $p$ is computed via fast powering in $O(\log p)$.

---

### 2.3 Celebrity Discovery
* **Problem Solved:** Locating the authoritative research paper in a citation matrix (cited by all $n-1$ papers, cites none).
* **Core Idea:** Pairwise elimination. If paper $A$ cites $B$, $A$ cannot be a celebrity (celebrities cite nobody). If $A$ does not cite $B$, $B$ cannot be a celebrity (celebrities are cited by all).
* **Hand-Traceable Example:** 4 papers $\{0, 1, 2, 3\}$. Candidate $C = 0$.
  1. Query $M[0][1]$: 0 cites 1 $\implies 0$ eliminated, candidate becomes 1.
  2. Query $M[1][2]$: 1 cites 2 $\implies 1$ eliminated, candidate becomes 2.
  3. Query $M[2][3]$: 2 does NOT cite 3 $\implies 3$ eliminated, candidate remains 2.
  4. Verification: check row 2 is all 0s, column 2 is all 1s. Paper 2 verified!
* **Proof of Correctness:** Every query eliminates exactly one candidate from celebrity status. After $n-1$ queries, exactly one survivor remains. The verification step confirms whether the survivor satisfies both celebrity axioms.
* **Complexity:**
  - Elimination queries: $n - 1$.
  - Verification queries: $2(n - 1)$.
  - Total queries: $3(n - 1) = O(n)$. Space: $O(1)$.
* **Failure Cases:** When no celebrity exists (candidate cited someone or someone failed to cite candidate $\to$ returns $-1$).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Can two celebrities exist in the same network?*  
     *A:* No. If $A$ and $B$ were both celebrities, $A$ must cite $B$ (celebrity must be cited by all), but $A$ cannot cite $B$ (celebrity cites nobody). Contradiction.
  2. *Q: Why is brute-force celebrity checking inefficient?*  
     *A:* Checking all rows and columns takes $O(n^2)$ queries.
  3. *Q: Can this be solved recursively?*  
     *A:* Yes: solve for $n-1$ papers to get candidate $C'$, then pit $C'$ against the $n$-th paper.

---

### 2.4 Strassen's Matrix Multiplication
* **Problem Solved:** Fast multiplication of large co-citation matrices.
* **Core Idea:** Partition $n \times n$ matrices into four submatrices and compute 7 recursive products instead of 8 using algebraic additions.
* **Hand-Traceable Example:** $2 \times 2$ matrices $A = \begin{pmatrix} 1 & 2 \\ 3 & 4 \end{pmatrix}, B = \begin{pmatrix} 5 & 6 \\ 7 & 8 \end{pmatrix}$
  - $M_1 = (A_{11}+A_{22})(B_{11}+B_{22}) = (1+4)(5+8) = 5 \times 13 = 65$.
  - $M_2 = (A_{21}+A_{22})B_{11} = (3+4) \times 5 = 35$.
  - $M_3 = A_{11}(B_{12}-B_{22}) = 1 \times (6-8) = -2$.
  - $M_4 = A_{22}(B_{21}-B_{11}) = 4 \times (7-5) = 8$.
  - $M_5 = (A_{11}+A_{12})B_{22} = (1+2) \times 8 = 24$.
  - $M_6 = (A_{21}-A_{11})(B_{11}+B_{12}) = (3-1)(5+6) = 22$.
  - $M_7 = (A_{12}-A_{22})(B_{21}+B_{22}) = (2-4)(7+8) = -30$.
  - $C_{11} = M_1 + M_4 - M_5 + M_7 = 65 + 8 - 24 - 30 = 19$.
  - $C_{12} = M_3 + M_5 = -2 + 24 = 22$.
  - $C_{21} = M_2 + M_4 = 35 + 8 = 43$.
  - $C_{22} = M_1 - M_2 + M_3 + M_6 = 65 - 35 - 2 + 22 = 50$.
  Result matches classical: $\begin{pmatrix} 19 & 22 \\ 43 & 50 \end{pmatrix}$.
* **Recurrence & Complexity:**
  $$T(n) = 7T(n/2) + \Theta(n^2) \implies \Theta(n^{\log_2 7}) \approx \Theta(n^{2.8074})$$
* **Failure Cases:** Matrices not powers of 2 (resolved by zero-padding); small matrices where submatrix allocation overhead exceeds operations saved (resolved by threshold cutoff $\approx 16-64$).
* **3 Likely Viva Questions & Answers:**
  1. *Q: How does Strassen reduce 8 multiplications to 7?*  
     *A:* By trading 1 expensive scalar multiplication for 18 cheaper scalar additions/subtractions.
  2. *Q: Why isn't Strassen used universally for small matrices?*  
     *A:* Recursive allocation overhead, cache thrashing, and loss of numerical precision with floating points.
  3. *Q: What is the theoretical fastest matrix multiplication algorithm today?*  
     *A:* Algorithms based on the Coppersmith-Winograd method achieve $\approx O(n^{2.371})$, but have astronomical constant factors.

---

## 3. Greedy Algorithms

### 3.1 Activity Selection
* **Problem Solved:** Maximizing the number of non-overlapping study sessions booked in a single reading room.
* **Core Idea:** Greedily select the activity that finishes earliest, leaving maximum remaining time for subsequent activities.
* **Hand-Traceable Example:** Activities sorted by finish time:
  $A_1(1..4), A_2(3..5), A_3(0..6), A_4(5..7), A_5(8..11)$
  1. Select $A_1(1..4)$. Last finish = 4.
  2. $A_2$ starts at $3 < 4$ $\implies$ Skip.
  3. $A_3$ starts at $0 < 4$ $\implies$ Skip.
  4. $A_4$ starts at $5 \ge 4 \implies$ Select $A_4(5..7)$. Last finish = 7.
  5. $A_5$ starts at $8 \ge 7 \implies$ Select $A_5(8..11)$. Last finish = 11.
  Selected: $\{A_1, A_4, A_5\}$ (3 activities).
* **Proof of Correctness (Greedy Choice Property via Exchange Argument):**
  Let $A_k$ be the activity with earliest finish time. Let $S^*$ be an optimal solution. If $S^*$ does not start with $A_k$, replace the first activity in $S^*$ with $A_k$. Since $f_k \le f_{\text{first}}$, $A_k$ cannot conflict with subsequent activities in $S^*$. Thus, $S^*$ remains valid and optimal.
* **Complexity:** $O(n \log n)$ to sort by finish time, $O(n)$ single scan. Space: $O(1)$ beyond output.
* **Failure Cases:** Activities have variable values/weights (requires Weighted Interval Scheduling via DP).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why sort by finish time instead of start time or shortest duration?*  
     *A:* Earliest start time can pick a long activity that blocks all others. Shortest duration can pick an activity in the middle that conflicts with two others.
  2. *Q: Does greedy choice always produce a unique optimal solution?*  
     *A:* No, multiple distinct subsets can achieve the same maximum cardinality.
  3. *Q: What if activities are already sorted?*  
     *A:* The algorithm runs in $O(n)$ time.

---

### 3.2 Interval Partitioning
* **Problem Solved:** Finding the minimum number of reading rooms needed to schedule all requested group sessions without conflicts.
* **Core Idea:** Sort intervals by start time. Use a MinHeap tracking active rooms by earliest finish time. Reuse a room if its finish time $\le$ current start time; else allocate a new room.
* **Hand-Traceable Example:** Intervals: $I_1(1..4), I_2(2..5), I_3(4..7)$
  1. Sort by start: $I_1(1..4), I_2(2..5), I_3(4..7)$.
  2. $I_1(1..4)$: Heap empty $\implies$ Room 1 allocated (Finishes at 4). Heap: `[(4, R1)]`.
  3. $I_2(2..5)$: Earliest room finishes at $4 > 2$ (busy) $\implies$ Room 2 allocated (Finishes at 5). Heap: `[(4, R1), (5, R2)]`.
  4. $I_3(4..7)$: Earliest room R1 finishes at $4 \le 4$ (free!) $\implies$ Reuse R1. R1 finish becomes 7. Heap: `[(5, R2), (7, R1)]`.
  Total Rooms: 2.
* **Proof of Correctness:** At any point where a new room $d+1$ is allocated, there are $d$ existing intervals mutually overlapping at the current interval's start time. Hence, the "depth" of the interval set is at least $d+1$. By the Interval Partitioning Theorem, no valid schedule can use fewer rooms than the maximum depth.
* **Complexity:** Time: $O(n \log n)$ sorting $+ O(n \log d)$ heap operations ($d \le n$). Space: $O(n)$.
* **Failure Cases:** Rigid fixed intervals that cannot be accommodated within fixed room budgets.
* **3 Likely Viva Questions & Answers:**
  1. *Q: What is the relationship between depth and number of rooms?*  
     *A:* $\text{Number of rooms} = \text{Depth} = \max_{t} (\text{concurrent active intervals at time } t)$.
  2. *Q: Why sort by start time rather than finish time?*  
     *A:* Sorting by start time processes events in chronological order, ensuring all conflicts at time $t$ are detected before assigning resources.
  3. *Q: What heap operation is performed when a room is reused?*  
     *A:* `pop()` the room with the smallest finish time, update its finish time, and `push()` it back.

---

### 3.3 Huffman Coding
* **Problem Solved:** Optimal lossless compression of library manuscript abstracts.
* **Core Idea:** Bottom-up tree construction: merge the two lowest-frequency subtrees iteratively using a MinHeap.
* **Hand-Traceable Example:** Characters with frequencies: `A: 5, B: 9, C: 12, D: 13, E: 16, F: 45`
  1. Merge `A(5)` and `B(9)` $\to$ `T1(14)`.
  2. Merge `C(12)` and `D(13)` $\to$ `T2(25)`.
  3. Heap has: `T1(14), E(16), T2(25), F(45)`. Merge `T1(14)` and `E(16)` $\to$ `T3(30)`.
  4. Merge `T2(25)` and `T3(30)` $\to$ `T4(55)`.
  5. Merge `F(45)` and `T4(55)` $\to$ `Root(100)`.
  Codes: `F: 0`, `C: 100`, `D: 101`, `A: 1100`, `B: 1101`, `E: 111`.
* **Proof of Correctness:**
  - *Lemma (Greedy Choice):* The two characters $x, y$ with lowest frequencies appear as sibling leaves of maximum depth in some optimal prefix code.
  - *Lemma (Optimal Substructure):* Replacing $x, y$ with a merged character $z$ of frequency $f(x) + f(y)$ reduces the cost of the optimal tree by $f(x) + f(y)$.
* **Complexity:** Time: $O(k \log k)$ where $k = |\Sigma|$ (alphabet size). Space: $O(k)$ for tree.
* **Failure Cases:** Equal character frequencies (cannot achieve compression below $\lceil \log_2 k \rceil$ bits).
* **3 Likely Viva Questions & Answers:**
  1. *Q: What is a prefix-free code?*  
     *A:* A code where no codeword is a proper prefix of any other codeword, allowing instantaneous decoding without delimiters.
  2. *Q: How does Huffman compare with Shannon-Fano coding?*  
     *A:* Huffman is bottom-up and guaranteed optimal; Shannon-Fano is top-down and can be suboptimal.
  3. *Q: How is the compressed text decoded?*  
     *A:* Start at the root; traverse left for '0' and right for '1'. When a leaf is reached, emit the character and return to root.

---

### 3.4 Dijkstra's Algorithm
* **Problem Solved:** Single-source shortest path in citation and routing networks with non-negative edge weights.
* **Core Idea:** Maintain tentative distances from source. Repeatedly extract the unvisited vertex with smallest tentative distance and relax all its outgoing edges.
* **Hand-Traceable Example:** 3 vertices: $0 \to 1 (w=4), 0 \to 2 (w=2), 2 \to 1 (w=1)$. Source = 0.
  1. $dist = [0, \infty, \infty]$. MinHeap: `(0, 0)`.
  2. Extract 0 ($d=0$). Relax $0 \to 1 \implies dist[1] = 4$; relax $0 \to 2 \implies dist[2] = 2$.
     Heap: `(2, 2), (4, 1)`.
  3. Extract 2 ($d=2$). Relax $2 \to 1 \implies dist[1] = \min(4, 2 + 1) = 3$.
     Heap: `(3, 1), (4, 1)`.
  4. Extract 1 ($d=3$). Outgoing edges relaxed.
  5. Pop stale entry `(4, 1)`: discarded since $4 > dist[1]$.
  Final distances: `0: 0, 1: 3, 2: 2`.
* **Proof of Correctness:** Induction on visited set $S$: when vertex $u$ is extracted from the heap, $dist[u]$ is the exact shortest distance from source. Any alternate unvisited path from source to $u$ must cross from $S$ to $V \setminus S$ via some edge $(x, y)$, where $dist[x] + w(x, y) \ge dist[u]$ because edge weights are non-negative.
* **Complexity:** Time: $O((V + E) \log V)$ with binary MinHeap. Space: $O(V + E)$.
* **Failure Cases:** **Negative edge weights** (can cause infinite loops or incorrect shortest paths).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why does Dijkstra fail on negative edge weights?*  
     *A:* The greedy assumption fails: once a vertex is marked visited, Dijkstra assumes its shortest path is finalized, which a negative edge encountered later can invalidate.
  2. *Q: What data structure achieves $O(E + V \log V)$ time?*  
     *A:* Fibonacci Heap (amortized $O(1)$ decrease-key).
  3. *Q: How are predecessors stored for path reconstruction?*  
     *A:* An array $pred[v] = u$, updated whenever $dist[u] + w(u, v) < dist[v]$.

---

### 3.5 Prim's Algorithm
* **Problem Solved:** Constructing the minimum-cost branch courier network connecting all library buildings.
* **Core Idea:** Grow a single connected tree from vertex 0, greedily adding the cheapest edge connecting a tree vertex to a non-tree vertex via a MinHeap.
* **Hand-Traceable Example:** 4 vertices: $0-1(1), 1-2(2), 0-2(3), 2-3(4)$. Root = 0.
  1. $key = [0, \infty, \infty, \infty]$. Heap: `(0, 0)`.
  2. Extract 0. Neighbors: $1 (w=1), 2 (w=3)$. $key[1]=1, key[2]=3$.
  3. Extract 1 ($w=1$). Edge $(0, 1)$ added. Neighbor $2: w(1, 2) = 2 < key[2](3) \implies key[2]=2$.
  4. Extract 2 ($w=2$). Edge $(1, 2)$ added. Neighbor $3: w(2, 3) = 4 \implies key[3]=4$.
  5. Extract 3 ($w=4$). Edge $(2, 3)$ added.
  Total MST Weight: $1 + 2 + 4 = 7$.
* **Proof of Correctness (Cut Property):** Let $S$ be the current tree vertices. The cut $(S, V \setminus S)$ partitions the graph. By the Cut Property, the minimum-weight edge crossing $(S, V \setminus S)$ must belong to the MST.
* **Complexity:** Time: $O((V + E) \log V)$ with binary heap. Space: $O(V + E)$.
* **Failure Cases:** Disconnected graphs (yields a spanning forest; detected if $< V$ vertices added).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Prim vs Kruskal: when to use which?*  
     *A:* Prim is faster on dense graphs ($E \approx V^2$); Kruskal is faster on sparse graphs ($E \approx V$).
  2. *Q: Does Prim work with negative edge weights?*  
     *A:* Yes! MST algorithms depend only on relative edge ordering, not non-negativity.
  3. *Q: Can the MST be non-unique?*  
     *A:* Yes, if multiple edges have identical weights. If all edge weights are strictly distinct, the MST is unique.

---

### 3.6 Kruskal's Algorithm (with Union-Find)
* **Problem Solved:** Minimum-cost spanning tree using edge-centric sorting and disjoint-set cycle prevention.
* **Core Idea:** Sort all edges ascending by weight. Iterate through edges, adding $(u, v)$ to MST if and only if $u$ and $v$ belong to different components (verified via Union-Find).
* **Hand-Traceable Example:** Edges sorted: $E_1(0-1, w=1), E_2(1-2, w=2), E_3(0-2, w=3), E_4(2-3, w=4)$.
  1. $E_1(0, 1, 1)$: $find(0) \ne find(1) \implies unite(0, 1)$, add $E_1$. MST weight = 1.
  2. $E_2(1, 2, 2)$: $find(1) \ne find(2) \implies unite(1, 2)$, add $E_2$. MST weight = 3.
  3. $E_3(0, 2, 3)$: $find(0) == find(2)$ (same component!) $\implies$ Discard (prevents cycle).
  4. $E_4(2, 3, 4)$: $find(2) \ne find(3) \implies unite(2, 3)$, add $E_4$. MST weight = 7.
  Done: $V-1 = 3$ edges added.
* **Proof of Correctness:** Follows from the Cut Property. Kruskal adds the lightest edge that connects two previously unconnected components.
* **Complexity:**
  - Sorting edges: $O(E \log E)$.
  - Union-Find: $O(E \cdot \alpha(V))$ where $\alpha$ is the inverse Ackermann function ($\alpha(V) \le 4$).
  - Overall Time: $O(E \log E)$. Space: $O(V)$ for disjoint sets.
* **Failure Cases:** Disconnected graphs (terminates with fewer than $V-1$ edges).
* **3 Likely Viva Questions & Answers:**
  1. *Q: What are Path Compression and Union by Rank?*  
     *A:* Path compression makes every visited node point directly to root during `find()`. Union by rank attaches the shorter tree under the taller root during `unite()`.
  2. *Q: Why is $O(E \log E) = O(E \log V)$?*  
     *A:* Since $E \le V^2$, $\log E \le \log(V^2) = 2 \log V = O(\log V)$.
  3. *Q: How does Kruskal detect cycles?*  
     *A:* If `find(u) == find(v)`, both endpoints already have a path between them; adding $(u, v)$ would form a cycle.

---

## 4. Dynamic Programming

### 4.1 0/1 Knapsack (Acquisitions under Budget)
* **Problem Solved:** Selecting the most valuable set of book acquisitions under a fixed budgetary capacity $W$.
* **Core Idea:** Subproblem: $dp[i][w] = \max$ value using a subset of first $i$ items with capacity $w$.
  $$dp[i][w] = \begin{cases} dp[i-1][w] & \text{if } w_i > w \\ \max(dp[i-1][w], dp[i-1][w - w_i] + v_i) & \text{if } w_i \le w \end{cases}$$
* **Hand-Traceable Example:** Items: $I_1(w=2, v=3), I_2(w=3, v=4), I_3(w=4, v=5)$. Capacity $W = 5$.
  - Row 0: all 0.
  - Row 1 ($w_1=2, v_1=3$): $w < 2 \to 0$; $w \ge 2 \to 3$.
  - Row 2 ($w_2=3, v_2=4$): $w=0,1 \to 0; w=2 \to 3; w=3 \to 4; w=4 \to 4; w=5 \to \max(3, dp[1][2] + 4) = 7$.
  - Row 3 ($w_3=4, v_3=5$): $w=5 \to \max(7, dp[2][1] + 5) = 7$.
  Max Value: 7 (Items 1 and 2).
* **Proof of Correctness (Optimal Substructure):** An optimal solution either contains item $i$ or does not. If it contains item $i$, the remaining items must form an optimal solution for capacity $w - w_i$.
* **Complexity:** Time: $O(n W)$. Space: $O(n W)$ (or $O(W)$ if only value is needed).
* **Failure Cases:** Real-valued (fractional) weights or exponential capacity ($W = 10^{15}$).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why is 0/1 Knapsack NP-complete if DP solves it in $O(nW)$?*  
     *A:* $O(nW)$ is **pseudo-polynomial**. The input size of $W$ is $\log_2 W$ bits, so runtime is exponential in input length: $O(n \cdot 2^k)$.
  2. *Q: How do we reconstruct which items were selected?*  
     *A:* Start at $dp[n][W]$. If $dp[i][w] \ne dp[i-1][w]$, item $i$ was taken; subtract $w_i$ from $w$ and decrement $i$.
  3. *Q: How does Fractional Knapsack differ?*  
     *A:* Fractional knapsack allows taking parts of items and is solved greedily by value/weight ratio in $O(n \log n)$.

---

### 4.2 Matrix Chain Multiplication
* **Problem Solved:** Determining the optimal multiplication order of multi-matrix co-citation pipelines to minimize scalar multiplications.
* **Core Idea:** Define $m[i][j]$ as minimum multiplications to compute $A_i \dots A_j$:
  $$m[i][j] = \min_{i \le k < j} \{ m[i][k] + m[k+1][j] + p_{i-1} p_k p_j \}$$
* **Hand-Traceable Example:** Matrices $A_1(10 \times 100), A_2(100 \times 5), A_3(5 \times 50)$. Dimensions: `[10, 100, 5, 50]`.
  - Length 2:
    $m[1][2] = 10 \times 100 \times 5 = 5000$.
    $m[2][3] = 100 \times 5 \times 50 = 25000$.
  - Length 3:
    $k=1: m[1][1] + m[2][3] + 10 \times 100 \times 50 = 0 + 25000 + 50000 = 75000$.
    $k=2: m[1][2] + m[3][3] + 10 \times 5 \times 50 = 5000 + 0 + 2500 = 7500$.
  Minimum cost = 7500 with split $k=2 \implies ((A_1 \times A_2) \times A_3)$.
* **Complexity:** Time: $O(n^3)$. Space: $O(n^2)$ for cost and split tables.
* **Failure Cases:** Incompatible matrix dimensions ($p.size() < 2$).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why does multiplying in different orders change the operation count?*  
     *A:* Matrix multiplication is associative ($(AB)C = A(BC)$) but not commutative. Multiplying $(10 \times 100)$ by $(100 \times 5)$ produces a tiny $(10 \times 5)$ matrix, dramatically reducing subsequent inner dimensions.
  2. *Q: Why is brute-force evaluation impossible?*  
     *A:* The number of parenthesizations is given by the Catalan numbers $C_{n-1} = \Omega(4^n / n^{3/2})$.
  3. *Q: How does the split table $s[i][j]$ reconstruct parenthesization?*  
     *A:* Recursively print `(` + `print(i, s[i][j])` + ` x ` + `print(s[i][j]+1, j)` + `)`.

---

### 4.3 Longest Common Subsequence (LCS)
* **Problem Solved:** Measuring similarity between user reading histories or document revisions.
* **Core Idea:**
  $$dp[i][j] = \begin{cases} dp[i-1][j-1] + 1 & \text{if } X[i-1] == Y[j-1] \\ \max(dp[i-1][j], dp[i][j-1]) & \text{if } X[i-1] \ne Y[j-1] \end{cases}$$
* **Hand-Traceable Example:** $X = \text{"ABC"}$, $Y = \text{"AC"}$
  - $dp[1][1]$ ('A'=='A') = $0 + 1 = 1$.
  - $dp[2][1]$ ('B'!='A') = $\max(dp[1][1], dp[2][0]) = 1$.
  - $dp[3][2]$ ('C'=='C') = $dp[2][1] + 1 = 1 + 1 = 2$.
  LCS = "AC", Length = 2.
* **Complexity:** Time: $O(m n)$. Space: $O(m n)$.
* **Failure Cases:** Substring vs Subsequence confusion (subsequences need not be contiguous).
* **3 Likely Viva Questions & Answers:**
  1. *Q: What is the difference between Substring and Subsequence?*  
     *A:* Substrings must be contiguous in the original string; subsequences preserve relative order but need not be contiguous.
  2. *Q: Can space be reduced to $O(\min(m, n))$?*  
     *A:* Yes, if only the length is needed, by maintaining only the current and previous row.
  3. *Q: How is LCS related to Edit Distance?*  
     *A:* Edit distance with insertions and deletions only is equal to $(m + n) - 2 \cdot \text{LCS}(X, Y)$.

---

### 4.4 Optimal Binary Search Tree (OBST)
* **Problem Solved:** Organizing library search keywords into a BST that minimizes the expected search cost given query access frequencies.
* **Core Idea:** Dynamic programming over subtrees:
  $$e[i][j] = \min_{i \le r \le j} \{ e[i][r-1] + e[r+1][j] + w[i][j] \}$$
  where $w[i][j] = \sum_{l=i}^j p_l + \sum_{l=i-1}^j q_l$.
* **Hand-Traceable Example:** 3 keys $k_1, k_2, k_3$ with probabilities $p$, dummy keys $q$.
  Subtrees of length 1 (single key $k_i$) evaluate cost $e[i][i] = q_{i-1} + q_i + w[i][i]$.
  Iterating length $L = 1 \dots n$, find optimal root $root[i][j]$ for every sub-range.
* **Complexity:** Time: $O(n^3)$ standard DP (or $O(n^2)$ with Knuth's monotonic root property: $root[i][j-1] \le root[i][j] \le root[i+1][j]$). Space: $O(n^2)$.
* **Failure Cases:** Unsorted keys or unnormalized/negative probabilities.
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why add $w[i][j]$ at each level?*  
     *A:* When a root $r$ is chosen, all nodes in the left and right subtrees are pushed down by 1 level, increasing each node's search depth by 1, which adds exactly $w[i][j]$ to expected cost.
  2. *Q: What are the dummy keys $q_i$?*  
     *A:* They represent unsuccessful searches for keys falling between $k_i$ and $k_{i+1}$.
  3. *Q: What is Knuth's optimization?*  
     *A:* It restricts search for root $r$ to $[root[i][j-1], root[i+1][j]]$, collapsing the innermost loop and reducing total time to $O(n^2)$.

---

### 4.5 Bellman-Ford Algorithm
* **Problem Solved:** Shortest path with arbitrary (including negative) weights and detection of negative-weight cycles.
* **Core Idea:** A simple shortest path contains at most $|V|-1$ edges. Relax all $|E|$ edges $|V|-1$ times. An additional $|V|$-th pass that finds any relaxation proves the existence of a negative cycle.
* **Hand-Traceable Example:** 3 vertices: $0 \to 1 (w=4), 1 \to 2 (w=-2), 0 \to 2 (w=5)$. Source = 0.
  - Init: $dist = [0, \infty, \infty]$.
  - Pass 1:
    Relax $0 \to 1 \implies dist[1] = 4$.
    Relax $0 \to 2 \implies dist[2] = 5$.
    Relax $1 \to 2 \implies dist[2] = \min(5, 4 + (-2)) = 2$.
  - Pass 2: No distances change $\implies$ Converged.
  - Final: `0: 0, 1: 4, 2: 2`.
* **Complexity:** Time: $O(V \cdot E)$. Space: $O(V)$.
* **Failure Cases:** Undirected edges with negative weights (instantly forms a 2-cycle of negative cost).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why relax $|V|-1$ times?*  
     *A:* The shortest path without cycles can have at most $|V|-1$ edges. Each pass guarantees at least one more edge of the shortest path tree is finalized.
  2. *Q: How does it detect negative cycles?*  
     *A:* If $dist[u] + w(u, v) < dist[v]$ on the $|V|$-th iteration, distances can be infinitely decreased, indicating a negative cycle.
  3. *Q: How can runtime be optimized?*  
     *A:* SPFA (Shortest Path Faster Algorithm) uses a queue of updated vertices to skip redundant edge evaluations.

---

### 4.6 Floyd-Warshall Algorithm
* **Problem Solved:** All-Pairs Shortest Path (APSP) across entire inter-branch library network.
* **Core Idea:** Dynamic programming over intermediate vertices $k \in \{0 \dots V-1\}$:
  $$D^{(k)}[i][j] = \min(D^{(k-1)}[i][j], D^{(k-1)}[i][k] + D^{(k-1)}[k][j])$$
* **Hand-Traceable Example:** 3 vertices. $D[0][1]=3, D[1][2]=2, D[0][2]=8$.
  - Step $k=0$: No change.
  - Step $k=1$: Path $0 \to 2$ via 1: $D[0][1] + D[1][2] = 3 + 2 = 5 < D[0][2](8) \implies D[0][2] = 5$.
  - Final distance matrix updated.
* **Complexity:** Time: $\Theta(V^3)$. Space: $\Theta(V^2)$.
* **Failure Cases:** Negative cycles (detected if any diagonal entry $D[i][i] < 0$).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why must the outer loop be $k$ (intermediate vertex)?*  
     *A:* $D[i][j]$ must build upon paths using vertices $\{0 \dots k-1\}$ before introducing vertex $k$.
  2. *Q: How is the actual path reconstructed?*  
     *A:* Maintain a $next[i][j]$ matrix. When $D[i][k] + D[k][j] < D[i][j]$, set $next[i][j] = next[i][k]$.
  3. *Q: Compare Floyd-Warshall vs running Dijkstra $|V|$ times.*  
     *A:* On dense graphs ($E \approx V^2$), $V \times \text{Dijkstra} = O(V^3 \log V)$, whereas Floyd-Warshall is $O(V^3)$ with simpler code and tighter loops.

---

### 4.7 Traveling Salesperson Problem (TSP via Bitmask DP)
* **Problem Solved:** Finding the minimum-cost closed tour for a librarian picking up reserved books across $n \le 15$ stacks.
* **Core Idea:** Held-Karp algorithm. $dp[mask][u] = \min$ cost to visit subset `mask` ending at vertex $u$:
  $$dp[mask \mid (1 \ll v)][v] = \min(dp[mask \mid (1 \ll v)][v], dp[mask][u] + dist[u][v])$$
* **Hand-Traceable Example:** 3 vertices $\{0, 1, 2\}$, symmetric distances $d(0,1)=10, d(1,2)=15, d(0,2)=20$.
  - Start at 0: $dp[001_2][0] = 0$.
  - Transition to 1: $dp[011_2][1] = 0 + 10 = 10$.
  - Transition to 2: $dp[101_2][2] = 0 + 20 = 20$.
  - From 1 to 2: $dp[111_2][2] = dp[011_2][1] + 15 = 25$.
  - From 2 to 1: $dp[111_2][1] = dp[101_2][2] + 15 = 35$.
  - Return to 0: $\min(dp[111_2][1] + 10, dp[111_2][2] + 20) = \min(35+10, 25+20) = 45$.
* **Complexity:** Time: $O(n^2 2^n)$. Space: $O(n 2^n)$.
* **Failure Cases:** $n > 20$ (the table has n x 2^n entries, so memory grows exponentially).
* **3 Likely Viva Questions & Answers:**
  1. *Q: What does the bitmask represent?*  
     *A:* The $i$-th bit of `mask` is 1 if vertex $i$ has been visited, 0 otherwise.
  2. *Q: How does $O(n^2 2^n)$ compare to $(n-1)!$?*  
     *A:* For $n=15$, $14! \approx 8.7 \times 10^{10}$ operations, whereas $15^2 \times 2^{15} \approx 7.37 \times 10^6$ operations (about 10,000x faster).
  3. *Q: How is the tour reconstructed?*  
     *A:* By storing `parent[mask][u]` recording which predecessor vertex generated the minimum cost.

---

## 5. Backtracking & Branch-and-Bound

### 5.1 N-Queens Problem
* **Problem Solved:** Placing $N$ non-attacking library security sentries on an $N \times N$ facility grid.
* **Core Idea:** Place queens row by row. At each row, test columns; backtrack if column, main diagonal ($r - c$), or anti-diagonal ($r + c$) is already occupied.
* **Hand-Traceable Example:** $N=4$
  - Row 0: Place at Col 1 $\implies [1]$.
  - Row 1: Col 0 attacked (diag); Col 1 attacked; Col 2 attacked (diag); Col 3 safe $\implies [1, 3]$.
  - Row 2: Col 0 safe $\implies [1, 3, 0]$.
  - Row 3: Col 2 safe $\implies [1, 3, 0, 2]$. Valid solution found!
* **Complexity:** Time: $O(N!)$ worst case. Space: $O(N)$ state vectors.
* **Failure Cases:** $N=2$ and $N=3$ have no solutions.
* **3 Likely Viva Questions & Answers:**
  1. *Q: How are diagonals tracked in $O(1)$ time?*  
     *A:* Using two boolean arrays: `diag1[row - col + N - 1]` and `diag2[row + col]`.
  2. *Q: How many solutions exist for $N=8$?*  
     *A:* Exactly 92 distinct solutions (12 fundamental symmetry classes).
  3. *Q: Backtracking vs Brute Force?*  
     *A:* Brute force tests $\binom{N^2}{N} \approx 4.4 \times 10^9$ positions for $N=8$, whereas backtracking checks only safe configurations, examining fewer than 15,000 nodes.

---

### 5.2 Subset Sum Problem
* **Problem Solved:** Verifying if an exact requested budget or credit balance can be settled using existing catalog voucher denominations.
* **Core Idea:** State-space tree with two bounding functions: prune branch if $currentSum + S[i] > target$ or if $currentSum + remainingSum < target$.
* **Hand-Traceable Example:** $S = [2, 3, 4]$, Target = 7.
  1. Include 2: sum = 2, remaining = 7.
  2. Include 3: sum = 5, remaining = 4.
  3. Include 4: $5 + 4 = 9 > 7 \implies$ Prune!
  4. Exclude 3: sum = 2. Include 4: sum = 6. Remaining = 0. $6 < 7 \implies$ Prune!
  5. Exclude 2: Include 3, Include 4 $\implies 3 + 4 = 7$. Solution: $\{3, 4\}$.
* **Complexity:** Worst: $O(2^n)$. Pruned: orders of magnitude faster. Space: $O(n)$.
* **Failure Cases:** Sum of all elements $< target$.
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why pre-sort elements ascending?*  
     *A:* If $currentSum + S[i] > target$, then all subsequent elements $S[j] \ge S[i]$ will also exceed target, pruning the entire remaining loop.
  2. *Q: Is Subset Sum NP-complete?*  
     *A:* Yes, it is one of Karp's 21 NP-complete problems.
  3. *Q: What is the bounding condition?*  
     *A:* $currentSum + remainingSum \ge target$.

---

### 5.3 0/1 Knapsack Branch-and-Bound (FIFO, LIFO, Least-Cost)
* **Problem Solved:** Optimal resource allocation under budget without filling full $O(nW)$ dynamic programming tables.
* **Core Idea:** Fractional knapsack relaxation computes an upper bound for each state node. Subtrees whose bound $\le$ best known value are pruned.
  - **FIFO:** Explores states using a Queue (Breadth-First).
  - **LIFO:** Explores states using a Stack (Depth-First).
  - **Least-Cost (LCBB):** Explores states using a Max-Heap ordered by upper bound (Best-First).
* **Hand-Traceable Example:** Items sorted by $v/w$: $I_1(w=2, v=40), I_2(w=3, v=50), I_3(w=5, v=95)$. Capacity $W = 7$.
  - Root: Bound = $40 + 50 + (2/5 \times 95) = 128$.
  - Node A (Take $I_1$): $w=2, v=40$, Bound = 128.
  - Node B (Drop $I_1$): $w=0, v=0$, Bound = $50 + (4/5 \times 95) = 126$.
  - LCBB expands Node A first because $128 > 126$.
* **Complexity:** Time: $O(2^n)$ worst case, but LCBB visits minimal nodes. Space: $O(2^n)$ in worst case heap.
* **Failure Cases:** Identical value/weight ratios prevent pruning.
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why does LCBB explore fewer nodes than FIFO?*  
     *A:* LCBB always expands the most promising node first, quickly reaching near-optimal integer solutions that raise `bestValue` early and prune subsequent branches.
  2. *Q: How is the upper bound calculated?*  
     *A:* Greedily pack whole items; add the fractional value of the first item that exceeds capacity.
  3. *Q: Why is the fractional relaxation a valid upper bound?*  
     *A:* Allowing fractional item choices expands the feasible solution set, guaranteeing the fractional optimum $\ge$ integer optimum.

---

## 6. Flow and Matching

### 6.1 Edmonds-Karp Maximum Flow
* **Problem Solved:** Maximizing digital document download bandwidth across the inter-branch network.
* **Core Idea:** Ford-Fulkerson method using BFS to select the augmenting path with the fewest edges in the residual graph.
* **Hand-Traceable Example:** Network $s(0) \to 1(c=3), s \to 2(c=2), 1 \to t(3)(c=2), 2 \to t(c=3)$.
  1. BFS finds path $0 \to 1 \to 3$. Bottleneck = $\min(3, 2) = 2$.
     Augment 2 units. Residual: $0 \to 1 (1), 1 \to 3 (0), 3 \to 1 (2)$.
  2. BFS finds path $0 \to 2 \to 3$. Bottleneck = $\min(2, 3) = 2$.
     Augment 2 units. Residual: $0 \to 2 (0), 2 \to 3 (1), 3 \to 2 (2)$.
  3. No augmenting path from $0$ to $3$ in residual graph.
  Total Max Flow = $2 + 2 = 4$.
* **Proof of Correctness (Max-Flow Min-Cut Theorem):** The value of max flow equals the capacity of the minimum $s-t$ cut. When no augmenting path exists, the set of vertices $S$ reachable from $s$ in $G_f$ defines a cut $(S, T)$ whose residual capacity is 0, proving flow is maximal.
* **Complexity:** Time: $O(V \cdot E^2)$ strictly polynomial. Space: $O(V^2)$.
* **Failure Cases:** Disconnected source and sink (flow = 0).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why use BFS instead of DFS?*  
     *A:* Generic Ford-Fulkerson with DFS can take $O(E \cdot |f|)$ time and fail on irrational capacities. Edmonds-Karp with BFS guarantees polynomial $O(V E^2)$ convergence.
  2. *Q: What is a residual graph?*  
     *A:* A graph containing residual capacity edges $c_f(u, v) = c(u, v) - f(u, v)$ and reverse edges $c_f(v, u) = f(u, v)$ allowing flow cancellation.
  3. *Q: What is the Cut Property of flows?*  
     *A:* Net flow across any cut $(S, T)$ equals total flow from source to sink.

---

### 6.2 Maximum Bipartite Matching via Max Flow
* **Problem Solved:** Optimally assigning library readers to reserved physical book copies.
* **Core Idea:** Reduction to Max Flow: create super-source $s$ and super-sink $t$. Connect $s \to \text{readers}$ with capacity 1, $\text{copies} \to t$ with capacity 1, and bipartite edges with capacity 1. Max matching = max flow.
* **Hand-Traceable Example:** Readers $\{R_1, R_2\}$, Copies $\{C_1, C_2\}$. Edges: $(R_1, C_1), (R_1, C_2), (R_2, C_1)$.
  Flow network routes 1 unit via $s \to R_1 \to C_2 \to t$ and 1 unit via $s \to R_2 \to C_1 \to t$.
  Total Flow = 2 $\implies$ Matching: $(R_1, C_2)$ and $(R_2, C_1)$.
* **Proof of Correctness (Integrality Theorem):** Since all capacities are integral (1), the max flow found by Edmonds-Karp is strictly integral ($0$ or $1$). Each reader has in-degree 1 and each copy has out-degree 1, guaranteeing disjoint matching pairs.
* **Complexity:** Time: $O(V' \cdot E'^2)$ (in practice $O(V E)$ on unit networks). Space: $O(V^2)$.
* **Failure Cases:** Non-bipartite input graphs.
* **3 Likely Viva Questions & Answers:**
  1. *Q: What is Hall's Marriage Theorem?*  
     *A:* A bipartite graph has a matching covering $X$ iff for every subset $S \subseteq X$, $|N(S)| \ge |S|$.
  2. *Q: How does Hopcroft-Karp algorithm compare?*  
     *A:* Hopcroft-Karp finds multiple shortest augmenting paths simultaneously in $O(E \sqrt{V})$ time.
  3. *Q: Why are unit edge capacities crucial?*  
     *A:* Unit capacities prevent a single reader or book copy from being matched more than once.

---

## 7. Number Theory

### 7.1 Chinese Remainder Theorem (with Extended Euclidean)
* **Problem Solved:** Synchronizing periodic multi-branch maintenance cycles and shelf audit schedules.
* **Core Idea:**
  Given simultaneous congruences $x \equiv a_i \pmod{m_i}$ with pairwise coprime moduli:
  $$M = \prod m_i, \quad M_i = \frac{M}{m_i}, \quad y_i = M_i^{-1} \pmod{m_i}$$
  $$x = \left(\sum a_i M_i y_i\right) \pmod M$$
* **Hand-Traceable Example:**
  $x \equiv 2 \pmod 3$, $x \equiv 3 \pmod 5$, $x \equiv 2 \pmod 7$.
  1. $M = 3 \times 5 \times 7 = 105$.
  2. $M_1 = 35, M_2 = 21, M_3 = 15$.
  3. Modular inverses:
     - $35 \equiv 2 \pmod 3 \implies 2 y_1 \equiv 1 \pmod 3 \implies y_1 = 2$.
     - $21 \equiv 1 \pmod 5 \implies y_2 = 1$.
     - $15 \equiv 1 \pmod 7 \implies y_3 = 1$.
  4. Sum $= 2(35)(2) + 3(21)(1) + 2(15)(1) = 140 + 63 + 30 = 233$.
  5. $x = 233 \pmod{105} = 23$.
  Check: $23 = 7(3)+2$, $23 = 4(5)+3$, $23 = 3(7)+2$. Correct!
* **Proof of Correctness:**
  For each $j \ne i$, $M_j$ contains factor $m_i$, so $M_j \equiv 0 \pmod{m_i}$.
  For $j = i$, $M_i y_i \equiv 1 \pmod{m_i}$.
  Thus, $x \equiv a_i(1) + \sum_{j \ne i} 0 \equiv a_i \pmod{m_i}$.
* **Complexity:** Time: $O(k \log M)$. Space: $O(1)$.
* **Failure Cases:** Non-coprime moduli ($\gcd(m_i, m_j) \ne 1$).
* **3 Likely Viva Questions & Answers:**
  1. *Q: What does Extended Euclidean Algorithm return?*  
     *A:* $\gcd(a, b)$ and integers $x, y$ such that $a \cdot x + b \cdot y = \gcd(a, b)$ (Bézout's identity).
  2. *Q: When does a modular inverse exist?*  
     *A:* $a^{-1} \pmod m$ exists if and only if $\gcd(a, m) = 1$.
  3. *Q: How does CRT speed up large RSA computations?*  
     *A:* Decryption $c^d \pmod n$ is split into $c^d \pmod p$ and $c^d \pmod q$ ($n = pq$), which are about 4x faster (varies per run), then combined via CRT.

---

## 8. String Matching

### 8.1 Naive String Matching
* **Problem Solved:** Direct exact-substring keyword search in document text.
* **Core Idea:** Check all shifts $s \in [0 \dots n-m]$ with character-by-character comparison.
* **Hand-Traceable Example:** $T = \text{"ABABC"}, P = \text{"ABC"}$.
  - Shift 0: 'A'=='A', 'B'=='B', 'A'!='C' $\implies$ Mismatch.
  - Shift 1: 'B'!='A' $\implies$ Mismatch.
  - Shift 2: 'A'=='A', 'B'=='B', 'C'=='C' $\implies$ Match at index 2!
* **Complexity:** Best: $O(n)$. Worst: $O((n - m + 1) m)$ (e.g. $T = a^n, P = a^{m-1}b$). Space: $O(1)$.
* **Failure Cases:** Extreme slowdown on repetitive adversarial text.
* **3 Likely Viva Questions & Answers:**
  1. *Q: What is the worst-case comparison count?*  
     *A:* $(n - m + 1) \cdot m$ comparisons.
  2. *Q: Does Naive require any preprocessing?*  
     *A:* Zero preprocessing time and zero extra memory.
  3. *Q: When is Naive acceptable?*  
     *A:* In natural language search where mismatches typically occur on the first 1-2 characters.

---

### 8.2 Rabin-Karp Algorithm
* **Problem Solved:** Fast multi-pattern and single-pattern matching using rolling hash signatures.
* **Core Idea:** Treat strings as numbers in base $d \pmod q$. Slide window of size $m$ and update hash in $O(1)$ time:
  $$t_{s+1} = (d(t_s - T[s] h) + T[s+m]) \pmod q$$
* **Hand-Traceable Example:** Base $d=10$, $q=13$, $P = \text{"26"}$. Hash $p = 26 \pmod{13} = 0$.
  $T = \text{"3126"}$.
  - Window 0 ("31"): $31 \pmod{13} = 5 \ne 0$.
  - Window 1 ("12"): $12 \pmod{13} = 12 \ne 0$.
  - Window 2 ("26"): $26 \pmod{13} = 0 == p$. Compare chars $\implies$ Match at index 2!
* **Complexity:** Average: $O(n + m)$. Worst: $O(n \cdot m)$ on frequent hash collisions. Space: $O(1)$.
* **Failure Cases:** Spurious collisions caused by small or poorly chosen modulus.
* **3 Likely Viva Questions & Answers:**
  1. *Q: What is a spurious hit?*  
     *A:* When pattern hash equals text window hash, but character-by-character verification fails due to hash collision.
  2. *Q: Why is Rabin-Karp well suited for plagiarism detection?*  
     *A:* It extends naturally to multi-pattern search by storing pattern hashes in a hash set.
  3. *Q: How is the rolling hash computed in $O(1)$?*  
     *A:* Subtract the leading character multiplied by $d^{m-1}$, multiply by $d$, and add the new trailing character.

---

### 8.3 Finite Automaton String Matching
* **Problem Solved:** Strictly linear $O(n)$ search without backtracking the text pointer.
* **Core Idea:** Build a Deterministic Finite Automaton (DFA) where state $q$ represents the length of the longest prefix of $P$ that is a suffix of $T$ processed so far.
* **Hand-Traceable Example:** Pattern $P = \text{"ABAB"}$.
  - State 0 on 'A' $\to$ 1; on 'B' $\to$ 0.
  - State 1 on 'A' $\to$ 1; on 'B' $\to$ 2.
  - State 2 on 'A' $\to$ 3; on 'B' $\to$ 0.
  - State 3 on 'A' $\to$ 1; on 'B' $\to$ 4 (Accepting state).
* **Complexity:** Matching Time: $\Theta(n)$. Preprocessing: $O(m^3 |\Sigma|)$ or $O(m |\Sigma|)$. Space: $O(m |\Sigma|)$.
* **Failure Cases:** Enormous alphabet size $|\Sigma|$ (e.g. Unicode 65,536 characters) explodes transition table memory.
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why is the text scan strictly $O(n)$?*  
     *A:* Exactly one table lookup $q = \delta[q][c]$ is executed for each character of the text.
  2. *Q: What is the start and accepting state?*  
     *A:* State 0 is start; state $m$ is accepting.
  3. *Q: How does DFA compare to KMP?*  
     *A:* KMP computes transitions implicitly in $O(m)$ space, avoiding the $O(m |\Sigma|)$ transition matrix.

---

### 8.4 Knuth-Morris-Pratt (KMP) Algorithm
* **Problem Solved:** Guaranteed $O(n + m)$ string matching with minimal $O(m)$ auxiliary space.
* **Core Idea:** Precompute prefix table $\pi[0..m-1]$ where $\pi[q]$ is the length of the longest proper prefix of $P[0..q]$ that is also a suffix of $P[0..q]$. On mismatch, fall back along $\pi$ table without moving the text pointer backward.
* **Hand-Traceable Example:** Pattern $P = \text{"ABABC"}$.
  - $\pi[0]$ ("A") = 0.
  - $\pi[1]$ ("AB") = 0.
  - $\pi[2]$ ("ABA") = 1 ("A").
  - $\pi[3]$ ("ABAB") = 2 ("AB").
  - $\pi[4]$ ("ABABC") = 0.
  Table $\pi = [0, 0, 1, 2, 0]$.
  If mismatch occurs at index 4 ('C') on text "ABABD", shift pattern directly to $\pi[3] = 2$, keeping pointer aligned with "AB".
* **Complexity:** Preprocessing: $O(m)$. Matching: $O(n)$. Total Time: $\Theta(n + m)$. Space: $O(m)$ for $\pi$ array.
* **Failure Cases:** Empty pattern (handled safely by early exit).
* **3 Likely Viva Questions & Answers:**
  1. *Q: Why does KMP never backtrack the text pointer?*  
     *A:* Because the $\pi$ table encodes the exact overlap between the pattern's prefix and suffix, shifting the pattern to the next valid alignment directly.
  2. *Q: What is the formal definition of $\pi[q]$?*  
     *A:* $\pi[q] = \max \{ k \mid k < q+1 \text{ and } P[0..k-1] \sqsupset P[0..q] \}$ (longest proper prefix that is also a suffix).
  3. *Q: What is the amortized cost per character in KMP?*  
     *A:* While the inner `while` loop can fall back multiple times, the pointer $q$ increases by at most 1 per text character. Since $q$ can decrease at most as many times as it increases, total amortized comparisons across the entire scan are $\le 2n$.
