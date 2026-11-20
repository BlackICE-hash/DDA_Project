# Digital Library Search & Recommendation Engine (DLS Engine)

The **Digital Library Search and Recommendation Engine (DLS Engine)** is an algorithmic simulation of a modern digital research library. Built in modern C++17 with zero third-party dependencies, the engine models catalog indexing, distributed synchronization, physical logistical routing, room reservation scheduling, data compression, manuscript search, and acquisition optimization.

---

## Documentation Quick Links

Comprehensive project documentation is available in the [`docs/`](docs/) directory:

| Document | Description |
|:---|:---|
| 📖 [**`docs/USER_GUIDE.md`**](docs/USER_GUIDE.md) | Complete guide on building, running the interactive CLI menu, running `--all`, running benchmarks, and exporting CSV files with sample terminal output. |
| 🏗️ [**`docs/DESIGN_GUIDE.md`**](docs/DESIGN_GUIDE.md) | Deep dive into architecture, folder layout, core data structures (`Document`, `Metrics`, `MinHeap`, `DisjointSet`), and the rationale mapping each algorithm to library tasks. |
| ⏱️ [**`docs/DEMO_SCRIPT.md`**](docs/DEMO_SCRIPT.md) | An 8-minute presentation script covering the three scenarios, listing commands to run and key numbers to highlight. |
| 📐 [**`docs/COMPLEXITY.md`**](docs/COMPLEXITY.md) | Theoretical complexity analysis, Master Theorem derivations, recursion tree proofs, and formal NP-Completeness reduction (Subset-Sum $\le_P$ 0/1 Knapsack). |
| 🎓 [**`docs/STUDY_NOTES.md`**](docs/STUDY_NOTES.md) | Comprehensive algorithm study notes, loop invariants, correctness proofs, hand-traceable numerical examples, and viva questions & answers for all 20+ algorithms. |

---

## Key Architectural Highlights

* **Zero External Dependencies**: Implemented strictly using modern C++17 standard library components (`std::vector`, `std::string`, `std::chrono`, `std::iostream`).
* **From-Scratch Data Structures**: Includes a binary min-heap (`MinHeap`) and Disjoint Set Union (`DisjointSet`) with path compression and union by rank.
* **Unified Metrics Harness**: Every algorithm is instrumented with a `Metrics` tracker measuring runtime in milliseconds, exact key comparisons, and theoretical auxiliary space.
* **Dual-Implementation Verification**: Dual algorithmic strategies verify correctness against an independent oracle across sorting, divide-and-conquer, graph algorithms, knapsack search, and pattern matching.
* **Resilience Under Constraint Shock**: Tests system behavior during financial austerity (50% budget cuts) and network infrastructure failures (inter-branch fiber cuts).
* **Scalability**: Tested on datasets of 100,000+ catalog records and adversarial character streams.

---

## Quickstart

```bash
# 1. Configure and build
cmake -B build -S .
cmake --build build -j$(nproc)

# 2. Run unit tests
ctest --test-dir build --output-on-failure

# 3. Run all demonstration scenarios
./build/dls_app --all

# 4. Run automated performance benchmarks and export CSV
./build/dls_bench
```

---

## Build Instructions

### Prerequisites
* **Compiler**: GCC 9.0+ or Clang 10.0+ supporting C++17.
* **Build System**: CMake 3.16+ and GNU Make or Ninja.
* **Operating System**: Linux (Ubuntu 20.04+, Debian, Fedora, Arch) or WSL2.

### Step-by-Step Compilation
```bash
# Create build directory and generate build files
cmake -B build -S .

# Compile static library, CLI application, benchmark suite, and tests
cmake --build build -j$(nproc)
```

### Running Automated Tests
```bash
ctest --test-dir build --output-on-failure
```

---

## Run Instructions

### 1. Interactive CLI Application
Launch the interactive command menu:
```bash
./build/dls_app
```
Menu options include:
* `1`: Scenario 1: Normal Library Operation
* `2`: Scenario 2: Constraint Shock (Budget Cut & Capacity Drop)
* `3`: Scenario 3: Stress Test (100,000+ Items)
* `4`: Library Desk (search, find, top books, recommend)
* `5`: Export Sample Books to `data/documents.csv`
* `0`: Exit

### 2. Automated CLI Modes (`--all`, `--scenario`)
Bypass the interactive menu for automated scripts or grading pipelines:
```bash
# Execute all three scenarios sequentially
./build/dls_app --all

# Execute a single scenario
./build/dls_app --scenario 1   # Normal operations
./build/dls_app --scenario 2   # Constraint shock
./build/dls_app --scenario 3   # 100k-item stress test
```

