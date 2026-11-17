#include "dls/greedy.hpp"
#include "dls/generator.hpp"
#include <iostream>
#include <cassert>
#include <algorithm> // Oracle only in tests!

void testActivitySelection() {
    std::cout << "[RUNNING] testActivitySelection...\n";
    dls::Metrics m;

    // Edge case: Empty
    auto emptyRes = dls::selectActivities({}, m);
    assert(emptyRes.empty());

    // Classic textbook intervals:
    // (1,4), (3,5), (0,6), (5,7), (3,9), (5,9), (6,10), (8,11), (8,12), (2,14), (12,16)
    // Max mutually compatible: 4 activities, e.g. (1,4), (5,7), (8,11), (12,16)
    std::vector<dls::Interval> acts = {
        {1, 1, 4}, {2, 3, 5}, {3, 0, 6}, {4, 5, 7}, {5, 3, 9},
        {6, 5, 9}, {7, 6, 10}, {8, 8, 11}, {9, 8, 12}, {10, 2, 14}, {11, 12, 16}
    };

    auto selected = dls::selectActivities(acts, m);
    assert(selected.size() == 4);
    // Verify no two selected intervals overlap
    for (size_t i = 1; i < selected.size(); ++i) {
        assert(selected[i].start >= selected[i - 1].finish);
    }

    std::cout << "[PASS] testActivitySelection passed (selected=" << selected.size() << ")\n";
}

void testIntervalPartitioning() {
    std::cout << "[RUNNING] testIntervalPartitioning...\n";
    dls::Metrics m;

    // Edge case: Empty
    auto emptyPart = dls::partitionIntervals({}, m);
    assert(emptyPart.roomsUsed == 0 && emptyPart.assignment.empty());

    // 3 completely mutually overlapping intervals: requires 3 rooms
    std::vector<dls::Interval> allOverlap = {
        {1, 1, 10}, {2, 2, 9}, {3, 3, 8}
    };
    auto part3 = dls::partitionIntervals(allOverlap, m);
    assert(part3.roomsUsed == 3);

    // Non-overlapping intervals: requires 1 room
    std::vector<dls::Interval> chain = {
        {1, 1, 3}, {2, 3, 5}, {3, 5, 7}
    };
    auto part1 = dls::partitionIntervals(chain, m);
    assert(part1.roomsUsed == 1);

    // Generated reading-room schedule
    auto intervals = dls::DataGenerator::generateIntervals(50, 100, 42);
    auto res = dls::partitionIntervals(intervals, m);
    assert(res.roomsUsed > 0);

    // Verify valid room assignment: no two intervals in the same room overlap
    std::vector<std::vector<dls::Interval>> roomSchedules(res.roomsUsed + 1);
    for (const auto& assign : res.assignment) {
        int iId = assign.first;
        int rId = assign.second;
        // find interval
        for (const auto& iv : intervals) {
            if (iv.id == iId) {
                roomSchedules[rId].push_back(iv);
                break;
            }
        }
    }

    for (int r = 1; r <= res.roomsUsed; ++r) {
        auto& list = roomSchedules[r];
        std::sort(list.begin(), list.end(), [](const dls::Interval& a, const dls::Interval& b) {
            return a.start < b.start;
        });
        for (size_t i = 1; i < list.size(); ++i) {
            assert(list[i].start >= list[i - 1].finish);
        }
    }

    std::cout << "[PASS] testIntervalPartitioning passed (roomsUsed=" << res.roomsUsed << ")\n";
}

void testHuffmanCoding() {
    std::cout << "[RUNNING] testHuffmanCoding...\n";
    dls::Metrics m;

    std::string text = "digital library search and recommendation engine";
    auto res = dls::huffmanEncode(text, m);

    assert(!res.encodedBits.empty());
    assert(res.decodedText == text);

    // Verify prefix-free property: no code is a prefix of any other code
    for (const auto& p1 : res.codebook) {
        for (const auto& p2 : res.codebook) {
            if (p1.first != p2.first) {
                // p1.second should not be a prefix of p2.second
                assert(p2.second.find(p1.second) != 0);
            }
        }
    }

    // Compression ratio check: 8 bits/char vs encoded bits length
    size_t rawBits = text.size() * 8;
    size_t compressedBits = res.encodedBits.size();
    assert(compressedBits < rawBits);

    std::cout << "[PASS] testHuffmanCoding passed (rawBits=" << rawBits 
              << ", compressedBits=" << compressedBits 
              << ", ratio=" << (double)compressedBits / rawBits << ")\n";
}

