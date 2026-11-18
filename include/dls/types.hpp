#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <fstream>

namespace dls {

/**
 * @brief Representation of a digital library document / book / paper.
 */
struct Document {
    int id{0};
    std::string title;
    int year{0};
    double score{0.0};
    std::vector<std::string> keywords;

    bool operator==(const Document& other) const {
        return id == other.id &&
               title == other.title &&
               year == other.year &&
               score == other.score &&
               keywords == other.keywords;
    }

    bool operator!=(const Document& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Execution metrics collected during algorithm runs.
 * Every algorithm receives a reference to Metrics and increments comparisons.
 */
struct Metrics {
    double ms{0.0};              // Execution duration in milliseconds
    uint64_t comparisons{0};     // Total key comparisons made by the algorithm
    size_t peakBytes{0};         // Peak or allocated memory usage in bytes

    std::chrono::high_resolution_clock::time_point startTime;

    void reset() {
        ms = 0.0;
        comparisons = 0;
        peakBytes = 0;
    }

    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void stopTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
        ms = elapsed.count();
    }

    // Helper to count a comparison inline
    template <typename T>
    bool countLess(const T& a, const T& b) {
        comparisons++;
        return a < b;
    }

    template <typename T>
    bool countGreater(const T& a, const T& b) {
        comparisons++;
        return a > b;
    }

    template <typename T>
    bool countEqual(const T& a, const T& b) {
        comparisons++;
        return a == b;
    }
};

} // namespace dls
