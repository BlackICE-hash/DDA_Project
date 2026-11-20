# C++ Features Used in This Project (Quick Guide)

This guide explains the C++ features in this project for someone who knows basic C (functions, loops, arrays, structs, if/else, pointers) and a little `std::vector`.

---

## 1. Struct with Functions Inside

* **Plain English Meaning:** In basic C, a `struct` only holds data variables. In C++, a `struct` can also have functions inside it that work directly on its own variables.
* **Closest Thing in Basic C++:** A normal struct plus standalone functions that take a pointer to the struct: `void reset(Metrics* m)`.
* **Real Line from Repo:** `include/dls/types.hpp:41`
  ```cpp
  struct Metrics {
      double ms{0.0};
      uint64_t comparisons{0};
      void reset() { ms = 0.0; comparisons = 0; }
  };
  ```
* **Viva Answer (if asked):** *"In C++, putting a function inside a struct is just convenient syntax so `m.reset()` can directly modify the fields of `m` without passing a pointer."*

---

## 2. Class, Public, and Private

* **Plain English Meaning:** A `class` is just a `struct` where variables under `private:` cannot be touched from outside the class. Only functions listed under `public:` can touch them.
* **Closest Thing in Basic C++:** A `struct` where you promise not to touch certain fields directly, only calling helper functions to modify them.
* **Real Line from Repo:** `include/dls/heap.hpp:76`
  ```cpp
  class DistMinHeap {
  private:
      std::vector<DistHeapNode> data_;
  public:
      void push(const DistHeapNode& item);
  };
  ```
* **Viva Answer (if asked):** *"We make `data_` private so outside code cannot accidentally break the min-heap tree property, forcing all changes through `push()` and `pop()`."*

---

## 3. Constructor

* **Plain English Meaning:** A special function that runs automatically the moment a class or struct variable is created. It has the exact same name as the class and no return type.
* **Closest Thing in Basic C++:** An `init()` function like `void init_set(DisjointSet* ds, int n)` that you must remember to call right after creating a variable.
* **Real Line from Repo:** `include/dls/disjoint_set.hpp:29`
  ```cpp
  explicit DisjointSet(int n, Metrics* m = nullptr)
      : parent(n), rank(n, 0), metrics_(m) { ... }
  ```
* **Viva Answer (if asked):** *"The constructor automatically initializes array sizes and default values so an object is never left in an uninitialized state."*

---

## 4. `template <typename T>`

* **Plain English Meaning:** A blueprint that tells the compiler: "write a copy of this function for any data type `T` (like `int`, `double`, or `Document`) when the code calls it."
* **Closest Thing in Basic C++:** Writing two identical functions with different names: `sort_ints(int* arr)` and `sort_docs(Document* arr)`.
* **Real Line from Repo:** `include/dls/search.hpp:25`
  ```cpp
  template <typename T>
  int binarySearchIterative(const std::vector<T>& arr, const T& target, Metrics& metrics);
  ```
* **Viva Answer (if asked):** *"Templates let us write one search algorithm that works on arrays of numbers, strings, or library documents without rewriting code."*

---

## 5. Comparator Structs with `operator()`

* **Plain English Meaning:** A small struct that defines a function called `operator()`. It lets you pass comparison logic (like sorting ascending vs descending) into a sorting function as if the struct were a function.
* **Closest Thing in Basic C++:** A function pointer passed into `qsort()`: `int compare(const void* a, const void* b)`.
* **Real Line from Repo:** `include/dls/sort.hpp:25`
  ```cpp
  struct DocumentScoreDesc {
      bool operator()(const Document& a, const Document& b) const {
          return a.score > b.score;
      }
  };
  ```
* **Viva Answer (if asked):** *"It tells the sort function how to compare two elements; `a.score > b.score` sorts documents by highest relevance score first."*

---

## 6. `auto`

* **Plain English Meaning:** Tells the compiler: "you already know the type of the right-hand side, so figure out the variable type for me."
* **Closest Thing in Basic C++:** Writing the full type name explicitly (e.g. `int x = 5;` instead of `auto x = 5;`).
* **Real Line from Repo:** `include/dls/types.hpp:59`
  ```cpp
  auto endTime = std::chrono::high_resolution_clock::now();
  ```
* **Viva Answer (if asked):** *"`auto` avoids typing very long compiler type names like `std::chrono::time_point<...>` when the type is already obvious."*

---

## 7. `static_cast<int>`

* **Plain English Meaning:** Converts a value safely from one numeric type to another (e.g., from an unsigned `size_t` size to a regular signed `int`).
* **Closest Thing in Basic C++:** Old C-style casting: `(int)items.size()`.
* **Real Line from Repo:** `src/bb/bb.cpp:171`
  ```cpp
  int n = static_cast<int>(items.size());
  ```
* **Viva Answer (if asked):** *"`static_cast<int>` is modern C++ syntax for `(int)size`, preventing compiler warnings about comparing signed and unsigned numbers."*

---

## 8. `namespace dls` and `::`

* **Plain English Meaning:** A namespace is a named folder for code. It prevents name collisions. `::` is the scope operator: `folder::function` means "look inside this folder for this function."
* **Closest Thing in Basic C++:** Adding a prefix to function names, like `dls_binarySearch()`.
* **Real Line from Repo:** `include/dls/bb.hpp:8`
  ```cpp
  namespace dls {
  ```
  and `app/main.cpp:420`:
  ```cpp
  dls::DataGenerator::exportDocumentsToCSV(...);
  ```
* **Viva Answer (if asked):** *"`namespace dls` groups all library engine functions so they don't clash with standard C++ functions having the same name."*

