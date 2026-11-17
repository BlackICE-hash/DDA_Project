# Algorithm Complexity Analysis & Reductions

**Digital Library Search and Recommendation Engine**  
*Theoretical Foundations, Recurrence Derivations, and Formal NP-Completeness Reductions*

---

## 1. Master Theorem Reference & Derivations

The Master Theorem provides a cookbook solution for divide-and-conquer recurrences of the canonical form:

$$T(n) = a T\left(\frac{n}{b}\right) + f(n)$$

where $a \ge 1$, $b > 1$, and $f(n)$ is an asymptotically positive function representing division and combination costs. Let $c_{crit} = \log_b a$.

| Case | Condition on $f(n)$ | Solution | Intuition |
| :--- | :--- | :--- | :--- |
| **Case 1** | $f(n) = O(n^{\log_b a - \epsilon})$ for some $\epsilon > 0$ | $T(n) = \Theta(n^{\log_b a})$ | Tree leaves dominate cost |
| **Case 2** | $f(n) = \Theta(n^{\log_b a} \log^k n)$ for $k \ge 0$ | $T(n) = \Theta(n^{\log_b a} \log^{k+1} n)$ | Cost evenly spread across levels |
| **Case 3** | $f(n) = \Omega(n^{\log_b a + \epsilon})$ and regularity condition holds ($a f(n/b) \le c f(n)$ for $c < 1$) | $T(n) = \Theta(f(n))$ | Root work dominates cost |

---

### 1.1 Merge Sort

* **Recurrence:**
  $$T(n) = 2 T\left(\frac{n}{2}\right) + \Theta(n), \quad T(1) = \Theta(1)$$
* **Parameters:** $a = 2$, $b = 2$, $f(n) = \Theta(n)$.
* **Critical Exponent:**
  $$\log_b a = \log_2 2 = 1 \implies n^{\log_b a} = n^1 = n$$
* **Master Theorem Case:** $f(n) = \Theta(n) = \Theta(n^{\log_2 2} \log^0 n) \implies \text{Case 2 with } k = 0$.
* **Solution:**
  $$T(n) = \Theta(n^1 \log^{0+1} n) = \Theta(n \log n)$$
* **Recursion Tree Proof:**
  - Depth of tree: $\log_2 n$.
  - Number of nodes at level $i$: $2^i$.
  - Problem size at level $i$: $n / 2^i$.
  - Work per node at level $i$: $c \cdot (n / 2^i)$.
  - Total work at level $i$: $2^i \cdot c \cdot (n / 2^i) = c \cdot n$.
  - Total across all $\log_2 n + 1$ levels:
    $$\sum_{i=0}^{\log_2 n} c n = c n (\log_2 n + 1) = \Theta(n \log n)$$

---

### 1.2 Binary Search (Recursive)

* **Recurrence:**
  $$T(n) = T\left(\frac{n}{2}\right) + \Theta(1), \quad T(1) = \Theta(1)$$
* **Parameters:** $a = 1$, $b = 2$, $f(n) = \Theta(1) = \Theta(n^0)$.
* **Critical Exponent:**
  $$\log_b a = \log_2 1 = 0 \implies n^{\log_b a} = n^0 = 1$$
* **Master Theorem Case:** $f(n) = \Theta(1) = \Theta(n^0 \log^0 n) \implies \text{Case 2 with } k = 0$.
* **Solution:**
  $$T(n) = \Theta(\log n)$$

---

### 1.3 Maximum Subarray (Divide and Conquer)

* **Recurrence:**
  $$T(n) = 2 T\left(\frac{n}{2}\right) + \Theta(n), \quad T(1) = \Theta(1)$$
* **Parameters:** $a = 2$, $b = 2$, $f(n) = \Theta(n)$ (finding crossing subarray takes linear time).
* **Master Theorem Case:** Case 2 with $k = 0$.
* **Solution:**
  $$T(n) = \Theta(n \log n)$$

---

### 1.4 Fast Powering (Modular Exponentiation)

* **Recurrence:**
  $$T(n) = T\left(\frac{n}{2}\right) + \Theta(1), \quad T(0) = \Theta(1)$$
* **Parameters:** $a = 1$, $b = 2$, $f(n) = \Theta(1)$ (single multiplication and modulo).
* **Master Theorem Case:** Case 2 with $k = 0$.
* **Solution:**
  $$T(n) = \Theta(\log n)$$

