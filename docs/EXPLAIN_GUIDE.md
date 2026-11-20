# DLS Engine: Tonight's Oral Defense & Viva Study Guide

This document is your complete, zero-fluff study guide for tomorrow's 8-minute presentation and 4-minute viva examination.

---

## 1. How the Program Runs (In 5 Lines)

```text
1. main (app/main.cpp) parses flags (--scenario 1/2/3, --all, or menu).
2. DataGenerator creates realistic synthetic library data using deterministic seed 42.
3. Algorithms execute from the core library (libdls_core.a) without third-party tools.
4. Metrics reference (&) records wall-clock milliseconds, key comparisons, and aux space.
5. main prints formatted tables, comparisons, and telemetry for the examiner.
```

---

## 2. The 5 Core Algorithms: Deep Dive & Live Traces

### 2.1 Binary Search (Iterative & Recursive)
* **File & Function:** `include/dls/search.inl` (`binarySearchIterative`), `src/search/search.cpp` (`binarySearchRecursive`)
* **Step-by-Step Trace:**
  * **Input:** Array `[2, 5, 8, 12, 16, 23, 38]`, Target = `23`. Indices $0 \dots 6$.
  * **Step 1:** $low = 0, high = 6 \implies mid = 0 + (6 - 0) / 2 = 3$. Value is `arr[3] = 12`.
    * Compare $12 < 23$. Target is strictly greater. Set $low = mid + 1 = 4$.
  * **Step 2:** $low = 4, high = 6 \implies mid = 4 + (6 - 4) / 2 = 5$. Value is `arr[5] = 23`.
    * Compare `arr[5] == 23`. Target found!
  * **Output:** Returns index `5` in exactly **2 comparisons**.
* **Complexity:**
  * **Time:** Best = $O(1)$ (target at root/mid), Average = $O(\log n)$, Worst = $O(\log n)$.
  * **Space:** Iterative = $O(1)$ auxiliary space. Recursive = $O(\log n)$ call stack frames.
* **3 Likely Viva Questions:**
  1. *Q: Why write `mid = low + (high - low) / 2` instead of `(low + high) / 2`?*  
     *A:* If $low + high > 2^{31} - 1$, normal addition causes 32-bit signed integer overflow into negative numbers. Subtraction prevents overflow.
  2. *Q: What condition must hold before binary search can run?*  
     *A:* The input array must be sorted in ascending order. If unsorted, the decision to discard half the array is invalid.
  3. *Q: What is the recurrence relation for recursive binary search?*  
     *A:* $T(n) = T(n/2) + 1$, which solves to $O(\log n)$ by Master Theorem (Case 2: $a=1, b=2, d=0$).
* **2 Likely Live Code Changes:**
  1. *Examiner asks: "Change it to return the first occurrence if there are duplicate IDs."*  
     *Fix:* When `arr[mid] == target`, do not return immediately. Save `result = mid` and continue searching left: `high = mid - 1`.
  2. *Examiner asks: "What happens if we write `low = mid` instead of `low = mid + 1`?"*  
     *Fix:* When $high - low = 1$ and `arr[mid] < target`, $mid$ equals $low$, causing an infinite loop. Always use `low = mid + 1`.

---

