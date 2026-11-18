# Digital Library Search & Recommendation Engine: 8-Minute Demo Script

This document provides a minute-by-minute presentation script for demonstrating the **Digital Library Search and Recommendation Engine (DLS Engine)**. It is designed for academic defenses, project evaluations, viva panels, and technical walk-throughs.

---

## Presentation Overview & Timing Breakdown

| Time Window | Section | Primary Terminal Command | Focus & Key Takeaways |
|---|---|---|---|
| **0:00 – 1:00** | Introduction & Architecture | `cmake -B build -S . && cmake --build build` | Project scope, zero third-party dependencies, C++17 modular design, and telemetry. |
| **1:00 – 3:30** | Scenario 1: Normal Operations | `./build/dls_app --scenario 1` | 10-step full library lifecycle from catalog ranking to string matching. |
| **3:30 – 5:30** | Scenario 2: Constraint Shock | `./build/dls_app --scenario 2` | Financial austerity (50% budget cut) & network fiber cut throughput collapse. |
| **5:30 – 7:15** | Scenario 3: 100k-Item Stress Test | `./build/dls_app --scenario 3` | Scalability on 100,000 documents; in-place vs buffered sorting; adversarial strings. |
| **7:15 – 8:00** | Conclusion & Automated Benchmarks | `./build/dls_bench` | CSV metrics export, validation summary, and closing remarks. |

---

## Act 1: Introduction & Architecture (0:00 – 1:00)

### Presenter Action:
Open your terminal at the project root. Build the project to demonstrate clean compilation:
```bash
cmake -B build -S . && cmake --build build -j$(nproc)
```

### What to Say:
> "Good morning, respected professors and evaluation panel. Today, I am presenting the **Digital Library Search and Recommendation Engine**, or **DLS Engine**—a high-performance, algorithmic simulation of a modern digital research library.
>
> In real-world digital libraries, systems must handle diverse challenges: ranking millions of scientific papers, scheduling limited physical rooms, planning inter-branch book deliveries, surviving sudden budget cuts, and querying digitized manuscripts.
>
> We have engineered this system from the ground up in modern C++17 with **zero external dependencies**. Foundational data structures—such as our generic array-based `MinHeap` and our `DisjointSet` with path compression and union by rank—are built completely from scratch.
>
> Furthermore, every algorithm in this project is instrumented with a unified `Metrics` harness that tracks wall-clock execution time in milliseconds, exact key comparison counts, and real process memory using the Linux kernel's Resident Set Size. Let us now examine how these algorithms perform across three real-world scenarios."

### Numbers & Architecture to Point Out:
* Point to the clean build output compiling `libdls_core.a` in less than 2 seconds.
* Emphasize the modular separation: domain structures in `include/dls/`, algorithm implementations in `src/`, automated tests in `tests/`, and benchmarks in `bench/`.

---

## Act 2: Scenario 1 — Normal Digital Library Operation (1:00 – 3:30)

### Presenter Action:
Execute Scenario 1 in the terminal:
```bash
./build/dls_app --scenario 1
```

### What to Say:
> "In Scenario 1, we simulate the end-to-end operational workflow of a digital library under normal conditions. This scenario executes 10 distinct algorithmic tasks.
>
> **Step 1: Catalog Ranking.** We rank 500 documents by relevance score. Notice that we run dual implementations: Merge Sort and Quick Sort with Median-of-Three pivot. Both identify the top-ranked book: ID 145, *Algorithms in C++: Graph Algorithms*, with a score of 99.97.
>
> **Step 2: Document Lookup.** A student requests Book ID 250. Our iterative and recursive Binary Searches locate it at index 249 in less than 0.0001 milliseconds with just a single comparison on the mid-element.
>
> **Step 3: Turnstile Visitor Surge.** Using hourly turnstile logs, we detect the peak reading-activity window. Both Divide-and-Conquer and Kadane's algorithm pinpoint hours 50 through 70, showing a net influx of +106 visitors.
>
> **Step 4: Seminal Literature Discovery.** In citation networks, an authoritative paper is cited by all peers while citing none. Using our $O(n)$ elimination algorithm, we discover Authoritative Paper ID 7 in just 57 queries, avoiding an $O(n^2)$ matrix scan.
>
> **Step 5: Room Scheduling.** When 30 student study groups request conference rooms, Greedy Activity Selection fits 13 non-overlapping sessions into a single room. Meanwhile, Interval Partitioning uses our custom Min-Heap to determine that exactly 5 rooms are needed to accommodate all 30 groups without rejection.
>
> **Step 6: Abstract Compression.** Huffman Coding constructs an optimal prefix code tree for book abstracts, compressing 1,248 raw bits down to 666 bits—a 46.6% storage savings.
>
> **Step 7: Inter-Branch Logistics.** To courier physical books across 8 branch libraries, both Prim's algorithm using our Min-Heap and Kruskal's algorithm using our Disjoint-Set determine an identical Minimum Spanning Tree cost of 99 units across 7 transit links.
>
> **Step 8: Acquisition Budgeting.** With a budget of 60 dollars, Dynamic Programming and Least-Cost Branch-and-Bound both select 9 high-impact books, maximizing catalog utility to 419 points.
>
> **Step 9: Book Stacks Retrieval Route.** A librarian must collect books across 5 floor stacks. Held-Karp Bitmask DP computes the optimal TSP walking tour in 0.0046 ms, achieving a total travel distance of 29 meters.
>
> **Step 10: Full-Text Search.** Finally, scanning a digitized manuscript for the keyword 'algorithms' yields 3 matches using Knuth-Morris-Pratt and Rabin-Karp in under 0.001 ms."

