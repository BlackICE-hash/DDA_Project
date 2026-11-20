#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <fstream>

// NOTE: "struct Document" and "struct Metrics" are C++ structs that bundle data fields
// together with member functions (like reset() or operator==) that operate directly on them.
// "namespace dls" is a named code folder grouping our project's types to avoid naming conflicts.

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

    // WHAT:    Checks if two Document structs have identical field values.
    // IDEA:    Compares id, title, year, score, and keywords one by one.
    //          Returns true only if every single field matches.
    // EXAMPLE: docA(id=1, score=90) == docB(id=1, score=90) -> true
    // TIME:    best O(1) / avg O(k) / worst O(k) where k is keyword string length   SPACE: O(1)
    // VIVA:    Change 'score == other.score' to 'std::abs(score - other.score) < 1e-6'
    //          to avoid floating-point rounding equality bugs.
    bool operator==(const Document& other) const {
        return id == other.id &&
               title == other.title &&
               year == other.year &&
               score == other.score &&
               keywords == other.keywords;
    }

    // WHAT:    Checks if two Document structs differ in any field.
    // IDEA:    Calls operator== and returns the logical opposite (NOT equal).
    // EXAMPLE: docA != docB -> true if any field differs
    // TIME:    best O(1) / avg O(k) / worst O(k)      SPACE: O(1)
    // VIVA:    Change to '!(*this == other)' ensures consistency whenever operator== changes.
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

    // WHAT:    Resets all recorded telemetry counters back to zero.
    // IDEA:    Sets elapsed milliseconds to 0.0, key comparisons to 0, and peakBytes to 0.
    // EXAMPLE: reset() sets {ms: 12.4, comparisons: 500} -> {ms: 0.0, comparisons: 0}
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Omitting 'comparisons = 0' would accumulate counts across multiple benchmark runs.
    void reset() {
        ms = 0.0;
        comparisons = 0;
        peakBytes = 0;
    }

    // WHAT:    Starts the high-resolution stopwatch.
    // IDEA:    Reads current system clock timestamp and saves it in startTime.
    // EXAMPLE: startTimer() -> startTime = 16:00:00.000000
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    If not called before stopTimer(), elapsed duration will be invalid/undefined.
    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    // WHAT:    Stops the stopwatch and records elapsed milliseconds.
    // IDEA:    Takes now() minus startTime, converts to fractional milliseconds, and stores in ms.
    // EXAMPLE: stopTimer() after 500 microseconds -> ms = 0.500
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Changing std::milli to std::micro would report microseconds instead of milliseconds.
    void stopTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
        ms = elapsed.count();
    }

    // WHAT:    Increments the comparison counter and returns whether a < b.
    // IDEA:    Adds 1 to comparisons, then performs the standard '<' check on arguments.
    // EXAMPLE: countLess(3, 7) -> returns true, comparisons incremented by 1
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    If 'comparisons++' is removed, sorting and searching comparison metrics show zero.
    template <typename T>
    bool countLess(const T& a, const T& b) {
        comparisons++;
        return a < b;
    }

    // WHAT:    Increments the comparison counter and returns whether a > b.
    // IDEA:    Adds 1 to comparisons, then performs the standard '>' check on arguments.
    // EXAMPLE: countGreater(9, 2) -> returns true, comparisons incremented by 1
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Changing '>' to '>=' would change strict inequality to non-strict, affecting ties.
    template <typename T>
    bool countGreater(const T& a, const T& b) {
        comparisons++;
        return a > b;
    }

    // WHAT:    Increments the comparison counter and returns whether a == b.
    // IDEA:    Adds 1 to comparisons, then performs the equality '==' check on arguments.
    // EXAMPLE: countEqual(5, 5) -> returns true, comparisons incremented by 1
    // TIME:    best O(1) / avg O(1) / worst O(1)      SPACE: O(1)
    // VIVA:    Used in binary search; swapping '==' with '!=' would invert search termination.
    template <typename T>
    bool countEqual(const T& a, const T& b) {
        comparisons++;
        return a == b;
    }
};

} // namespace dls