### 2.2 Quick Sort (Partition & Median-of-Three)
* **File & Function:** `include/dls/sort.inl` (`partitionLomuto`, `quickSortInternal`, `selectMedianOfThree`)
* **Step-by-Step Trace:**
  * **Input:** `[5, 2, 9, 1, 7]`, Last-Element Pivot = `7`. Indices $0 \dots 4$.
  * **Initialization:** Pivot = `7`. Boundary pointer $i = low - 1 = -1$.
  * **Loop $j = 0 \dots 3$:**
    * $j = 0$: `arr[0] = 5 \le 7$. Increment $i = 0$. Swap `arr[0]` with `arr[0]` $\implies [5, 2, 9, 1, 7]$.
    * $j = 1$: `arr[1] = 2 \le 7$. Increment $i = 1$. Swap `arr[1]` with `arr[1]` $\implies [5, 2, 9, 1, 7]$.
    * $j = 2$: `arr[2] = 9 > 7$. Skip ($i$ stays $1$).
    * $j = 3$: `arr[3] = 1 \le 7$. Increment $i = 2$. Swap `arr[2] (9)` with `arr[3] (1)` $\implies [5, 2, 1, 9, 7]$.
  * **Final Swap:** Swap pivot `arr[4] (7)` with `arr[i+1] = arr[3] (9)`.
  * **Output Partition:** `[5, 2, 1, 7, 9]`. Pivot `7` is placed at final index `3`. Subarrays `[5, 2, 1]` and `[9]` recurse.
* **Complexity:**
  * **Time:** Best = $O(n \log n)$, Average = $O(n \log n)$, Worst = $O(n^2)$ (extreme unbalanced partitions).
  * **Space:** $O(\log n)$ average recursion stack frames; operates **in-place** with $0$ auxiliary data buffer.
* **3 Likely Viva Questions:**
  1. *Q: How does Median-of-Three prevent worst-case $O(n^2)$ time on pre-sorted data?*  
     *A:* It inspects first, middle, and last elements and picks their median as pivot. On sorted data, the median is never the extreme minimum or maximum, guaranteeing balanced partitions.
  2. *Q: Why is Quick Sort faster in wall-clock time than Merge Sort if it makes more comparisons?*  
     *A:* Likely due to data movement and cache behaviour: Quick Sort partitions directly in-place, whereas Merge Sort copies whole `Document` structs into and out of an auxiliary buffer.
  3. *Q: How can you guarantee $O(\log n)$ maximum recursion stack depth?*  
     *A:* Tail recursion: always recurse on the smaller partition first, and use a while-loop for the larger partition.
* **2 Likely Live Code Changes:**
  1. *Examiner asks: "Change Lomuto partition to sort in descending order."*  
     *Fix:* In `partitionLomuto`, change `if (metrics.countLess(arr[j], pivot))` to `if (metrics.countGreater(arr[j], pivot))`.
  2. *Examiner asks: "What if pivot index $i$ is initialized to $low$ instead of $low - 1$?"*  
     *Fix:* Elements will be placed one position too far right, overwriting unchecked values or placing the pivot out of bounds.

---

### 2.3 Merge Sort (The Merge Step)
* **File & Function:** `include/dls/sort.inl` (`merge`, `mergeSortInternal`)
* **Step-by-Step Trace:**
  * **Input Subarrays:** Left = `[27, 38]`, Right = `[3, 43]`. Pointers $i = 0$ (Left), $j = 0$ (Right), $k = 0$ (Merged).
  * **Step 1:** Compare `Left[0] (27)` and `Right[0] (3)`. Since $3 \le 27$, copy `Right[0]` $\implies Merged = [3]$. Advance $j = 1$.
  * **Step 2:** Compare `Left[0] (27)` and `Right[1] (43)`. Since $27 \le 43$, copy `Left[0]` $\implies Merged = [3, 27]$. Advance $i = 1$.
  * **Step 3:** Compare `Left[1] (38)` and `Right[1] (43)`. Since $38 \le 43$, copy `Left[1]` $\implies Merged = [3, 27, 38]$. Advance $i = 2$.
  * **Step 4:** Left subarray exhausted. Drain remaining Right elements: copy `Right[1] (43)` $\implies Merged = [3, 27, 38, 43]$.
  * **Output:** Fully merged sorted array `[3, 27, 38, 43]`.
* **Complexity:**
  * **Time:** Best = $O(n \log n)$, Average = $O(n \log n)$, Worst = $O(n \log n)$ (guaranteed).
  * **Space:** $O(n)$ auxiliary memory (copies elements into a temporary merge buffer).