void testDijkstra() {
    std::cout << "[RUNNING] testDijkstra...\n";
    dls::Metrics m;

    // Graph with 5 vertices
    // 0 -> 1 (w=4), 0 -> 2 (w=2)
    // 1 -> 2 (w=3), 1 -> 3 (w=2), 1 -> 4 (w=3)
    // 2 -> 1 (w=1), 2 -> 3 (w=4), 2 -> 4 (w=5)
    // 3 -> 4 (w=1)
    std::vector<dls::Edge> edges = {
        {0, 1, 4}, {0, 2, 2},
        {1, 2, 3}, {1, 3, 2}, {1, 4, 3},
        {2, 1, 1}, {2, 3, 4}, {2, 4, 5},
        {3, 4, 1}
    };

    auto res = dls::dijkstraShortestPath(5, edges, 0, m);

    // Expected shortest paths from 0:
    // 0: 0
    // 1: 0 -> 2 -> 1 = 2 + 1 = 3
    // 2: 0 -> 2 = 2
    // 3: 0 -> 2 -> 1 -> 3 = 2 + 1 + 2 = 5
    // 4: 0 -> 2 -> 1 -> 3 -> 4 = 5 + 1 = 6
    assert(res.distances[0] == 0);
    assert(res.distances[1] == 3);
    assert(res.distances[2] == 2);
    assert(res.distances[3] == 5);
    assert(res.distances[4] == 6);

    std::cout << "[PASS] testDijkstra passed\n";
}

void testPrimAndKruskalMST() {
    std::cout << "[RUNNING] testPrimAndKruskalMST...\n";
    dls::Metrics mPrim;
    dls::Metrics mKruskal;

    // Connected graph with 6 vertices:
    // 0-1 (w=4), 0-2 (w=4)
    // 1-2 (w=2)
    // 2-3 (w=3), 2-4 (w=2), 2-5 (w=4)
    // 3-4 (w=3)
    // 4-5 (w=3)
    std::vector<dls::Edge> edges = {
        {0, 1, 4}, {0, 2, 4},
        {1, 2, 2},
        {2, 3, 3}, {2, 4, 2}, {2, 5, 4},
        {3, 4, 3},
        {4, 5, 3}
    };

    auto primRes = dls::primMST(6, edges, mPrim);
    auto kruskalRes = dls::kruskalMST(6, edges, mKruskal);

    assert(primRes.connected);
    assert(kruskalRes.connected);
    assert(primRes.totalWeight == kruskalRes.totalWeight);
    assert(primRes.mstEdges.size() == 5);
    assert(kruskalRes.mstEdges.size() == 5);
    // Expected MST weight: 0-1(4) + 1-2(2) + 2-4(2) + 4-3(3) + 4-5(3) = 14
    assert(primRes.totalWeight == 14);

    // Disconnected graph test
    std::vector<dls::Edge> discEdges = {{0, 1, 5}, {2, 3, 10}};
    auto primDisc = dls::primMST(4, discEdges, mPrim);
    auto kruskalDisc = dls::kruskalMST(4, discEdges, mKruskal);
    assert(!primDisc.connected);
    assert(!kruskalDisc.connected);

    std::cout << "[PASS] testPrimAndKruskalMST passed (MST Total Weight=" << primRes.totalWeight << ")\n";
}

int main() {
    std::cout << "=== Running Stage 4 Greedy Algorithms Tests ===\n";
    testActivitySelection();
    testIntervalPartitioning();
    testHuffmanCoding();
    testDijkstra();
    testPrimAndKruskalMST();
    std::cout << "All Stage 4 Tests Passed Successfully!\n";
    return 0;
}