---

### 1.5 Strassen's Matrix Multiplication

* **Classical Matrix Multiplication:**
  8 subproblems of size $n/2$:
  $$T_{class}(n) = 8 T\left(\frac{n}{2}\right) + \Theta(n^2) \implies \log_2 8 = 3 \implies T_{class}(n) = \Theta(n^{\log_2 8}) = \Theta(n^3)$$
* **Strassen's Recurrence:**
  7 subproblems of size $n/2$ with 18 matrix additions/subtractions of size $n/2 \times n/2$:
  $$T(n) = 7 T\left(\frac{n}{2}\right) + \Theta(n^2), \quad T(1) = \Theta(1)$$
* **Parameters:** $a = 7$, $b = 2$, $f(n) = \Theta(n^2)$.
* **Critical Exponent:**
  $$\log_b a = \log_2 7 \approx 2.807355$$
* **Comparison with $f(n)$:**
  $$f(n) = O(n^2) = O(n^{\log_2 7 - \epsilon}) \quad \text{for } \epsilon \approx 0.807 > 0$$
* **Master Theorem Case:** Case 1 (leaf-dominated).
* **Solution:**
  $$T(n) = \Theta(n^{\log_2 7}) \approx \Theta(n^{2.8074})$$
* **Impact:** For a $1024 \times 1024$ co-citation matrix, classical multiplication takes $\approx 1024^3 \approx 1.07 \times 10^9$ operations, whereas Strassen requires $\approx 1024^{2.807} \approx 2.64 \times 10^8$ operations—a $4\times$ reduction.

---

## 2. Non-Standard Recurrences & Recursion Trees

### 2.1 Quick Sort Analysis

#### Worst-Case (Pessimistic Partitioning 0 : $n-1$)
Occurs when the pivot is always the smallest or largest element (e.g., naive last-element pivot on an already sorted or reverse-sorted array):
$$T(n) = T(n - 1) + \Theta(n), \quad T(1) = \Theta(1)$$
Expanding by substitution:
$$T(n) = T(n - 1) + c n = T(n - 2) + c (n - 1) + c n = \dots = c \sum_{i=1}^n i = c \frac{n(n + 1)}{2} = \Theta(n^2)$$

#### Best-Case (Balanced Partitioning $n/2 : n/2$)
$$T(n) = 2 T\left(\frac{n}{2}\right) + \Theta(n) = \Theta(n \log n)$$

#### Average-Case (Random Uniform Permutations)
Under uniform input distribution, partition rank $k$ is equally likely for all $1 \le k \le n$:
$$T(n) = \frac{1}{n} \sum_{k=1}^n \left( T(k - 1) + T(n - k) \right) + \Theta(n) = \frac{2}{n} \sum_{k=0}^{n-1} T(k) + c n$$
Multiplying by $n$:
$$n T(n) = 2 \sum_{k=0}^{n-1} T(k) + c n^2$$
Subtracting $(n-1)T(n-1)$:
$$n T(n) - (n - 1)T(n - 1) = 2 T(n - 1) + 2 c n - c$$
$$n T(n) = (n + 1) T(n - 1) + 2 c n$$
Dividing by $n(n+1)$:
$$\frac{T(n)}{n + 1} = \frac{T(n - 1)}{n} + \frac{2 c}{n + 1}$$
Summing telescoping terms:
$$\frac{T(n)}{n + 1} = \Theta\left(\sum_{k=1}^n \frac{1}{k}\right) = \Theta(\ln n) \implies T(n) = \Theta(n \ln n) = \Theta(n \log n)$$

---

### 2.2 Matrix Chain Parenthesization (Catalan Recurrence)

Let $P(n)$ be the number of distinct full parenthesizations of $n$ matrices.
For $n = 1$, $P(1) = 1$.
For $n \ge 2$, the outer split can be at any position $k$ from $1$ to $n-1$:
$$P(n) = \sum_{k=1}^{n-1} P(k) P(n - k)$$
This generates the **Catalan Numbers**:
$$P(n) = C_{n-1} = \frac{1}{n} \binom{2n - 2}{n - 1} = \Omega\left(\frac{4^n}{n^{3/2}}\right)$$
*Viva Insight:* Because the number of parenthesizations grows exponentially ($\Omega(4^n / n^{3/2})$), brute-force evaluation is impossible. However, the number of distinct subproblems is only $\binom{n}{2} = \Theta(n^2)$, making Dynamic Programming ($O(n^3)$) optimal.