* **3 Likely Viva Questions:**
  1. *Q: Is Merge Sort stable? Why?*  
     *A:* Yes. In the merge step, when `Left[i] == Right[j]`, we take `Left[i]` first (`<=`), preserving the relative order of duplicate elements.
  2. *Q: What is the exact recurrence relation?*  
     *A:* $T(n) = 2T(n/2) + \Theta(n)$. By Master Theorem Case 2 ($a=2, b=2, d=1$), $T(n) = \Theta(n \log n)$.
  3. *Q: What is the main drawback of Merge Sort in systems software?*  
     *A:* It requires $O(n)$ extra memory (an auxiliary buffer of size $n$), which causes heavy data copying when elements are large structs.
* **2 Likely Live Code Changes:**
  1. *Examiner asks: "Make Merge Sort unstable with a single character change."*  
     *Fix:* In the merge step, change `if (Left[i] <= Right[j])` to `<`. When values are equal, taking the right element first breaks stability.
  2. *Examiner asks: "Change it to sort descending."*  
     *Fix:* In the merge step, pick the larger element first: `if (Left[i] >= Right[j])`.

---

### 2.4 Knuth-Morris-Pratt / KMP String Matching
* **File & Function:** `src/strmatch/strmatch.cpp` (`computePrefixTable`, `kmpSearch`)
* **Step-by-Step Trace:**
  * **Part A: Prefix Table ($\pi$) for Pattern `P = "ABAB"` (Length $m = 4$):**
    * $\pi[0] = 0$. Length of current matching prefix $k = 0$.
    * $q = 1$: $P[1] = 'B' \ne P[0] = 'A' \implies \pi[1] = 0$.
    * $q = 2$: $P[2] = 'A' == P[0] = 'A' \implies k = 1 \implies \pi[2] = 1$.
    * $q = 3$: $P[3] = 'B' == P[1] = 'B' \implies k = 2 \implies \pi[3] = 2$.
    * **Table Result:** $\pi = [0, 0, 1, 2]$.
  * **Part B: Search in Text `T = "ABABDABAB"`:**
    * Matches $T[0..3] = \text{"ABAB"}$ with $P[0..3]$. **Match found at index 0!**
    * Next shift uses $\pi[3] = 2$. The text pointer never moves backward; pattern pointer resets to index 2 (retaining prefix "AB").
    * Continues matching from $T[4] = 'D' \ne P[2] = 'A'$. Fallback to $\pi[1] = 0$.
    * Matches $T[5..8] = \text{"ABAB"}$ with $P[0..3]$. **Match found at index 5!**
* **Complexity:**
  * **Time:** Best = $O(n)$, Average = $O(n + m)$, Worst = $O(n + m)$ (guaranteed linear time).
  * **Space:** $O(m)$ auxiliary space to store the prefix failure array $\pi$.
* **3 Likely Viva Questions:**
  1. *Q: Why is KMP immune to adversarial strings like $T = "aaaa\dots b"$ and $P = "aaab"$?*  
     *A:* The Naive algorithm resets its text pointer, causing $O(n \cdot m)$ comparisons. KMP advances the text pointer monotonically; on a mismatch, $\pi$ slides the pattern to the longest prefix that is also a suffix in $O(1)$ amortized time.
  2. *Q: What does $\pi[i]$ represent?*  
     *A:* The length of the longest proper prefix of $P[0..i]$ that is also a proper suffix of $P[0..i]$.
  3. *Q: How many times can the text pointer backtrack?*  
     *A:* Zero. The text pointer $i$ strictly increments from $0$ to $n-1$.
* **2 Likely Live Code Changes:**
  1. *Examiner asks: "Change KMP to return only the first match and stop."*  
     *Fix:* When $q == m$, save index $i - m + 1$ and return immediately instead of doing $q = \pi[q-1]$.
  2. *Examiner asks: "What happens if you initialize $\pi[0] = 1$ instead of $0$?"*  
     *Fix:* A single character cannot have a proper prefix equal to a proper suffix. Setting it to 1 would cause an out-of-bounds pointer or infinite self-loop.