---

## 9. `std::vector` (push_back, size, indexing, 2D vectors)

* **Plain English Meaning:** A dynamic array that grows automatically when you add items.
  * `.push_back(x)`: appends item `x` to the end.
  * `.size()`: returns how many items are currently in the array.
  * `v[i]`: accesses the item at index `i` (0-indexed, like basic arrays).
  * `std::vector<std::vector<int>>`: a 2D matrix (a vector of rows).
* **Closest Thing in Basic C++:** A dynamically allocated array with `malloc()` and `realloc()`, plus an integer keeping track of count.
* **Real Line from Repo:** `include/dls/dc.hpp:86`
  ```cpp
  int findCelebrity(const std::vector<std::vector<int>>& citationMatrix, Metrics& metrics);
  ```
* **Viva Answer (if asked):** *"`std::vector` manages its own memory on the heap, preventing memory leaks when the array goes out of scope."*

---

## 10. `std::pair<T1, T2>`

* **Plain English Meaning:** A simple struct that holds two values together. The first is `.first` and the second is `.second`.
* **Closest Thing in Basic C++:** A custom struct: `struct Pair { int first; int second; };`.
* **Real Line from Repo:** `include/dls/flow.hpp:39`
  ```cpp
  std::vector<std::pair<int, int>> pairs;
  ```
* **Viva Answer (if asked):** *"`std::pair` is a standard two-item package; in bipartite matching, `.first` is the student ID and `.second` is the book copy ID."*

---

## 11. `std::map<Key, Value>`

* **Plain English Meaning:** A lookup table (binary search tree) that stores key-value pairs sorted by key. Looking up or inserting an element takes $O(\log n)$ time.
* **Closest Thing in Basic C++:** Two parallel sorted arrays or a binary search tree of structs.
* **Real Line from Repo:** `src/greedy/greedy.cpp:124`
  ```cpp
  std::map<char, int> freqMap;
  ```
* **Viva Answer (if asked):** *"In Huffman coding, `freqMap[c]` maps each character `c` to its integer frequency count in $O(\log k)$ time."*

---

## 12. `enum class` (PivotStrategy, BBStrategy)

* **Plain English Meaning:** A named list of integer options that prevents mixing up different option categories.
* **Closest Thing in Basic C++:** `#define FIRST 0`, `#define MEDIAN 1`, or a plain C `enum`.
* **Real Line from Repo:** `include/dls/sort.hpp:73`
  ```cpp
  enum class PivotStrategy { First, Last, Random, MedianOfThree };
  ```
* **Viva Answer (if asked):** *"`enum class` gives type-safe names for choices, preventing us from passing an invalid integer where a strategy is expected."*

---

## 13. `nullptr`

* **Plain English Meaning:** A clean, type-safe keyword representing the null pointer (address 0).
* **Closest Thing in Basic C++:** `NULL` or `0`.
* **Real Line from Repo:** `include/dls/disjoint_set.hpp:26`
  ```cpp
  Metrics* metrics_{nullptr};
  ```
* **Viva Answer (if asked):** *"`nullptr` is the modern C++ replacement for `NULL`; it cannot accidentally be confused with integer 0."*

---

## 14. `std::swap(a, b)`

* **Plain English Meaning:** Swaps the contents of variable `a` and variable `b`.
* **Closest Thing in Basic C++:** `{ auto temp = a; a = b; b = temp; }`.
* **Real Line from Repo:** `include/dls/heap.hpp:86`
  ```cpp
  std::swap(data_[index], data_[parent]);
  ```
* **Viva Answer (if asked):** *"`std::swap` exchanges the values of two variables in three fast operations using temporary moves."*

---

## 15. `std::move(x)`

* **Plain English Meaning:** Tells C++: "I am done using `x`; instead of making a slow copy of its data, just transfer ownership of its data buffer to the new variable."
* **Closest Thing in Basic C++:** Stealing a pointer: `dest = src; src = NULL;`.
* **Real Line from Repo:** `include/dls/heap.hpp:136`
  ```cpp
  data_[0] = std::move(data_.back());
  ```
* **Viva Answer (if asked):** *"`std::move` avoids a deep copy by transferring ownership of the last heap node into the root index."*

---

## 16. `std::chrono` Timing

* **Plain English Meaning:** The standard C++ library for high-precision time measurements (nanoseconds and milliseconds).
* **Closest Thing in Basic C++:** C's `clock()` or `gettimeofday()`.
* **Real Line from Repo:** `include/dls/types.hpp:55`
  ```cpp
  startTime = std::chrono::high_resolution_clock::now();
  ```
* **Viva Answer (if asked):** *"`std::chrono::high_resolution_clock` provides cross-platform, sub-microsecond wall-clock timestamps."*

---

## 17. References (`&`) and `const` References (`const &`)

* **Plain English Meaning:**
  * `T& x`: an alias (nickname) for an existing variable. Changing `x` changes the original, without copying pointers or dereferencing with `*`.
  * `const T& x`: passes the original variable by reference so it is **not copied**, but guarantees the function will **not modify** it.
* **Closest Thing in Basic C++:** Passing a pointer: `const T* x`.
* **Real Line from Repo:** `include/dls/bb.hpp:97`
  ```cpp
  BBKnapsackResult knapsackBranchAndBound(const std::vector<Item>& items, int capacity, BBStrategy strategy, Metrics& metrics);
  ```
* **Viva Answer (if asked):** *"`const std::vector<Item>&` passes thousands of items without a slow memory copy, while `Metrics&` lets the function record comparison counts directly into our tracker."*
