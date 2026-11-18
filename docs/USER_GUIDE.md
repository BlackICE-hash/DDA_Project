# Digital Library Search & Recommendation Engine: User Guide

Welcome to the **Digital Library Search and Recommendation Engine (DLS Engine)** user guide. This document provides complete instructions on how to build the software, run interactive and automated CLI commands, execute performance benchmarks, and export dataset metrics to CSV files.

---

## Table of Contents
1. [System Requirements & Prerequisites](#1-system-requirements--prerequisites)
2. [Building the Engine](#2-building-the-engine)
3. [Running Automated Unit Tests](#3-running-automated-unit-tests)
4. [Running the Interactive CLI Menu](#4-running-the-interactive-cli-menu)
5. [Running Non-Interactive CLI Scenarios (`--all`, `--scenario`)](#5-running-non-interactive-cli-scenarios---all---scenario)
6. [Running the Performance Benchmark Suite (`dls_bench`)](#6-running-the-performance-benchmark-suite-dls_bench)
7. [Exporting Datasets and Benchmark Results to CSV](#7-exporting-datasets-and-benchmark-results-to-csv)
8. [Troubleshooting & FAQ](#8-troubleshooting--faq)

---

## 1. System Requirements & Prerequisites

The DLS Engine is engineered in modern C++17 with zero external third-party library dependencies beyond the standard C++ library (`std::chrono`, `std::vector`, `std::string`, `std::iostream`, etc.).

* **Operating System**: Linux (Ubuntu 20.04+, Debian 11+, Fedora, Arch) or POSIX-compliant environment (WSL2 / macOS with Clang).
* **C++ Compiler**: GCC 9.0+ or Clang 10.0+ supporting `-std=c++17`.
* **Build System**: CMake 3.16 or newer and GNU Make (or Ninja).
* **Hardware Requirements**: Minimum 512 MB RAM (100,000-record stress tests utilize ~100 MB peak RSS).

Verify your environment tools:
```bash
cmake --version
g++ --version
```

---

## 2. Building the Engine

The project utilizes CMake for cross-platform out-of-source builds.

### Step 1: Configure the Build Directory
From the root repository directory:
```bash
cmake -B build -S .
```
*Output:*
```text
-- The CXX compiler identification is GNU 13.3.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.2s)
-- Generating done (1.2s)
-- Build files have been written to: /path/to/DDA_Project/build
```

### Step 2: Compile All Targets
Compile the static library `libdls_core.a`, the main CLI application `dls_app`, the benchmark executable `dls_bench`, and all test binaries:
```bash
cmake --build build -j$(nproc)
```
*Output:*
```text
[  3%] Building CXX object CMakeFiles/dls_core.dir/src/bb/bb.cpp.o
[  7%] Building CXX object CMakeFiles/dls_core.dir/src/data/generator.cpp.o
[ 11%] Building CXX object CMakeFiles/dls_core.dir/src/dc/dc.cpp.o
[ 14%] Building CXX object CMakeFiles/dls_core.dir/src/dp/dp.cpp.o
[ 18%] Building CXX object CMakeFiles/dls_core.dir/src/flow/flow.cpp.o
[ 22%] Building CXX object CMakeFiles/dls_core.dir/src/greedy/greedy.cpp.o
[ 25%] Building CXX object CMakeFiles/dls_core.dir/src/num/num.cpp.o
[ 29%] Building CXX object CMakeFiles/dls_core.dir/src/search/search.cpp.o
[ 33%] Building CXX object CMakeFiles/dls_core.dir/src/sort/sort.cpp.o
[ 37%] Building CXX object CMakeFiles/dls_core.dir/src/strmatch/strmatch.cpp.o
[ 40%] Linking CXX static library libdls_core.a
[ 40%] Built target dls_core
[ 44%] Building CXX object CMakeFiles/dls_app.dir/app/main.cpp.o
[ 48%] Linking CXX executable dls_app
[ 48%] Built target dls_app
[ 51%] Building CXX object CMakeFiles/dls_bench.dir/bench/benchmark.cpp.o
[ 55%] Linking CXX executable dls_bench
[ 55%] Built target dls_bench
[ 59%] Building CXX object CMakeFiles/test_dc.dir/tests/test_dc.cpp.o
[ 62%] Linking CXX executable test_dc
[ 62%] Built target test_dc
[ 66%] Building CXX object CMakeFiles/test_dp.dir/tests/test_dp.cpp.o
[ 70%] Linking CXX executable test_dp
[ 70%] Built target test_dp
[ 74%] Building CXX object CMakeFiles/test_greedy.dir/tests/test_greedy.cpp.o
[ 77%] Linking CXX executable test_greedy
[ 77%] Built target test_greedy
[ 81%] Building CXX object CMakeFiles/test_sort_search.dir/tests/test_sort_search.cpp.o
[ 85%] Linking CXX executable test_sort_search
[ 85%] Built target test_sort_search
[ 88%] Building CXX object CMakeFiles/test_stage1.dir/tests/test_stage1.cpp.o
[ 92%] Linking CXX executable test_stage1
[ 92%] Built target test_stage1
[ 96%] Building CXX object CMakeFiles/test_stage6.dir/tests/test_stage6.cpp.o
[100%] Linking CXX executable test_stage6
[100%] Built target test_stage6
```

---

## 3. Running Automated Unit Tests

To verify that all algorithms, custom data structures, and edge cases pass verification:
```bash
ctest --test-dir build --output-on-failure
```
*Sample Output:*
```text
Test project /path/to/DDA_Project/build
    Start 1: test_dc
1/6 Test #1: test_dc ..........................   Passed    0.01 sec
    Start 2: test_dp
2/6 Test #2: test_dp ..........................   Passed    0.01 sec
    Start 3: test_greedy
3/6 Test #3: test_greedy ......................   Passed    0.02 sec
    Start 4: test_sort_search
4/6 Test #4: test_sort_search .................   Passed    0.01 sec
    Start 5: test_stage1
5/6 Test #5: test_stage1 ......................   Passed    0.04 sec
    Start 6: test_stage6
6/6 Test #6: test_stage6 ......................   Passed    0.01 sec

100% tests passed out of 6

Total Test time (real) =   0.42 sec
```

---

## 4. Running the Interactive CLI Menu

To launch the interactive command-line interface:
```bash
./build/dls_app
```

### The Interactive Menu Options
```text
======================================================================
   DIGITAL LIBRARY SEARCH AND RECOMMENDATION ENGINE - CLI REGISTRY   
======================================================================
 1. Run Scenario 1: Normal Library Operation Demo
 2. Run Scenario 2: Constraint Shock (Budget Cut & Capacity Drop)
 3. Run Scenario 3: Stress Test (100,000+ Items)
 4. Sorting & Search Demo (MergeSort, QuickSort, BinarySearch)
 5. Divide & Conquer Demo (Max Subarray, Fast Power, Celebrity, Strassen)
 6. Greedy Demo (Activity Selection, Interval Partition, Huffman, Dijkstra, MST)
 7. Dynamic Programming Demo (Knapsack, Matrix Chain, LCS, OBST, Bellman, TSP)
 8. Backtracking, Flow, CRT & String Matching Demo
 9. Export Sample Datasets to data/
 0. Exit
----------------------------------------------------------------------
Enter selection [0-9]: 
```

### Piped / Scripted Menu Execution Example
You can run any menu option unattended by piping your selection:
```bash
# Run Option 4 (Sorting & Search Demo) and then exit (0)
printf "4\n0\n" | ./build/dls_app
```
*Sample Output:*
```text
Welcome to the Digital Library Search and Recommendation Engine!

======================================================================
   DIGITAL LIBRARY SEARCH AND RECOMMENDATION ENGINE - CLI REGISTRY   
======================================================================
 1. Run Scenario 1: Normal Library Operation Demo
...
Enter selection [0-9]: 
======================================================================
  Sorting & Binary Search Demo
======================================================================
  [MergeSort 50 Docs] Time: 0.0521 ms | Comparisons: 247 | Aux space (theory): O(n)
Top document: [ID 4] Discrete Mathematics and Its Applications #4 (Score: 93.92)

======================================================================
   DIGITAL LIBRARY SEARCH AND RECOMMENDATION ENGINE - CLI REGISTRY   
======================================================================
...
Enter selection [0-9]: Exiting DLS Engine. Goodbye!
```

---

## 5. Running Non-Interactive CLI Scenarios (`--all`, `--scenario`)

For automated pipelines, scripts, or continuous integration, `dls_app` provides CLI flags that bypass the interactive prompt.

### Option A: Run All Three Scenarios Sequentially (`--all` or `--demo`)
```bash
./build/dls_app --all
```
This executes Scenario 1 (Normal Operations), Scenario 2 (Constraint Shock), and Scenario 3 (Stress Test) in sequence.

*Truncated Sample Output:*
```text
======================================================================
  SCENARIO 1: NORMAL DIGITAL LIBRARY OPERATION
======================================================================
Simulating a comprehensive end-to-end digital library workflow.

1. Ranking Document Catalog by Relevance Score...
  [MergeSort (Stable)] Time: 0.6631 ms | Comparisons: 3859 | Aux space (theory): O(n)
  [QuickSort (Median-of-3)] Time: 0.0594 ms | Comparisons: 4914 | Aux space (theory): O(log n) stack
  -> Top Ranked Book: [ID 145] "Algorithms in C++: Graph Algorithms #145" (Score: 99.9700)

2. Looking up Document by ID in Sorted Catalog...
  [Binary Search (Iterative)] Time: 0.0000 ms | Comparisons: 1 | Aux space (theory): O(1)
  [Binary Search (Recursive)] Time: 0.0001 ms | Comparisons: 1 | Aux space (theory): O(log n) stack
  -> Found at index 249: "Computer Networks: A Systems Approach #250"

3. Detecting Peak Reading-Activity Window (Net Reader Visitor Deltas)...
  [Max Subarray (Divide & Conquer)] Time: 0.0018 ms | Comparisons: 823 | Aux space (theory): O(log n) stack
  [Max Subarray (Kadane's Algorithm)] Time: 0.0002 ms | Comparisons: 198 | Aux space (theory): O(1)
  -> Peak Window: Hours [50..70] with net visitor gain of +106

4. Discovering Authoritative 'Celebrity' Paper (Cited by all, cites none)...
  [Celebrity Discovery O(n)] Time: 0.0001 ms | Comparisons: 57 | Aux space (theory): O(1)
  -> Authoritative Paper Found: ID 7

5. Allocating Reading Rooms for Student Study Groups...
  [Activity Selection (Max Compatible Slots in 1 Room)] Time: 0.0021 ms | Comparisons: 143 | Aux space (theory): O(n)
  [Interval Partitioning (Min Rooms for ALL Slots)] Time: 0.0030 ms | Comparisons: 230 | Aux space (theory): O(n)
  -> Max compatible slots in single room: 13
  -> Total rooms required to schedule all 30 groups: 5

6. Compressing Book Abstract using Huffman Coding...
  [Huffman Coding] Time: 0.0212 ms | Comparisons: 950 | Aux space (theory): O(k) tree
  -> Original: 1248 bits, Compressed: 666 bits (Space Saved: 46.6%)

7. Optimizing Inter-Branch Physical Courier Network (MST)...
  [Prim's Algorithm (MinHeap)] Time: 0.0019 ms | Comparisons: 78 | Aux space (theory): O(V + E)
  [Kruskal's Algorithm (Union-Find)] Time: 0.0014 ms | Comparisons: 78 | Aux space (theory): O(V + E)
  -> Minimum Connection Cost: 99 units (Edges used: 7)

8. Optimal Book Acquisitions under Acquisition Budget...
  [0/1 Knapsack (Dynamic Programming)] Time: 0.0034 ms | Comparisons: 814 | Aux space (theory): O(nW)
  [0/1 Knapsack (Least-Cost Branch & Bound)] Time: 0.0061 ms | Comparisons: 471 | Aux space (theory): O(2^n) queue
  -> Max Catalog Value Acquired: 419 (Items chosen: 9)

9. Computing Librarian Optimal Book Pickup Route (TSP Bitmask DP)...
  [TSP (Bitmask DP O(n^2 * 2^n))] Time: 0.0046 ms | Comparisons: 56 | Aux space (theory): O(n * 2^n)
  -> Optimal Pickup Distance: 29 meters | Route: 0 -> 4 -> 3 -> 2 -> 1 -> 0

10. Full-Text Manuscript Search for Keyword 'algorithms'...
  [KMP String Matcher] Time: 0.0006 ms | Comparisons: 112 | Aux space (theory): O(m)
  [Rabin-Karp String Matcher] Time: 0.0007 ms | Comparisons: 121 | Aux space (theory): O(1)
  -> Found 3 matches at character indices: 17 67 89 

======================================================================
  SCENARIO 2: CONSTRAINT SHOCK (BUDGET CUT & CAPACITY DROP)
======================================================================
Demonstrating system resilience and algorithm behavior when constraints tighten abruptly.

--- PART A: FINANCIAL AUSTERITY SHOCK (50% Budget Cut) ---
[Baseline: Normal Budget = 100]
  * DP Knapsack Value: 479 | Time: 0.0069 ms | Comparisons: 2051
  * LCBB Knapsack Value: 479 | Nodes: 39 | Time: 0.0061 ms
  * FIFO Knapsack Value: 479 | Nodes: 4028 | Time: 0.3337 ms

[Shocked: Austerity Budget = 50 (50% Cut)]
  * DP Knapsack Value: 338 | Time: 0.0044 ms | Comparisons: 951
  * LCBB Knapsack Value: 338 | Nodes: 21 | Time: 0.0041 ms
  * FIFO Knapsack Value: 338 | Nodes: 879 | Time: 0.0554 ms
  -> Budget reduction ratio: 50.0% | Acquired Value drop: 29.4%
  -> Pruning effectiveness under tighter bounds: LCBB explored 21 nodes vs FIFO 879 nodes.

--- PART B: INTER-BRANCH FIBER CUT (Capacity Drop Shock) ---
  * Normal Download Bandwidth: 160 Mbps (Edmonds-Karp Time: 0.0 ms)
  * Degraded Download Bandwidth: 20 Mbps (Edmonds-Karp Time: 0.0 ms)
  -> Bandwidth Loss: -140 Mbps (87.5% throughput drop)

======================================================================
  SCENARIO 3: STRESS TEST WITH 100,000+ ITEMS
======================================================================
Benchmarking scalability on massive datasets: comparing dual strategies.

Generating 100000 document records with deterministic seed...

1. Sorting 100,000 Documents by Relevance Score:
  [Merge Sort (O(n log n) Guaranteed)] Time: 242.3609 ms | Comparisons: 1536433 | Aux space (theory): O(n)
  [Quick Sort (Median-of-3 In-Place)] Time: 20.8030 ms | Comparisons: 1974783 | Aux space (theory): O(log n) stack
  -> Equivalence verified: YES (Identical order)
  -> Memory footprint: MergeSort allocated auxiliary buffer (7 MB), QuickSort ran in-place.
  -> QuickSort performance advantage is likely due to data movement and cache behaviour.
     (Note: Merge Sort copies whole Document structs into its auxiliary buffer, while Quick Sort partitions in-place).

2. Searching in 100,000 Sorted Documents (Looking for Book ID 77777):
  [Binary Search Iterative] Time: 0.0013 ms | Comparisons: 31 | Aux space (theory): O(1)
  [Binary Search Recursive] Time: 0.0008 ms | Comparisons: 31 | Aux space (theory): O(log n) stack
  -> Result: ID 77777 found at index 77776 (Title: "Artificial Intelligence: A Modern Approach #77777")
  -> Both methods made exactly 31 comparisons.

3. String Matching Stress Test (100,000 Characters with Adversarial Suffix):
  [Naive String Matcher] Time: 0.1922 ms | Comparisons: 799944 | Aux space (theory): O(1)
  [Rabin-Karp Rolling Hash] Time: 0.3360 ms | Comparisons: 100001 | Aux space (theory): O(1)
  [Knuth-Morris-Pratt (KMP)] Time: 0.2022 ms | Comparisons: 200005 | Aux space (theory): O(m)
  -> Comparisons & Time: Naive 799944 comps (0.1922 ms) vs KMP 200005 comps (0.2022 ms)
  -> On adversarial text: similar time, but 4.0 times fewer comparisons with KMP.
```

### Option B: Run a Single Specific Scenario
```bash
# Execute only Scenario 1: Normal Operation
./build/dls_app --scenario 1

# Execute only Scenario 2: Constraint Shock
./build/dls_app --scenario 2

# Execute only Scenario 3: 100,000+ Item Stress Test
./build/dls_app --scenario 3
```

---

## 6. Running the Performance Benchmark Suite (`dls_bench`)

The automated benchmark binary rigorously stress tests dual algorithmic implementations across multiple problem sizes ($N = 1\,000, 10\,000, 100\,000$).

### Command
```bash
./build/dls_bench
```

### Terminal Output
```text
======================================================================
            DIGITAL LIBRARY ENGINE: AUTOMATED BENCHMARKS              
======================================================================
[Sort N=  1000] MergeSort:  1.25709 ms (8705 comps) | QuickSort: 0.122897 ms (11613 comps)
[Sort N= 10000] MergeSort:  16.2102 ms (120439 comps) | QuickSort:  1.56469 ms (144281 comps)
[Sort N=100000] MergeSort:  244.016 ms (1536433 comps) | QuickSort:  23.7028 ms (1974783 comps)

[Search N=100000] Iterative: 0.001648 ms (31 comps) | Recursive: 0.000522 ms (31 comps)

[StringMatch N=100000] Naive: 0.184204 ms (799944 comps) | KMP: 0.199062 ms (200005 comps) | RK: 0.328165 ms (100001 comps)

[MST V=50, E=300] Prim: 0.017332 ms (Weight: 465) | Kruskal: 0.0139 ms (Weight: 465)

[Knapsack N=20, W=60] DP: 0.003652 ms (Val: 414) | LCBB: 0.004474 ms (Nodes: 21) | FIFO-BB: 0.032669 ms (Nodes: 407)

Successfully exported benchmark metrics to data/benchmark_results.csv
```

---

## 7. Exporting Datasets and Benchmark Results to CSV

The DLS Engine includes automated CSV serialization for catalog documents and performance metrics.

### 7.1 Exporting Document Records via CLI
From the CLI menu, choose option `9`:
```bash
printf "9\n0\n" | ./build/dls_app
```
*Output File:* `data/documents.csv`

#### Inspecting the Exported Document CSV:
```bash
head -n 5 data/documents.csv
```
*File Content:*
```csv
id,title,year,score,keywords
1,"Computer Networks: A Systems Approach #1",2013,19.16,"distributed;np-completeness;np-completeness"
2,"Design Patterns: Elements of Reusable Object-Oriented Software #2",1994,10.9,"greedy"
3,"Computer Architecture: A Quantitative Approach #3",1988,15.14,"database;algorithms;algorithms"
4,"Discrete Mathematics and Its Applications #4",2009,93.92,"algorithms"
```

### 7.2 Benchmark Metrics CSV
Running `./build/dls_bench` automatically generates `data/benchmark_results.csv`.

#### Schema:
* `category`: Algorithm category (`Sort`, `Search`, `StringMatch`, `MST`, `Knapsack`).
* `algorithm`: Implementation name (`MergeSort`, `QuickSort`, `BinarySearchIterative`, `KMP`, etc.).
* `input_size`: Number of elements $N$ or vertices $V$.
* `time_ms`: Elapsed execution wall time in milliseconds.
* `comparisons`: Total comparison operations performed.
* `aux_space_theory`: Theoretical auxiliary space complexity (e.g. `O(n)`, `O(log n) stack`, `O(m)`).

#### Inspecting Benchmark CSV Content:
```bash
cat data/benchmark_results.csv
```
*File Content:*
```csv
category,algorithm,input_size,time_ms,comparisons,aux_space_theory
Sort,MergeSort,1000,1.25709,8705,O(n)
Sort,QuickSort,1000,0.122897,11613,O(log n) stack
Sort,MergeSort,10000,16.2102,120439,O(n)
Sort,QuickSort,10000,1.56469,144281,O(log n) stack
Sort,MergeSort,100000,244.016,1536433,O(n)
Sort,QuickSort,100000,23.7028,1974783,O(log n) stack
Search,BinarySearchIterative,100000,0.001648,31,O(1)
Search,BinarySearchRecursive,100000,0.000522,31,O(log n) stack
StringMatch,Naive,100000,0.184204,799944,O(1)
StringMatch,RabinKarp,100000,0.328165,100001,O(1)
StringMatch,KMP,100000,0.199062,200005,O(m)
MST,Prim,50,0.017332,2079,O(V + E)
MST,Kruskal,50,0.0139,2473,O(V + E)
Knapsack,DP,20,0.003652,1074,O(nW)
Knapsack,LCBB,20,0.004474,360,O(2^n) queue
Knapsack,FIFO-BB,20,0.032669,3529,O(2^n) queue
```
Knapsack,DP,20,0.003652,1074,0
Knapsack,LCBB,20,0.004474,360,0
Knapsack,FIFO-BB,20,0.032669,3529,0
```

---

## 8. Troubleshooting & FAQ

* **Q: Why does `ctest` report `No tests were found!!!`?**  
  *A:* Ensure you run `ctest --test-dir build` so CMake knows where the test manifests reside.
* **Q: Why does `dls_bench` or option 9 fail to open the CSV file?**  
  *A:* The application expects a `data/` directory relative to the current working directory. Ensure you execute `./build/dls_app` from the repository root, or create `mkdir -p data`.
* **Q: How are tie-breakers handled when sorting documents with identical relevance scores?**  
  *A:* `DocumentScoreDesc` breaks score ties using ascending Document ID (`a.id < b.id`), ensuring deterministic ordering.