### Exact Numbers to Point Out to the Panel:
1. **Top Book**: ID 145, Score **99.9700**.
2. **Turnstile Peak**: Hours **[50..70]**, Net gain **+106**.
3. **Celebrity Checks**: Exactly **57 queries** ($3 \times (20 - 1)$ checks).
4. **Rooms Used**: Single room max = **13 sessions**; Total rooms needed for all 30 groups = **5 rooms**.
5. **Huffman Savings**: Compressed from **1,248 bits** to **666 bits** (**46.6% space saved**).
6. **MST Weight**: **99 units**, exactly **7 edges** (both Prim and Kruskal match).
7. **Knapsack Optimum**: Value **419** from budget **60**.
8. **TSP Tour**: **29 meters**, visiting sequence `0 -> 4 -> 3 -> 2 -> 1 -> 0`.

---

## Act 3: Scenario 2 — Constraint Shock: Austerity & Fiber Cut (3:30 – 5:30)

### Presenter Action:
Execute Scenario 2 in the terminal:
```bash
./build/dls_app --scenario 2
```

### What to Say:
> "Now let us examine system resilience when constraints tighten abruptly. Real-world systems encounter sudden funding cuts and hardware infrastructure failures.
>
> **Part A: Financial Austerity Shock.** The municipal authority slashes our book acquisition budget by 50%, dropping available funds from 100 dollars to 50 dollars across 22 candidate collections.
>
> Please look closely at these comparison numbers:
> Under the normal 100-dollar budget, the maximum utility was 479 points. Under the shocked 50-dollar budget, utility drops to 338 points. That means a **50.0% budget cut causes only a 29.4% drop in academic utility**. The algorithm prioritized high-yield books first, absorbing the shock gracefully.
>
> Even more fascinating is the algorithmic behavior between search strategies:
> Under the tighter 50-dollar budget, Least-Cost Branch-and-Bound explored **only 21 state-space nodes**, compared to FIFO Branch-and-Bound, which had to explore **879 nodes**. The tighter upper bound allowed Least-Cost Branch-and-Bound to prune unpromising branches aggressively, achieving a **41-fold reduction in explored nodes**!
>
> **Part B: Inter-Branch Fiber Cut.** Next, a physical fiber trunk line between Branch 1 and Branch 3 is severed, and link 2 to 4 degrades. Edmonds-Karp Max Flow recalculates the residual network in real time: normal download throughput collapses from 160 Mbps to 20 Mbps—an 87.5% capacity drop. The system immediately flags this bottleneck for automated rerouting."

### Exact Numbers to Point Out to the Panel:
1. **Budget Cut**: Cut from **$100 \to \$50$** (50.0% reduction).
2. **Acquisition Value**: Drops from **479 to 338** (only **29.4% value loss**).
3. **Node Pruning Efficiency**:
   * Baseline (Budget 100): LCBB = **39 nodes** vs FIFO = **4,028 nodes**.
   * Shocked (Budget 50): LCBB = **21 nodes** vs FIFO = **879 nodes**.
   * Point out that LCBB explored **41x fewer nodes** than FIFO under austerity.
4. **Bandwidth Loss**: Normal **160 Mbps** $\to$ Degraded **20 Mbps** (**-140 Mbps loss**, **87.5% throughput drop**).

---

## Act 4: Scenario 3 — Stress Test on 100,000+ Items (5:30 – 7:15)

### Presenter Action:
Execute Scenario 3 in the terminal:
```bash
./build/dls_app --scenario 3
```

