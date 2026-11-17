#include "dls/generator.hpp"
#include <random>
#include <sstream>
#include <iomanip>
#include <filesystem>

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