---

### 2.5 0/1 Knapsack: DP vs Branch-and-Bound (FIFO, LIFO, LCBB)
* **File & Function:** `src/dp/dp.cpp` (`knapsack01`), `src/bb/bb.cpp` (`knapsackBranchAndBound`, `calculateBound`)
* **Step-by-Step Trace (DP Table):**
  * **Input:** Capacity $W = 5$. Items:
    * Item 1: Weight = 2, Value = 3
    * Item 2: Weight = 3, Value = 4
    * Item 3: Weight = 4, Value = 5
  * **DP Recurrence:** $dp[i][w] = \max(dp[i-1][w], \, dp[i-1][w - w_i] + v_i)$
  * **DP Table ($i \times w$):**
    | Item $i$ | $w=0$ | $w=1$ | $w=2$ | $w=3$ | $w=4$ | $w=5$ |
    |:---:|:---:|:---:|:---:|:---:|:---:|:---:|
    | **0 (None)** | 0 | 0 | 0 | 0 | 0 | 0 |
    | **1 ($w=2, v=3$)** | 0 | 0 | 3 | 3 | 3 | 3 |
    | **2 ($w=3, v=4$)** | 0 | 0 | 3 | 4 | 4 | **7** ($3+4$) |
    | **3 ($w=4, v=5$)** | 0 | 0 | 3 | 4 | 5 | **7** |
  * **Backtracking Result:** Select Item 2 ($w=3, v=4$) + Item 1 ($w=2, v=3$). Total Weight = 5, Maximum Value = **7**.
* **Branch-and-Bound Strategies:**
  * **FIFO (Queue):** Breadth-first search explores levels exhaustively; slow pruning.
  * **LIFO (Stack):** Depth-first search dives deep fast; prunes only after hitting leaves.
  * **Least-Cost / LCBB (Priority Queue):** Always expands the node with the highest fractional relaxation bound. Under budget constraints, it prunes unpromising branches aggressively (e.g. 21 nodes vs 879 for FIFO).
* **Complexity:**
  * **DP:** Time = $O(n W)$ (pseudo-polynomial), Space = $O(n W)$ table.
  * **Branch-and-Bound:** Time = Worst-case $O(2^n)$, Average = pruned substantially; Space = $O(2^n)$ active nodes.
* **3 Likely Viva Questions:**
  1. *Q: Why is $O(nW)$ called pseudo-polynomial time?*  
     *A:* Because $W$ is represented by $k = \log_2 W$ bits. In terms of input length $k$, runtime is $O(n \cdot 2^k)$, which is exponential in input size.
  2. *Q: How does LCBB calculate its upper bound?*  
     *A:* Continuous fractional relaxation: items are sorted by value/weight ratio ($v_i/w_i$). It greedily takes whole items and adds a fractional portion of the next item that fits.
  3. *Q: When can LCBB prune a branch?*  
     *A:* When the node's calculated upper bound is $\le$ the best feasible integer solution found so far.
* **2 Likely Live Code Changes:**
  1. *Examiner asks: "Change the DP table to use $O(W)$ space instead of $O(nW)$."*  
     *Fix:* Maintain a 1D vector `dp[w]` of size $W+1$ and iterate backwards from $W$ down to $w_i$: `dp[w] = max(dp[w], dp[w - w_i] + v_i)`.
  2. *Examiner asks: "What if you accidentally iterate forward from $0$ to $W$ in a 1D DP array?"*  
     *Fix:* It solves the **Unbounded Knapsack** problem (items can be taken repeatedly) because the same item gets added multiple times in the same pass.

---

## 3. One Plain Sentence for Every Other Algorithm in the Project