---

### 2.3 Traveling Salesperson Problem (Held-Karp Bitmask DP)

* Subproblems: $(S, v)$ where $S \subseteq \{0, 1, \dots, n-1\}$, $0 \in S$, $v \in S$.
* Number of subsets $S$: $2^n$.
* For a subset of size $k$, there are $k$ choices for current vertex $v$.
* Total subproblems:
  $$\sum_{k=1}^n \binom{n}{k} \cdot k = n \cdot 2^{n-1} = \Theta(n \cdot 2^n)$$
* Work per subproblem: $n$ transitions.
* Total Time Complexity:
  $$T(n) = \Theta(n^2 2^n)$$
* Comparison with Brute Force:
  For $n = 15$:
  - Brute force ($(n-1)! / 2$): $14! / 2 \approx 43,589,145,600$ paths.
  - Bitmask DP ($n^2 2^n$): $225 \times 32,768 \approx 7.37 \times 10^6$ operations ($\approx 5$ milliseconds).

---

### 2.4 Edmonds-Karp Max Flow Bound: $O(V E^2)$

* **Shortest Augmenting Path Monotonicity Lemma:**
  For all vertices $v \in V \setminus \{s, t\}$, the shortest-path distance $\delta_f(s, v)$ in residual network $G_f$ increases monotonically with each augmentation.
