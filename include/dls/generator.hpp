#pragma once

#include "dls/types.hpp"
#include <string>
#include <vector>

// NOTE: "class DataGenerator" is a stateless utility class containing only static functions.
// In C++, "static" means the function belongs to the class itself, so you call it as
// DataGenerator::generateDocuments() without creating a class instance.

namespace dls {

struct Interval {
    int id{0};
    int start{0};
    int finish{0}; // 'finish' time (end)

    // WHAT:    Checks equality between two study-room intervals.
    // IDEA:    Returns true if id, start, and finish times all match.
    // EXAMPLE: i1(id=1, start=2, finish=5) == i2(id=1, start=2, finish=5) -> true
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Swapping finish with start would invert interval duration checks.
    bool operator==(const Interval& other) const {
        return id == other.id && start == other.start && finish == other.finish;
    }
};

struct Item {
    int id{0};
    int weight{0};
    int value{0};

    // WHAT:    Checks equality between two knapsack acquisition items.
    // IDEA:    Returns true if id, weight, and value all match.
    // EXAMPLE: itemA(id=2, weight=10, value=30) == itemB(id=2, weight=10, value=30) -> true
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Used in unit tests to verify item reconstruction after dynamic programming.
    bool operator==(const Item& other) const {
        return id == other.id && weight == other.weight && value == other.value;
    }
};

struct Edge {
    int u{0};
    int v{0};
    int weight{0};

    // WHAT:    Checks equality between two directed or undirected graph edges.
    // IDEA:    Returns true if endpoint u, endpoint v, and edge weight all match.
    // EXAMPLE: e1(0, 1, 15) == e2(0, 1, 15) -> true
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    For undirected graphs, an edge (u, v) could also equal (v, u) if symmetric comparison is needed.
    bool operator==(const Edge& other) const {
        return u == other.u && v == other.v && weight == other.weight;
    }
};

// CLASS: DataGenerator
// DATA STORED: None (stateless utility class of static factory methods).
// PUBLIC METHODS:
//   - generateDocuments: Produces n pseudo-random Document structs using fixed seed 42; O(n).
//   - generateSortedDocuments: Generates documents pre-sorted by score (ascending or descending); O(n).
//   - generateIdenticalDocuments: Generates documents with identical scores for quicksort testing; O(n).
//   - generateActivityLog: Produces net reader arrival/departure deltas for max subarray; O(n).
//   - generateIntervals: Creates study session intervals with valid start < finish times; O(n).
//   - generateItems: Produces acquisition candidates with randomized weights and values; O(n).
//   - generateGraph: Constructs connected random edge lists for network algorithms; O(V + E).
//   - generateCelebrityMatrix: Generates an n x n binary citation matrix with a designated celebrity; O(n^2).
//   - generateMatrix: Generates an n x n integer matrix for Strassen multiplication; O(n^2).
//   - generateAdversarialString: Builds repeated 'a' strings ending in 'b' to stress KMP vs Naive; O(n).
//   - exportDocumentsToCSV: Serializes documents into a CSV file format; O(n).
//   - loadDocumentsFromCSV: Deserializes documents from a CSV file back into vectors; O(n).
class DataGenerator {
public:
    // WHAT:    Generates a list of realistic synthetic catalog documents.
    // IDEA:    Uses a linear congruential generator initialized with seed (default 42) to produce reproducible scores and titles.
    // EXAMPLE: generateDocuments(3, 42) -> vector of 3 documents with deterministic titles/scores
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n) for returned vector
    // VIVA:    Changing seed=42 to another number produces a different reproducible test dataset.
    static std::vector<Document> generateDocuments(size_t count, unsigned int seed = 42);
    
    // WHAT:    Generates documents with scores already sorted in ascending or descending order.
    // IDEA:    Sets score proportional to index i, creating pre-sorted input to test quicksort pivot strategies.
    // EXAMPLE: generateSortedDocuments(3, true) -> scores: 1.0, 2.0, 3.0
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
    // VIVA:    Used to demonstrate why first-element pivot degrades to O(n^2) on sorted data.
    static std::vector<Document> generateSortedDocuments(size_t count, bool ascending = true);

    // WHAT:    Generates documents with all identical relevance scores.
    // IDEA:    Fills count documents with score 50.0 to test quicksort partitioning on equal keys.
    // EXAMPLE: generateIdenticalDocuments(3) -> scores: 50.0, 50.0, 50.0
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
    // VIVA:    Exposes Lomuto partitioning quadratic degradation if pivot is equal to all elements.
    static std::vector<Document> generateIdenticalDocuments(size_t count);

