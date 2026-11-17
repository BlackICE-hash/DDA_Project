#pragma once

#include "dls/types.hpp"
#include <string>
#include <vector>

namespace dls {

struct Interval {
    int id{0};
    int start{0};
    int finish{0}; // 'finish' time (end)

    bool operator==(const Interval& other) const {
        return id == other.id && start == other.start && finish == other.finish;
    }
};

struct Item {
    int id{0};
    int weight{0};
    int value{0};

    bool operator==(const Item& other) const {
        return id == other.id && weight == other.weight && value == other.value;
    }
};

struct Edge {
    int u{0};
    int v{0};
    int weight{0};

    bool operator==(const Edge& other) const {
        return u == other.u && v == other.v && weight == other.weight;
    }
};

class DataGenerator {
public:
    // Fixed seed generator for documents
    static std::vector<Document> generateDocuments(size_t count, unsigned int seed = 42);
    
    // Adversarial document sets for quicksort
    static std::vector<Document> generateSortedDocuments(size_t count, bool ascending = true);
    static std::vector<Document> generateIdenticalDocuments(size_t count);

    // Activity log with positive and negative net reader deltas (for max subarray)
    static std::vector<int> generateActivityLog(size_t count, unsigned int seed = 42);

    // Reading-room intervals (for Activity Selection & Interval Partitioning)
    static std::vector<Interval> generateIntervals(size_t count, int maxTime = 1000, unsigned int seed = 42);

    // Acquisitions items (for 0/1 Knapsack)
    static std::vector<Item> generateItems(size_t count, int maxWeight = 50, int maxValue = 100, unsigned int seed = 42);

    // Graph edge list (for Dijkstra, Prim, Kruskal, Flow)
    static std::vector<Edge> generateGraph(int numVertices, int numEdges, int minW = 1, int maxW = 100, unsigned int seed = 42);

    // Citation matrix where one paper is a celebrity (cited by all, cites none)
    static std::vector<std::vector<int>> generateCelebrityMatrix(size_t n, int celebrityId);

    // Square matrix for Strassen vs Classical multiplication
    static std::vector<std::vector<int>> generateMatrix(size_t n, int minVal = 0, int maxVal = 10, unsigned int seed = 42);

    // Adversarial text & pattern for string matching (e.g. "aaaa...ab" vs "aaab")
    static std::string generateAdversarialString(size_t textLen, size_t patternLen, std::string& outPattern);

    // CSV File I/O
    static bool exportDocumentsToCSV(const std::string& filepath, const std::vector<Document>& docs);
    static std::vector<Document> loadDocumentsFromCSV(const std::string& filepath);
};

} // namespace dls
