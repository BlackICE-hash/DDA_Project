#include "dls/types.hpp"
#include "dls/generator.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

void testMetrics() {
    dls::Metrics m;
    m.startTimer();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    m.stopTimer();

    assert(m.ms >= 5.0);
    assert(m.countLess(5, 10) == true);
    assert(m.countLess(10, 5) == false);
    assert(m.comparisons == 2);
    
    size_t rss = dls::Metrics::queryCurrentRSS();
    std::cout << "[PASS] testMetrics: elapsed=" << m.ms << "ms, comparisons=" << m.comparisons 
              << ", RSS=" << rss << " bytes\n";
}

void testGenerator() {
    auto docs1 = dls::DataGenerator::generateDocuments(10, 42);
    auto docs2 = dls::DataGenerator::generateDocuments(10, 42);
    assert(docs1.size() == 10);
    assert(docs1 == docs2);

    // Test CSV roundtrip
    const std::string csvPath = "data/test_docs.csv";
    bool exported = dls::DataGenerator::exportDocumentsToCSV(csvPath, docs1);
    assert(exported);

    auto loaded = dls::DataGenerator::loadDocumentsFromCSV(csvPath);
    assert(loaded.size() == docs1.size());
    for (size_t i = 0; i < loaded.size(); ++i) {
        assert(loaded[i].id == docs1[i].id);
        assert(loaded[i].title == docs1[i].title);
        assert(loaded[i].year == docs1[i].year);
        assert(std::abs(loaded[i].score - docs1[i].score) < 0.001);
    }

    // Test adversarial string
    std::string pattern;
    std::string text = dls::DataGenerator::generateAdversarialString(50, 5, pattern);
    assert(text.size() == 50);
    assert(pattern == "aaaab");
    assert(text.back() == 'b');

    // Test celebrity matrix
    auto celebMat = dls::DataGenerator::generateCelebrityMatrix(5, 2);
    for (size_t j = 0; j < 5; ++j) {
        assert(celebMat[2][j] == 0); // Celebrity cites nobody
    }
    for (size_t i = 0; i < 5; ++i) {
        if (i != 2) {
            assert(celebMat[i][2] == 1); // Everyone cites celebrity
        }
    }

    std::cout << "[PASS] testGenerator: documents, CSV roundtrip, adversarial data verified\n";
}

int main() {
    std::cout << "=== Running Stage 1 Tests ===\n";
    testMetrics();
    testGenerator();
    std::cout << "All Stage 1 Tests Passed Successfully!\n";
    return 0;
}