    // WHAT:    Generates net visitor influx/outflux values for library turnstiles.
    // IDEA:    Generates alternating positive and negative integers representing hourly turnstile traffic.
    // EXAMPLE: generateActivityLog(4, 42) -> [5, -8, 12, 4]
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
    // VIVA:    The resulting array is input to Maximum Subarray (Divide-and-Conquer vs Kadane).
    static std::vector<int> generateActivityLog(size_t count, unsigned int seed = 42);

    // WHAT:    Generates reading room reservation intervals.
    // IDEA:    Draws random start times and adds positive durations so start < finish is guaranteed.
    // EXAMPLE: generateIntervals(2, 100) -> [{start: 10, finish: 25}, {start: 30, finish: 45}]
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
    // VIVA:    Input to Activity Selection and Interval Partitioning algorithms.
    static std::vector<Interval> generateIntervals(size_t count, int maxTime = 1000, unsigned int seed = 42);

    // WHAT:    Generates candidate book collection items for acquisition budgeting.
    // IDEA:    Generates items with random positive integer weights and monetary values.
    // EXAMPLE: generateItems(2) -> [{weight: 12, value: 45}, {weight: 8, value: 30}]
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
    // VIVA:    Input to 0/1 Knapsack DP and Branch-and-Bound algorithms.
    static std::vector<Item> generateItems(size_t count, int maxWeight = 50, int maxValue = 100, unsigned int seed = 42);

    // WHAT:    Generates a connected weighted graph representing inter-branch library transit routes.
    // IDEA:    Builds a random backbone tree to guarantee connectivity, then adds remaining random edges.
    // EXAMPLE: generateGraph(4, 5) -> 5 weighted edges connecting 4 vertices
    // TIME:    best O(V + E) / avg O(V + E) / worst O(V + E)      SPACE: O(E)
    // VIVA:    Backbone tree ensures no disconnected components that would break Dijkstra or Prim.
    static std::vector<Edge> generateGraph(int numVertices, int numEdges, int minW = 1, int maxW = 100, unsigned int seed = 42);

    // WHAT:    Generates an n x n binary citation matrix with a designated celebrity paper.
    // IDEA:    Sets celebrity row to all 0s (cites no one) and celebrity column to all 1s (cited by all).
    // EXAMPLE: generateCelebrityMatrix(3, 1) -> row 1 has only 0s, column 1 has all 1s (except self)
    // TIME:    best O(n^2) / avg O(n^2) / worst O(n^2)      SPACE: O(n^2)
    // VIVA:    Used by findCelebrity() elimination algorithm to prove O(n) query discovery.
    static std::vector<std::vector<int>> generateCelebrityMatrix(size_t n, int celebrityId);

    // WHAT:    Generates an n x n matrix of integers for matrix multiplication benchmarking.
    // IDEA:    Populates an n x n 2D vector with random integers between minVal and maxVal.
    // EXAMPLE: generateMatrix(2) -> [[3, 7], [1, 5]]
    // TIME:    best O(n^2) / avg O(n^2) / worst O(n^2)      SPACE: O(n^2)
    // VIVA:    Input to Strassen submatrix multiplication vs classical O(n^3) multiplication.
    static std::vector<std::vector<int>> generateMatrix(size_t n, int minVal = 0, int maxVal = 10, unsigned int seed = 42);

    // WHAT:    Generates an adversarial string to trigger worst-case behavior in naive string matching.
    // IDEA:    Constructs text of 'a's ending in 'b', and pattern of 'a's ending in 'b'.
    // EXAMPLE: text: "aaaaab", pattern: "aaab"
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
    // VIVA:    Forces Naive matcher to make O(n * m) comparisons, while KMP makes only O(n + m).
    static std::string generateAdversarialString(size_t textLen, size_t patternLen, std::string& outPattern);

    // WHAT:    Exports a list of documents to a disk CSV file.
    // IDEA:    Iterates through vector and writes id,title,year,score,keywords separated by commas.
    // EXAMPLE: exportDocumentsToCSV("data/documents.csv", docs) -> creates file on disk, returns true
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(1)
    // VIVA:    Menu option 9 calls this function to export catalog records.
    static bool exportDocumentsToCSV(const std::string& filepath, const std::vector<Document>& docs);

    // WHAT:    Loads document records from a CSV file into a vector.
    // IDEA:    Reads file line by line, parses comma-separated tokens, and reconstructs Document structs.
    // EXAMPLE: loadDocumentsFromCSV("data/test_docs.csv") -> returns vector of Document objects
    // TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n) for returned vector
    // VIVA:    Used in unit test test_stage1 to verify CSV round-trip fidelity.
    static std::vector<Document> loadDocumentsFromCSV(const std::string& filepath);
};

} // namespace dls
