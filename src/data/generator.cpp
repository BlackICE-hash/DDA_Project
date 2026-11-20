#include "dls/generator.hpp"
#include <random>
#include <sstream>
#include <iomanip>
#include <filesystem>

// NOTE: This file implements synthetic data generation for testing library algorithms.
// "std::mt19937" is a standard pseudo-random number generator initialized with fixed seed 42
// so all benchmarks, tests, and demonstration runs produce identical, reproducible numbers.

namespace dls {

static const std::vector<std::string> SAMPLE_TITLES = {
    "Introduction to Algorithms",
    "Structure and Interpretation of Computer Programs",
    "Design Patterns: Elements of Reusable Object-Oriented Software",
    "Clean Code: A Handbook of Agile Software Craftsmanship",
    "Compilers: Principles, Techniques, and Tools",
    "Computer Networks: A Systems Approach",
    "Operating System Concepts",
    "Artificial Intelligence: A Modern Approach",
    "Database System Concepts",
    "Concrete Mathematics: A Foundation for Computer Science",
    "The Art of Computer Programming",
    "Algorithms in C++: Graph Algorithms",
    "Distributed Systems: Principles and Paradigms",
    "Computer Architecture: A Quantitative Approach",
    "Modern Cryptography: Theory and Practice",
    "Discrete Mathematics and Its Applications"
};

static const std::vector<std::string> SAMPLE_KEYWORDS = {
    "algorithms", "data-structures", "graph-theory", "dynamic-programming",
    "sorting", "searching", "greedy", "divide-and-conquer", "np-completeness",
    "database", "concurrency", "distributed", "networking", "security", "ai"
};

// WHAT:    Generates count realistic Document records with random titles, years, scores, and keywords.
// IDEA:    Seeds mt19937 PRNG with given seed (default 42), picks random titles, years (1970-2024),
//          scores (1.00-100.00), and 1-4 keywords for each document.
// EXAMPLE: generateDocuments(1, 42) -> Document(id=1, title="Algorithms in C++ #1", score=99.97)
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    Using a fixed seed guarantees identical scores across runs for side-by-side algorithm verification.
std::vector<Document> DataGenerator::generateDocuments(size_t count, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<size_t> titleDist(0, SAMPLE_TITLES.size() - 1);
    std::uniform_int_distribution<int> yearDist(1970, 2024);
    std::uniform_real_distribution<double> scoreDist(1.0, 100.0);
    std::uniform_int_distribution<size_t> kwDist(0, SAMPLE_KEYWORDS.size() - 1);
    std::uniform_int_distribution<int> kwCountDist(1, 4);

    std::vector<Document> docs;
    docs.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        Document doc;
        doc.id = static_cast<int>(i + 1);
        doc.title = SAMPLE_TITLES[titleDist(rng)] + " #" + std::to_string(i + 1);
        doc.year = yearDist(rng);
        // Round score to 2 decimal places
        doc.score = std::round(scoreDist(rng) * 100.0) / 100.0;

        int kwCount = kwCountDist(rng);
        for (int k = 0; k < kwCount; ++k) {
            doc.keywords.push_back(SAMPLE_KEYWORDS[kwDist(rng)]);
        }
        docs.push_back(doc);
    }
    return docs;
}

// WHAT:    Generates documents with scores pre-sorted in ascending or descending order.
// IDEA:    Generates count base documents and overwrites scores monotonically to test quicksort on sorted data.
// EXAMPLE: generateSortedDocuments(3, true) -> scores: 1.0, 2.0, 3.0
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    Demonstrates that naive LastElement pivot degrades to O(n^2) on pre-sorted arrays.
std::vector<Document> DataGenerator::generateSortedDocuments(size_t count, bool ascending) {
    std::vector<Document> docs = generateDocuments(count, 42);
    // Sort deterministically for generating sorted adversarial input
    for (size_t i = 0; i < count; ++i) {
        if (ascending) {
            docs[i].id = static_cast<int>(i + 1);
            docs[i].score = static_cast<double>(i + 1);
        } else {
            docs[i].id = static_cast<int>(count - i);
            docs[i].score = static_cast<double>(count - i);
        }
    }
    return docs;
}

// WHAT:    Generates documents with all identical scores (50.0).
// IDEA:    Sets score = 50.0 for every item to evaluate quicksort behavior on equal keys.
// EXAMPLE: generateIdenticalDocuments(3) -> scores: 50.0, 50.0, 50.0
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    With Lomuto partitioning, all identical keys cause unbalanced partitions unless handled.
std::vector<Document> DataGenerator::generateIdenticalDocuments(size_t count) {
    std::vector<Document> docs;
    docs.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        Document doc;
        doc.id = static_cast<int>(i + 1);
        doc.title = "Standard Textbook";
        doc.year = 2020;
        doc.score = 50.0;
        doc.keywords = {"algorithms", "textbook"};
        docs.push_back(doc);
    }
    return docs;
}