### 3. Automated Performance Benchmarks (`dls_bench`)
Run the standalone benchmarking binary to evaluate execution times, comparison counts and theoretical auxiliary space across multiple scales ($N = 1\,000, 10\,000, 100\,000$):
```bash
./build/dls_bench
```
This automatically updates and exports telemetry to `data/benchmark_results.csv`.

### 4. Exporting Data to CSV
* **Catalog Records**: Choose option `5` in the interactive menu or run `printf "5\n0\n" | ./build/dls_app` to generate `data/documents.csv`.
* **Benchmark Metrics**: Running `./build/dls_bench` exports results to `data/benchmark_results.csv`.

---

## Summary of Algorithmic Mappings

| Library Domain Task | Algorithm Used | Paradigm | Complexity |
|:---|:---|:---|:---|
| Catalog Relevance Ranking | Merge Sort vs Quick Sort | Divide & Conquer | $O(n \log n)$ |
| Document Fast Lookup | Binary Search (Iterative & Recursive) | Decrease & Conquer | $O(\log n)$ |
| Peak Visitor Traffic Detection | Maximum Subarray (D&C vs Kadane) | Divide & Conquer / DP | $O(n \log n)$ vs $O(n)$ |
| Seminal Research Discovery | Celebrity Elimination | Elimination / D&C | $O(n)$ queries |
| Reading Room Allocation | Activity Selection | Greedy | $O(n \log n)$ |
| Facility Room Planning | Interval Partitioning (MinHeap) | Greedy with Heap | $O(n \log d)$ |
| Abstract Text Compression | Huffman Coding (MinHeap) | Greedy with Heap | $O(k \log k)$ |
| Inter-Branch Courier Network | Prim's & Kruskal's MST (Heap / DSU) | Greedy with Heap / DSU | $O((V+E)\log V)$ |
| Acquisition Budget Allocation | 0/1 Knapsack (DP vs LCBB / FIFO) | DP / Branch-and-Bound | $O(nW)$ vs $O(2^n)$ |
| Book Stacks Retrieval Tour | Held-Karp Traveling Salesperson (TSP) | Bitmask DP | $O(n^2 \cdot 2^n)$ |
| Full-Text Manuscript Search | KMP, Rabin-Karp, Automaton, Naive | String Matching | $O(n + m)$ |
| Co-Citation Matrix Analysis | Strassen's Matrix Multiplication | Divide & Conquer | $O(n^{2.807})$ |
| Skewed Query Search Index | Optimal Binary Search Tree (OBST) | Dynamic Programming | $O(n^3)$ |
| Edition Diff & Text Overlap | Longest Common Subsequence (LCS) | Dynamic Programming | $O(m \cdot n)$ |
| Multi-Table Join Planning | Matrix Chain Multiplication | Dynamic Programming | $O(n^3)$ |
| Data Replication Routing | Dijkstra & Bellman-Ford | Greedy / Dynamic Programming | $O((V+E)\log V)$ / $O(VE)$ |
| Inter-Branch Distance Matrix | Floyd-Warshall All-Pairs | Dynamic Programming | $O(V^3)$ |
| Digital Delivery Bandwidth | Edmonds-Karp Max Flow | Flow & Matching (BFS) | $O(V \cdot E^2)$ |
| Book Lending Copy Assignment | Maximum Bipartite Matching | Flow Reduction | $O(V \cdot E^2)$ |
| Distributed Clock Synchronization | Chinese Remainder Theorem (CRT) | Number Theory (ExtGCD) | $O(k \log M)$ |
| Vault Security Sensor Layout | N-Queens Problem | Backtracking | $O(N!)$ |
| Invoicing & Grant Auditing | Subset Sum Problem | Backtracking with Bounding | $O(2^n)$ |
| Security Token Verification | Fast Modular Powering | Divide & Conquer | $O(\log \text{exp})$ |

---

## Repository Layout

```text
DDA_Project/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project overview & documentation index
├── app/                        # Main CLI application
│   └── main.cpp                # Menu, scenarios (--all, --scenario)
├── bench/                      # Benchmarking suite
│   └── benchmark.cpp           # Multi-scale benchmarks exporting CSV
├── data/                       # CSV datasets and benchmark results
│   ├── benchmark_results.csv   # Benchmark performance telemetry
│   └── documents.csv           # Exported sample catalog
├── docs/                       # Comprehensive documentation
│   ├── COMPLEXITY.md           # Master theorem & NP-completeness reductions
│   ├── DEMO_SCRIPT.md          # 8-minute presentation & viva script
│   ├── DESIGN_GUIDE.md         # Architecture, data structures & mapping rationale
│   ├── STUDY_NOTES.md          # In-depth algorithmic notes & viva Q&A
│   └── USER_GUIDE.md           # Build & run instructions with example outputs
├── include/dls/                # Header files and templates
├── src/                        # Implementation files (libdls_core.a)
└── tests/                      # Unit test suites (CTest)
```

---

## License

This project is released under the MIT License.