* **Critical Edges:**
  An edge $(u, v)$ is critical along augmenting path $P$ if $c_f(u, v) = c_f(P)$ (it is the bottleneck).
  When $(u, v)$ is critical, it disappears from $G_f$.
  To reappear, flow must be pushed in reverse direction $(v, u)$, requiring:
  $$\delta_{f'}(s, v) = \delta_{f'}(s, u) + 1$$
  Since $\delta_{f'}(s, u) \ge \delta_f(s, u)$,
  $$\delta_{f'}(s, v) \ge \delta_f(s, v) + 2$$
  Thus, between appearances as critical, distance to $u$ increases by at least 2.
* Since distance to any vertex is at most $|V| - 1$, an edge can become critical at most $(|V| - 1)/2 < |V|/2$ times.
* With $|E|$ total edges, total number of augmentations is at most $O(V E)$.
* Each augmentation takes BFS time $O(E)$.
* **Total Time:** $O(V E) \times O(E) = O(V E^2)$.

---

## 3. Formal Polynomial-Time Reduction: SUBSET-SUM $\le_P$ 0/1 KNAPSACK

This reduction proves that the decision version of 0/1 Knapsack is **NP-complete**.

```
                +------------------------------------+
                |       SUBSET-SUM Instance          |
                |   S = {s1, s2, ..., sn}, Target T  |
                +-----------------+------------------+
                                  |
                                  | Polynomial-Time Reduction f
                                  | Time: O(n), Space: O(n)
                                  v
                +------------------------------------+
                |       0/1 KNAPSACK Instance        |
                |   n items:                         |
                |     weight w_i = s_i               |
                |     value  v_i = s_i               |
                |   Capacity W = T                   |
                |   Goal Value V = T                 |
                +-----------------+------------------+
                                  |
            +---------------------+---------------------+
            |                                           |
            v                                           v
    If Answer is YES                             If Answer is NO
Subset sum S' equals T                    No subset sum equals T
==> Knapsack value >= T                   ==> Max knapsack value < T
```

### 3.1 Problem Definitions

#### Problem 1: SUBSET-SUM (Decision Version)
* **Instance:** A finite set of positive integers $S = \{s_1, s_2, \dots, s_n\}$ and a target positive integer $T$.
* **Question:** Does there exist a subset $S' \subseteq S$ such that:
  $$\sum_{s_i \in S'} s_i = T$$

#### Problem 2: 0/1 KNAPSACK (Decision Version)
* **Instance:** A set of $n$ items where each item $i$ has weight $w_i \in \mathbb{Z}^+$ and value $v_i \in \mathbb{Z}^+$, maximum capacity $W \in \mathbb{Z}^+$, and target threshold value $V \in \mathbb{Z}^+$.
* **Question:** Does there exist a subset of items $I \subseteq \{1, 2, \dots, n\}$ such that:
  $$\sum_{i \in I} w_i \le W \quad \text{and} \quad \sum_{i \in I} v_i \ge V$$

---

### 3.2 Construction of the Reduction Function $f$

Given an arbitrary instance $\langle S, T \rangle$ of SUBSET-SUM with $S = \{s_1, s_2, \dots, s_n\}$, we construct in polynomial time an instance $\langle \text{Items}, W, V \rangle$ of 0/1 KNAPSACK as follows:

1. For each element $s_i \in S$, create a knapsack item $i$ with:
   $$w_i = s_i$$
   $$v_i = s_i$$
2. Set the knapsack capacity:
   $$W = T$$
3. Set the target knapsack value:
   $$V = T$$

---

### 3.3 Complexity of the Reduction

* Creating $n$ items takes $O(n)$ steps.
* Assigning weights, values, capacity, and target value takes $O(n)$ space and time.
* Hence, $f$ runs in linear time $O(n)$, which is strictly polynomial in the size of the input:
  $$f \in \text{poly}(|S|, \log T)$$

---

### 3.4 Proof of Correctness (Biconditional $\iff$)

To prove that the reduction is valid, we must prove:
$$\langle S, T \rangle \in \text{SUBSET-SUM} \iff f(\langle S, T \rangle) \in \text{0/1-KNAPSACK}$$

#### ($\implies$) Forward Direction:
Assume $\langle S, T \rangle \in \text{SUBSET-SUM}$.
* Then there exists a subset $S' \subseteq S$ such that:
  $$\sum_{s_i \in S'} s_i = T$$
* In the constructed knapsack instance, select the index subset $I = \{i \mid s_i \in S'\}$.
* Evaluate total weight:
  $$\sum_{i \in I} w_i = \sum_{s_i \in S'} s_i = T \le W \quad (\text{since } W = T)$$
  Thus, capacity constraint is satisfied.
* Evaluate total value:
  $$\sum_{i \in I} v_i = \sum_{s_i \in S'} s_i = T \ge V \quad (\text{since } V = T)$$
  Thus, target value threshold is achieved.
* Therefore, $f(\langle S, T \rangle) \in \text{0/1-KNAPSACK}$.

#### ($\impliedby$) Backward Direction:
Assume $f(\langle S, T \rangle) \in \text{0/1-KNAPSACK}$.
* Then there exists a subset of items $I \subseteq \{1, 2, \dots, n\}$ such that:
  $$\sum_{i \in I} w_i \le W \quad \text{and} \quad \sum_{i \in I} v_i \ge V$$
* Substituting the reduction definitions $w_i = s_i$, $v_i = s_i$, $W = T$, and $V = T$:
  $$\sum_{i \in I} s_i \le T \quad \text{and} \quad \sum_{i \in I} s_i \ge T$$
* The only way a real number can be simultaneously $\le T$ and $\ge T$ is if it is **identically equal** to $T$:
  $$\sum_{i \in I} s_i = T$$
* Let $S' = \{s_i \mid i \in I\}$. Then $S' \subseteq S$ and $\sum_{s_i \in S'} s_i = T$.
* Therefore, $\langle S, T \rangle \in \text{SUBSET-SUM}$.

$$\text{Q.E.D.}$$

---

### 3.5 Viva Discussion: Pseudo-Polynomial Time vs NP-Completeness

* **Viva Question:** "If 0/1 Knapsack is NP-complete, why can our DP algorithm solve it in $O(n W)$?"
* **Model Answer:**
  "$O(n W)$ is **pseudo-polynomial time**, not polynomial time. The input length in bits to specify capacity $W$ is $k = \lceil \log_2 W \rceil$. Thus, $W = 2^k$. In terms of the true input size $k$, the runtime is $O(n \cdot 2^k)$, which is **exponential** in the length of the binary representation of $W$.
  
  If $W$ is polynomially bounded in $n$ (unary encoding or small numbers), DP runs quickly. But if $W$ is a large 64-bit integer ($W \approx 10^{15}$), DP requires petabytes of memory and trillions of operations, where Branch-and-Bound with fractional relaxation is indispensable."