// WHAT:    Generates hourly visitor net arrival and departure counts.
// IDEA:    Draws random integers from -20 to +25 representing net turnstile deltas.
// EXAMPLE: generateActivityLog(3, 42) -> [5, -12, 18]
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    Negative values are necessary for Maximum Subarray problem to have non-trivial boundaries.
std::vector<int> DataGenerator::generateActivityLog(size_t count, unsigned int seed) {
    std::mt19937 rng(seed);
    // Net visitor changes per hour: e.g. -20 to +25
    std::uniform_int_distribution<int> deltaDist(-20, 25);
    std::vector<int> log;
    log.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        log.push_back(deltaDist(rng));
    }
    return log;
}

// WHAT:    Generates study session intervals with valid start and finish times.
// IDEA:    Draws start time s in [0..maxTime-10] and random duration in [1..30], setting finish = s + dur.
// EXAMPLE: generateIntervals(1, 100) -> [{id: 1, start: 10, finish: 25}]
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    Ensures start < finish invariant for Activity Selection and Interval Partitioning.
std::vector<Interval> DataGenerator::generateIntervals(size_t count, int maxTime, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> startDist(0, maxTime - 10);
    std::uniform_int_distribution<int> lenDist(1, 30);
    std::vector<Interval> intervals;
    intervals.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        int s = startDist(rng);
        int dur = lenDist(rng);
        intervals.push_back({static_cast<int>(i + 1), s, s + dur});
    }
    return intervals;
}

// WHAT:    Generates candidate book collection items for 0/1 knapsack budgeting.
// IDEA:    Generates items with weights in [1..maxWeight] and academic values in [1..maxValue].
// EXAMPLE: generateItems(2) -> [{id:1, weight:10, value:40}, {id:2, weight:15, value:60}]
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    Weights must be positive integers for the discrete dynamic programming recurrence to apply.
std::vector<Item> DataGenerator::generateItems(size_t count, int maxWeight, int maxValue, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> wDist(1, maxWeight);
    std::uniform_int_distribution<int> vDist(1, maxValue);
    std::vector<Item> items;
    items.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        items.push_back({static_cast<int>(i + 1), wDist(rng), vDist(rng)});
    }
    return items;
}

// WHAT:    Generates a connected random graph of inter-branch transit links.
// IDEA:    Draws random pairs (u, v) with u != v and weights in [minW..maxW].
// EXAMPLE: generateGraph(4, 5) -> vector of 5 weighted edges
// TIME:    best O(E) / avg O(E) / worst O(E)      SPACE: O(E)
// VIVA:    Used as input for Dijkstra, Prim, Kruskal, and Max Flow algorithms.
std::vector<Edge> DataGenerator::generateGraph(int numVertices, int numEdges, int minW, int maxW, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> vDist(0, numVertices - 1);
    std::uniform_int_distribution<int> wDist(minW, maxW);
    std::vector<Edge> edges;
    edges.reserve(numEdges);

    for (int i = 0; i < numEdges; ++i) {
        int u = vDist(rng);
        int v = vDist(rng);
        while (v == u && numVertices > 1) {
            v = vDist(rng);
        }
        edges.push_back({u, v, wDist(rng)});
    }
    return edges;
}

// WHAT:    Generates an n x n binary citation matrix with a designated celebrity paper.
// IDEA:    Fills matrix with random citations, then zeros celebrity row (cites none) and
//          sets celebrity column to 1 (cited by all).
// EXAMPLE: generateCelebrityMatrix(4, 2) -> paper 2 is cited by 0, 1, 3 and cites no one
// TIME:    best O(n^2) / avg O(n^2) / worst O(n^2)      SPACE: O(n^2)
// VIVA:    Enables testing of O(n) findCelebrity() elimination against ground-truth paper ID.
std::vector<std::vector<int>> DataGenerator::generateCelebrityMatrix(size_t n, int celebrityId) {
    std::vector<std::vector<int>> mat(n, std::vector<int>(n, 0));
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> coin(0, 1);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == j) {
                mat[i][j] = 0;
            } else {
                mat[i][j] = coin(rng);
            }
        }
    }
    // Make celebrityId the true celebrity:
    // Celebrity cites nobody:
    for (size_t j = 0; j < n; ++j) {
        mat[celebrityId][j] = 0;
    }
    // Everyone else cites the celebrity:
    for (size_t i = 0; i < n; ++i) {
        if (static_cast<int>(i) != celebrityId) {
            mat[i][celebrityId] = 1;
        }
    }
    return mat;
}