1. **Maximum Subarray Divide-and-Conquer (`dc.cpp`):** Finds the contiguous reading window with peak visitor surge in $O(n \log n)$ time by recursively splitting turnstile entries and combining across the midpoint.
2. **Kadane's Algorithm (`dc.cpp`):** Finds the same maximum turnstile surge in linear $O(n)$ time and $O(1)$ space by maintaining the running maximum contiguous sum.
3. **Fast Modular Powering (`dc.cpp`):** Computes $a^b \pmod m$ in logarithmic $O(\log b)$ time using recursive repeated squaring for library security token verification.
4. **Celebrity Elimination (`dc.cpp`):** Identifies an authoritative paper cited by all but citing none in $O(n)$ queries using pairwise elimination instead of an $O(n^2)$ matrix scan.
5. **Strassen's Matrix Multiplication (`dc.cpp`):** Multiplies co-citation matrices in sub-cubic $O(n^{2.807})$ time by computing 7 recursive submatrix products instead of 8.
6. **Activity Selection (`greedy.cpp`):** Schedules the maximum number of non-overlapping study sessions in a single reading room in $O(n \log n)$ time by greedily choosing earliest finish time.
7. **Interval Partitioning (`greedy.cpp`):** Determines the minimum number of physical rooms needed for all conflicting study groups in $O(n \log d)$ time using our custom `RoomMinHeap`.
8. **Huffman Coding (`greedy.cpp`):** Compresses textual book abstracts into variable-length prefix-free binary codes in $O(k \log k)$ time using our custom `HuffmanMinHeap`.
9. **Dijkstra's Algorithm (`greedy.cpp`):** Computes the shortest delivery route from a central library hub to all branches with non-negative road weights in $O((V+E)\log V)$ time using `DistMinHeap`.
10. **Prim's Algorithm (`greedy.cpp`):** Constructs a Minimum Spanning Tree connecting all library branches at minimum total cable cost in $O((V+E)\log V)$ time using `DistMinHeap`.
11. **Kruskal's Algorithm (`greedy.cpp`):** Finds the same Minimum Spanning Tree in $O(E \log E)$ time by sorting edges and avoiding cycles with `DisjointSet` path compression and union by rank.
12. **Matrix Chain Multiplication (`dp.cpp`):** Determines the optimal parenthesization order for multi-table catalog queries in $O(n^3)$ time to minimize scalar multiplications.
13. **Longest Common Subsequence / LCS (`dp.cpp`):** Identifies overlapping textual passages between manuscript revisions in $O(m \cdot n)$ time using a 2D dynamic programming grid.
14. **Optimal Binary Search Tree / OBST (`dp.cpp`):** Organizes search keywords with non-uniform query frequencies into an optimal tree in $O(n^3)$ time to minimize expected lookup cost.
15. **Bellman-Ford Algorithm (`dp.cpp`):** Finds single-source shortest paths on graphs with arbitrary weights and detects negative weight cycles in $O(V \cdot E)$ time by relaxing all edges $V-1$ times.
16. **Floyd-Warshall Algorithm (`dp.cpp`):** Computes all-pairs shortest inter-branch transit distances in $O(V^3)$ time using a dynamic programming triple-loop.
17. **Held-Karp TSP (`dp.cpp`):** Solves the Traveling Salesperson tour for book-stacks inventory retrieval in $O(n^2 \cdot 2^n)$ time using bitmask dynamic programming.
18. **N-Queens Problem (`bb.cpp`):** Places non-interfering environmental sensors across an $N \times N$ archival preservation vault in $O(N!)$ worst-case time using backtracking.
19. **Subset Sum Backtracking (`bb.cpp`):** Finds an exact combination of acquisition invoices matching a fiscal grant in $O(2^n)$ time using backtracking with sum-bound pruning.
20. **Edmonds-Karp Max Flow (`flow.cpp`):** Computes maximum inter-branch digital delivery bandwidth in $O(V \cdot E^2)$ time using breadth-first search to find augmenting paths.
21. **Maximum Bipartite Matching (`flow.cpp`):** Assigns student reservations to available physical book copies in $O(V \cdot E^2)$ time by reduction to maximum network flow.
22. **Extended Euclidean Algorithm (`num.cpp`):** Finds integers $x, y$ such that $a x + b y = \gcd(a, b)$ in $O(\log(\min(a, b)))$ time.
23. **Modular Multiplicative Inverse (`num.cpp`):** Computes $a^{-1} \pmod m$ in $O(\log m)$ time when $\gcd(a, m) = 1$ using Extended Euclidean division.
24. **Chinese Remainder Theorem / CRT (`num.cpp`):** Reconstructs unified transaction timestamps across distributed catalog shards in $O(k \log M)$ time.
25. **Rabin-Karp String Matching (`strmatch.cpp`):** Searches manuscript keywords in $O(n + m)$ average time using polynomial rolling hashes and modular arithmetic.
26. **Finite Automaton String Matching (`strmatch.cpp`):** Processes text in strictly $O(n)$ time after building an $O(m |\Sigma|)$ state transition table.
27. **Naive String Matching (`strmatch.cpp`):** Baseline brute-force search that checks all character shifts in $O(n \cdot m)$ worst-case time.

