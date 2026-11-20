# Digital Library Search & Recommendation Engine: 8-Minute Demo Script

This document provides a presentation outline for demonstrating the **Digital Library Search and Recommendation Engine (DLS Engine)**.

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

### Terminal Command:
```bash
cmake -B build -S . && cmake --build build -j$(nproc)
```

### Key Numbers to Point Out:
1. **Compilation Time**: Static library `libdls_core.a` compiles cleanly in under 2 seconds with zero third-party dependencies.
2. **Standard Compliance**: 100% standard C++17 library components without external dependencies.
3. **Instrumentation Coverage**: Unified `Metrics` tracker instrumenting 20+ algorithms across runtime (ms), comparison counts, and theoretical auxiliary space.

---

## Act 2: Scenario 1 — Normal Digital Library Operation (1:00 – 3:30)

### Terminal Command:
```bash
./build/dls_app --scenario 1
```

### Key Numbers to Point Out:
1. **Catalog Ranking & Lookup**: Top-ranked book ID 145 (*Algorithms in C++*) with score **99.9700**; Binary Search locates ID 250 in **1 comparison** at index 249.
2. **Turnstile Peak Window**: Hours **[50..70]** identified with a net influx of **+106** visitors (Divide-and-Conquer and Kadane match).
3. **Abstract Compression & Logistics**: Huffman Coding compresses 1,248 bits to **666 bits** (**46.6% space savings**); Prim and Kruskal both yield an identical MST cost of **99 units** (7 edges).

---

## Act 3: Scenario 2 — Constraint Shock: Austerity & Fiber Cut (3:30 – 5:30)

### Terminal Command:
```bash
./build/dls_app --scenario 2
```

### Key Numbers to Point Out:
1. **Budget Austerity Resilience**: A **50.0% budget cut** ($100 to $50) results in only a **29.4% loss in academic utility** (479 down to 338).
2. **Branch-and-Bound Pruning**: Least-Cost Branch-and-Bound (LCBB) explored **21 nodes** vs FIFO Branch-and-Bound's **879 nodes** (about 40x fewer nodes).
3. **Network Fiber Cut Throughput Collapse**: Inter-branch capacity collapses from **160 Mbps to 20 Mbps** (**87.5% capacity drop**) following severed link (1, 3).

---

## Act 4: Scenario 3 — Stress Test on 100,000+ Items (5:30 – 7:15)

### Terminal Command:
```bash
./build/dls_app --scenario 3
```

### Key Numbers to Point Out:
1. **100k Sort Runtime**: Quick Sort is **about 10x faster (varies per run)** than Merge Sort (20.8 ms vs 242.4 ms) with 0 MB auxiliary buffer (in-place) vs 7.8 MB buffer.
2. **Binary Search at Scale**: Target ID 77777 located among 100,000 items in **0.001 ms** with exactly **31 comparisons** ($O(\log_2 n)$).
3. **Adversarial Pattern Matching**: KMP makes **200,005 comparisons** vs Naive's **799,944 comparisons** on 100k adversarial text (about 4x fewer comparisons), eliminating quadratic degradation.

---

## Act 5: Conclusion & Automated Benchmarks (7:15 – 8:00)

### Terminal Command:
```bash
./build/dls_bench
```

### Key Numbers to Point Out:
1. **Multi-Scale Scaling**: Benchmarks verified across **1,000**, **10,000**, and **100,000** elements across sorting, searching, MST, and Knapsack.
2. **Telemetry Export**: All execution times, comparison counts, and theoretical auxiliary space bounds are automatically exported to `data/benchmark_results.csv`.