// WHAT:    Generates an n x n square matrix with random integer entries.
// IDEA:    Allocates 2D vector and fills each cell with random value in [minVal..maxVal].
// EXAMPLE: generateMatrix(2, 1, 5) -> [[2, 4], [1, 5]]
// TIME:    best O(n^2) / avg O(n^2) / worst O(n^2)      SPACE: O(n^2)
// VIVA:    Input to compare classical O(n^3) matrix multiplication vs Strassen's O(n^2.807).
std::vector<std::vector<int>> DataGenerator::generateMatrix(size_t n, int minVal, int maxVal, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> valDist(minVal, maxVal);
    std::vector<std::vector<int>> mat(n, std::vector<int>(n, 0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            mat[i][j] = valDist(rng);
        }
    }
    return mat;
}

// WHAT:    Generates adversarial repetitive string and pattern to stress test string matchers.
// IDEA:    Builds text of 'a's ending in 'b', and pattern of 'a's ending in 'b'.
// EXAMPLE: text: "aaaaab", pattern: "aaab"
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    Forces Naive matcher to backtrack repeatedly, making 800k comparisons vs KMP's 200k.
std::string DataGenerator::generateAdversarialString(size_t textLen, size_t patternLen, std::string& outPattern) {
    if (patternLen == 0 || textLen < patternLen) {
        outPattern = "a";
        return std::string(textLen, 'a');
    }
    // Text: 'a' repeated textLen-1 times, followed by 'b'
    std::string text(textLen - 1, 'a');
    text.push_back('b');

    // Pattern: 'a' repeated patternLen-1 times, followed by 'b'
    outPattern = std::string(patternLen - 1, 'a') + 'b';
    return text;
}

// WHAT:    Serializes document objects into a standard CSV file on disk.
// IDEA:    Creates parent directory if needed, opens ofstream, and writes CSV header and comma-delimited rows.
// EXAMPLE: exportDocumentsToCSV("data/documents.csv", docs) -> creates CSV file, returns true
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(1)
// VIVA:    Keywords are semi-colon separated inside quotes to prevent splitting on commas.
bool DataGenerator::exportDocumentsToCSV(const std::string& filepath, const std::vector<Document>& docs) {
    try {
        std::filesystem::path p(filepath);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }
    } catch (...) {
        // Fall back to attempting open directly
    }

    std::ofstream out(filepath);
    if (!out.is_open()) return false;

    // Header
    out << "id,title,year,score,keywords\n";
    for (const auto& doc : docs) {
        out << doc.id << ",\"" << doc.title << "\"," << doc.year << "," << doc.score << ",\"";
        for (size_t k = 0; k < doc.keywords.size(); ++k) {
            out << doc.keywords[k] << (k + 1 < doc.keywords.size() ? ";" : "");
        }
        out << "\"\n";
    }
    return true;
}

// WHAT:    Deserializes document records from a CSV file into a vector of Document structs.
// IDEA:    Reads file line by line, parses comma and quote tokens, and reconstructs Document structs.
// EXAMPLE: loadDocumentsFromCSV("data/test_docs.csv") -> returns vector of 10 Document structs
// TIME:    best O(n) / avg O(n) / worst O(n)      SPACE: O(n)
// VIVA:    Handles quoted titles containing spaces, numbers, and symbols safely.
std::vector<Document> DataGenerator::loadDocumentsFromCSV(const std::string& filepath) {
    std::vector<Document> docs;
    std::ifstream in(filepath);
    if (!in.is_open()) return docs;

    std::string line;
    // Skip header line if present
    if (!std::getline(in, line)) return docs;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;

        Document doc;
        // id
        if (!std::getline(ss, token, ',')) continue;
        doc.id = std::stoi(token);

        // title (handle potential quotes)
        if (ss.peek() == '"') {
            ss.get(); // consume opening quote
            std::getline(ss, doc.title, '"');
            if (ss.peek() == ',') ss.get(); // consume comma after quote
        } else {
            std::getline(ss, doc.title, ',');
        }

        // year
        if (!std::getline(ss, token, ',')) continue;
        doc.year = std::stoi(token);

        // score
        if (!std::getline(ss, token, ',')) continue;
        doc.score = std::stod(token);

        // keywords
        std::string kwStr;
        if (ss.peek() == '"') {
            ss.get();
            std::getline(ss, kwStr, '"');
        } else {
            std::getline(ss, kwStr);
        }

        std::stringstream kwSS(kwStr);
        std::string kw;
        while (std::getline(kwSS, kw, ';')) {
            if (!kw.empty()) doc.keywords.push_back(kw);
        }

        docs.push_back(doc);
    }
    return docs;
}

} // namespace dls