---

## 4. The 3 Demonstration Scenarios

### Scenario 1: Normal Operations (`./build/dls_app --scenario 1`)
* **What it demonstrates:** A complete 10-step digital library lifecycle under healthy operating conditions, from catalog ranking to manuscript keyword search.
* **Key Numbers to Point Out:**
  1. **Top Document:** Book ID 145 (*Algorithms in C++*) ranked #1 with score **99.9700**.
  2. **Turnstile Peak Window:** Hours **[50..70]** with a net gain of **+106** visitors.
  3. **Abstract Compression:** Huffman Coding compresses 1,248 bits to **666 bits** (**46.6% space savings**).

### Scenario 2: Constraint Shock (`./build/dls_app --scenario 2`)
* **What it demonstrates:** System robustness under adverse financial and infrastructure shocks.
* **Key Numbers to Point Out:**
  1. **Budget Shock Resilience:** A **50.0% budget cut** ($100 to $50) results in only a **29.4% drop in academic utility** (479 down to 338).
  2. **Branch-and-Bound Pruning:** Least-Cost Branch-and-Bound explored **21 nodes** vs FIFO's **879 nodes** (about 40x fewer nodes).
  3. **Fiber Cut Throughput Collapse:** Inter-branch network capacity drops from **160 Mbps to 20 Mbps** (**87.5% drop**) after link (1, 3) fails.

### Scenario 3: Stress Test on 100,000+ Items (`./build/dls_app --scenario 3`)
* **What it demonstrates:** Scalability on massive industrial-scale catalogs and adversarial inputs.
* **Key Numbers to Point Out:**
  1. **100k Sort Runtime:** Quick Sort runs in **20.8 ms** (in-place) vs Merge Sort in **242.4 ms** (7.8 MB buffer)—**about 10x faster (varies per run)**.
  2. **Binary Search at Scale:** Target ID 77777 found among 100,000 items in **0.001 ms** with exactly **31 comparisons** ($O(\log_2 n)$).
  3. **Adversarial String Matching:** KMP makes **200,005 comparisons** vs Naive's **799,944 comparisons** on 100k characters (about 4x fewer comparisons).

---

## 5. Demo Commands in Order

```bash
# 1. Run Scenario 1: Normal Operations
./build/dls_app --scenario 1

# 2. Run Scenario 2: Constraint Shock (Budget Cut & Fiber Cut)
./build/dls_app --scenario 2

# 3. Run Scenario 3: 100k-Item Stress Test & Adversarial Text
./build/dls_app --scenario 3

# 4. Run automated unit test suite
ctest --test-dir build --output-on-failure
```

---

## 7. Library Desk (Menu Option 10)

The Library Desk is an interactive sub-menu that simulates a real library terminal. It reuses existing algorithms—no new algorithms were written.

### What Each Option Does