### What to Say:
> "To test true industrial scalability, Scenario 3 pushes the engine against **100,000 document records** and an **adversarial 100,000-character text stream**.
>
> **Test 1: Sorting 100,000 Documents.** We compare Merge Sort against Quick Sort.
> Both algorithms sort all 100,000 items deterministically and verify identical ordering. But look at the hardware profile:
> * Merge Sort took **242 milliseconds** and required an auxiliary buffer of **7.8 megabytes** for merging.
> * Quick Sort with Median-of-Three took **only 20.8 milliseconds** and executed completely **in-place**.
> That is a speedup of about 10x (varies per run) for Quick Sort. Notice that Merge Sort actually made fewer comparisons—1.53 million compared to 1.97 million for Quick Sort. Yet Quick Sort is about 10x (varies per run) faster, likely due to data movement and cache behaviour (merge sort copies whole Document structs into its auxiliary buffer, while Quick Sort partitions in-place).
>
> **Test 2: Binary Search at Scale.** In a catalog of 100,000 sorted books, looking up ID 77777 took **0.001 milliseconds**. Both iterative and recursive implementations completed in exactly **31 comparisons**, confirming our theoretical $O(\log_2 n)$ bound.
>
> **Test 3: Adversarial String Matching.** We feed an adversarial text of 100,000 repeated 'a' characters ending in a 'b', searching for the pattern 'aaaaaaab'.
> The Naive matcher is forced to backtrack repeatedly, making **799,944 comparisons**. In contrast, Knuth-Morris-Pratt utilizes its precomputed $\pi$ prefix failure table to advance linearly, making only **200,005 comparisons**—a **4x reduction in comparisons** that completely avoids worst-case quadratic degradation."

### Exact Numbers to Point Out to the Panel:
1. **100k Sort Runtime**: MergeSort = **242.4 ms** vs QuickSort = **20.8 ms** (**about 10x faster (varies per run)**).
2. **Sort Comparisons**: MergeSort = **1,536,433** vs QuickSort = **1,974,783**.
3. **Memory Allocation**: MergeSort = **7.8 MB auxiliary buffer**; QuickSort = **0 MB auxiliary (in-place)**.
4. **Binary Search**: Target ID 77777 found in **0.001 ms** with exactly **31 comparisons**.
5. **Adversarial String Comparisons**: Naive = **799,944 comparisons** vs KMP = **200,005 comparisons** (linear bound).

---

## Act 5: Conclusion & Automated Benchmarks (7:15 – 8:00)

### Presenter Action:
Run the automated benchmark executable:
```bash
./build/dls_bench
```

### What to Say:
> "To conclude our demonstration, I am running our automated benchmark binary, `dls_bench`.
>
> This test automatically benchmarks sorting across 1,000, 10,000, and 100,000 items, binary search, string matching, Minimum Spanning Trees, and 0/1 Knapsack.
>
> As you can see, all results are exported directly into `data/benchmark_results.csv` for post-run telemetry and visualization.
>
> In summary: every algorithm in the DLS Engine was deliberately mapped to solve an operational necessity in modern digital libraries. Theoretical invariants were upheld, verified with dual implementations, and benchmarked on massive datasets.
>
> Thank you for your time. I am now open to questions from the panel."

---

## Anticipated Viva / Evaluation Questions & Quick Answers

### Q1: Why does Quick Sort run about 10x (varies per run) faster than Merge Sort if it makes more comparisons?
> **Answer**: "Merge Sort requires an auxiliary vector allocation of $O(n)$ elements (7.8 MB for 100,000 documents) and copying elements back and forth between buffers. Quick Sort operates entirely in-place. Quick Sort's performance advantage is likely due to data movement and cache behaviour (merge sort copies whole Document structs to and from an auxiliary buffer, whereas Quick Sort partitions in-place), despite Quick Sort performing ~28% more comparisons."

### Q2: Why did Least-Cost Branch-and-Bound prune so much more effectively during the budget shock?
> **Answer**: "Least-Cost Branch-and-Bound uses a priority queue ordered by the best upper bound computed via continuous fractional knapsack relaxation. When the budget is cut by 50%, candidate items exceed the capacity much earlier in the tree. The tighter bound quickly falls below the best feasible integer solution found so far, allowing entire subtrees to be pruned immediately (21 nodes vs 879 nodes)."

### Q3: How does your custom `MinHeap` differ from standard library containers?
> **Answer**: "Our `MinHeap` is implemented from scratch as a templated complete binary tree over `std::vector`. Unlike `std::priority_queue`, it directly integrates with our `Metrics` harness, tracking comparisons during every `siftUp` and `siftDown` step. It also supports direct assignment and clearing, which we leverage in Prim's algorithm, Dijkstra's algorithm, Huffman encoding, and Interval Partitioning."

### Q4: How does KMP achieve immunity against adversarial repetitive strings?
> **Answer**: "The Naive string matcher resets the pattern pointer to index 0 after a mismatch, re-checking characters and resulting in $O(n \cdot m)$ worst-case time (799,944 comparisons on our 100k test). Knuth-Morris-Pratt precomputes the $\pi$ prefix failure table in $O(m)$ time. On a mismatch, it uses $\pi[q-1]$ to slide the pattern to the longest proper prefix that is also a suffix, ensuring the text pointer never backtracks and guaranteeing $O(n + m)$ runtime."