| Desk Option | What It Does | Algorithm Used | Complexity |
|:---:|:---|:---|:---|
| **1. Search titles** | Reads a word, converts both word and each book title to lower case, runs KMP string matching on every title, prints matching books. | KMP (`matchKMP`) | $O(500 \times (n + m))$ where $n$ = title length, $m$ = word length |
| **2. Find by ID** | Sorts a copy of the catalog by ID with MergeSort, then uses iterative Binary Search to find the book. | MergeSort + Binary Search | $O(n \log n)$ sort + $O(\log n)$ search |
| **3. Top N by score** | Sorts a copy by score descending with MergeSort, prints the first N. | MergeSort (`DocumentScoreDesc`) | $O(n \log n)$ |
| **4. Recommend similar** | Finds the source book by Binary Search. For every other book, computes `score = 2 * (shared keywords) + (LCS title length) / 5`. Sorts recommendations with MergeSort, prints top 5. | Binary Search + LCS (`longestCommonSubsequence`) + MergeSort | $O(n \cdot m_1 \cdot m_2)$ for LCS across titles |

### Sample Run

```text
# Start the program and select option 10 (Library Desk)
printf "10\n1\nalgorithms\n2\n145\n3\n5\n4\n145\n0\n0\n" | ./build/dls_app

# Option 1: Search "algorithms" -> finds books with "algorithms" in the title
#   Shows each matching book's ID, title, year, and score.
#   Prints total KMP comparisons across all 500 titles.

# Option 2: Find ID 145 -> prints full details of book 145
#   Shows ID, title, year, score, and keywords.
#   Reports binary search comparisons (about 9 for 500 books).

# Option 3: Top 5 -> prints the 5 highest-scored books
#   Books are sorted by relevance score descending.

# Option 4: Recommend for ID 145 -> finds 5 most similar books
#   Shows each recommendation's ID, title, score, and shared keywords.
#   Prints the formula: score = 2 * (shared keywords) + (LCS title length) / 5
```

### Viva Questions

1. *Q: Why do you convert to lower case before running KMP?*
   *A:* To make the search case-insensitive, so "Algorithms" matches "algorithms".

2. *Q: Why do you sort a copy instead of the original catalog?*
   *A:* To preserve the original order. Sorting changes the positions of elements, and each sub-option needs a different sort order (by ID vs by score).

3. *Q: What does the recommendation formula measure?*
   *A:* Shared keywords (weighted 2x) capture topic overlap. LCS of titles (divided by 5 to avoid dominating) captures naming similarity. Together they estimate how related two books are.

---

## 8. Honest Limitations & How to Address Them

If the examiner asks about limitations of this project, give these honest, professional answers:

1. **No Real Collaborative Recommendation Engine:**
   * *Limitation:* The project ranks books by relevance score and selects acquisition packages by knapsack utility, but lacks personalized collaborative filtering based on user checkout history.
   * *How to Add It:* *"I would add a user-book rating matrix and implement cosine similarity or matrix factorization to recommend books read by students with similar reading profiles."*
2. **No User or Reading-History Records:**
   * *Limitation:* The data generator models document records and turnstile visitor counts, but does not track persistent user accounts or borrowing logs.
   * *How to Add It:* *"I would introduce a `User` struct with a borrowing history vector and store it in an append-only transaction ledger."*
3. **No Inverted Keyword Index:**
   * *Limitation:* Full-text search runs string matchers (KMP, Rabin-Karp) across document text sequentially rather than looking up words in a pre-indexed dictionary.
   * *How to Add It:* *"I would build an inverted index (`std::unordered_map<std::string, std::vector<int>>`) mapping each stemmed keyword to the list of document IDs containing it for instant $O(1)$ lookups."*
4. **Limited Input Validation:**
   * *Limitation:* Function arguments assume valid ranges (e.g. non-negative capacities, non-empty text, valid graph vertex indices).
   * *How to Add It:* *"I would add defensive boundary checks (`if (capacity < 0 || arr.empty()) throw std::invalid_argument(...)`) at the beginning of each public API function."*
