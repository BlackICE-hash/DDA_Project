#pragma once

#include "dls/sort.hpp"
#include <utility>

// NOTE: ".inl" files contain C++ template function implementations that must be visible
// to the compiler in header files so it can generate code for any data type T.

namespace dls {

namespace detail {

// WHAT:    Merges two adjacent sorted subarrays [left..mid] and [mid+1..right] into sorted order.
// IDEA:    Copies elements into an auxiliary buffer, then uses two pointers (i for left half,
//          j for right half) to compare elements and write the smaller one back into arr[k].
// EXAMPLE: Left [27, 38] and Right [3, 43] -> arr becomes [3, 27, 38, 43]
// TIME:    best O(n) / avg O(n) / worst O(n) where n = right - left + 1      SPACE: O(n) buffer
// VIVA:    Changing 'comp(buffer[i], buffer[j])' to '<=' vs '<' determines stability for equal elements.
template <typename T, typename Comp>
void merge(std::vector<T>& arr, std::vector<T>& buffer, int left, int mid, int right, Metrics& metrics, Comp comp) {
    // WHY: We copy elements into the auxiliary buffer so we can safely overwrite arr[k] in-place.
    for (int idx = left; idx <= right; ++idx) {
        buffer[idx] = arr[idx]; // Copy original element into temporary workspace buffer
    }

    // WHY: i tracks our reading position in the left sorted half [left..mid].
    int i = left;

    // WHY: j tracks our reading position in the right sorted half [mid+1..right].
    int j = mid + 1;

    // WHY: k tracks our writing position in the destination array arr[left..right].
    int k = left;

    // WHY: While BOTH halves still have unpicked elements, compare their front elements.
    while (i <= mid && j <= right) {
        metrics.comparisons++; // WHY: Count every key comparison for academic telemetry

        // WHY: comp returns true if left element should come before right element.
        if (comp(buffer[i], buffer[j])) {
            // WHY: Left element is smaller; place it at arr[k], then advance i and k.
            arr[k++] = buffer[i++];
        } else {
            // WHY: Right element is smaller or equal; place it at arr[k], then advance j and k.
            arr[k++] = buffer[j++];
        }
    }

    // WHY: If left half still has remaining elements, copy them all into destination.
    // (They are already sorted and greater than everything placed in arr so far).
    while (i <= mid) {
        arr[k++] = buffer[i++]; // Copy leftover left element and advance pointers
    }

    // WHY: If right half still has remaining elements, copy them all into destination.
    while (j <= right) {
        arr[k++] = buffer[j++]; // Copy leftover right element and advance pointers
    }
}

// WHAT:    Recursive worker for Merge Sort dividing ranges and merging halves.
// IDEA:    Splits range at mid = left + (right - left)/2, recursively sorts both halves,
//          and calls merge() to combine them in sorted order.
// EXAMPLE: mergeSortInternal on [5, 2, 8, 1] -> divides to [5, 2] and [8, 1], merges to [1, 2, 5, 8]
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(log n) call stack frames
// VIVA:    Base condition 'left >= right' stops recursion when range has 0 or 1 element.
template <typename T, typename Comp>
void mergeSortInternal(std::vector<T>& arr, std::vector<T>& buffer, int left, int right, Metrics& metrics, Comp comp) {
    // WHY: A range of length 0 or 1 is already sorted by definition; stop recursion.
    if (left >= right) {
        return;
    }

    // WHY: Compute midpoint without integer overflow (avoids (left + right) / 2 overflow bug).
    int mid = left + (right - left) / 2;

    // WHY: Recursively sort the left half [left..mid].
    mergeSortInternal(arr, buffer, left, mid, metrics, comp);

    // WHY: Recursively sort the right half [mid+1..right].
    mergeSortInternal(arr, buffer, mid + 1, right, metrics, comp);

    // WHY: Combine both sorted halves into a single contiguous sorted range.
    merge(arr, buffer, left, mid, right, metrics, comp);
}

// WHAT:    Selects pivot index based on strategy (LastElement or MedianOfThree).
// IDEA:    For MedianOfThree, sorts low, mid, and high elements, placing the median at high
//          so standard Lomuto partitioning can run without changes.
// EXAMPLE: arr=[9, 2, 5] at low=0, mid=1, high=2 -> median 5 moved to high position 2
// TIME:    best O(1) / avg O(1) / worst O(1) (at most 3 comparisons)      SPACE: O(1)
// VIVA:    Placing median at high allows partition() to remain a standard last-element Lomuto partition.
template <typename T, typename Comp>
int choosePivot(std::vector<T>& arr, int low, int high, PivotStrategy strategy, Metrics& metrics, Comp comp) {
    // WHY: If naive strategy chosen, return last element index high immediately.
    if (strategy == PivotStrategy::LastElement) {
        return high; // Naive pivot: causes worst-case O(n^2) on pre-sorted data
    }

    // WHY: Compute middle index to sample first, middle, and last elements.
    int mid = low + (high - low) / 2;

    // WHY: Step 1 of 3-element sorting network: ensure arr[low] <= arr[mid].
    metrics.comparisons++;
    if (comp(arr[mid], arr[low])) {
        std::swap(arr[low], arr[mid]); // Swap so smaller of low and mid is at low
    }

    // WHY: Step 2 of 3-element sorting network: ensure arr[low] <= arr[high].
    metrics.comparisons++;
    if (comp(arr[high], arr[low])) {
        std::swap(arr[low], arr[high]); // Swap so smallest of all three is at low
    }

    // WHY: Step 3 of 3-element sorting network: ensure arr[mid] <= arr[high].
    metrics.comparisons++;
    if (comp(arr[high], arr[mid])) {
        std::swap(arr[mid], arr[high]); // Swap so largest is at high, median is at mid
    }

    // WHY: Move median value from mid to high position so Lomuto partition can treat high as pivot.
    std::swap(arr[mid], arr[high]);
    return high;
}

// WHAT:    Lomuto partition scheme: partitions arr[low..high] around pivot.
// IDEA:    Places pivot at high, iterates j from low to high-1, and swaps elements smaller
//          than pivot to the left side (index i). Finally swaps pivot to position i+1.
// EXAMPLE: [5, 2, 9, 1, 7] with pivot 7 -> partitions to [5, 2, 1, 7, 9], returns index 3
// TIME:    best O(n) / avg O(n) / worst O(n) where n = high - low + 1      SPACE: O(1)
// VIVA:    Returning i + 1 gives the final permanent index of the pivot element.
template <typename T, typename Comp>
int partition(std::vector<T>& arr, int low, int high, PivotStrategy strategy, Metrics& metrics, Comp comp) {
    // WHY: Select pivot using chosen strategy and place it at index 'high'.
    choosePivot(arr, low, high, strategy, metrics, comp);

    // WHY: Cache pivot value for comparison against other elements.
    const T pivot = arr[high];

    // WHY: i marks the boundary of elements strictly smaller than pivot (starts before low).
    int i = low - 1;

    // WHY: Scan through elements from low up to high - 1 (excluding pivot at high).
    for (int j = low; j < high; ++j) {
        metrics.comparisons++; // WHY: Count every key comparison against pivot

        // WHY: If current element arr[j] is smaller than pivot, it belongs in left partition.
        if (comp(arr[j], pivot)) {
            i++;                        // Advance smaller-element boundary pointer
            std::swap(arr[i], arr[j]);  // Swap smaller element into left boundary
        }
        // WHY: If arr[j] >= pivot, do nothing; it naturally remains in the right partition.
    }

    // WHY: Place pivot at its final sorted position immediately following the left partition (i + 1).
    std::swap(arr[i + 1], arr[high]);

    // WHY: Return pivot index so caller knows where to split left and right recursive calls.
    return i + 1;
}

// WHAT:    Recursive worker for Quick Sort.
// IDEA:    Partitions range [low..high] around pivot p, then recursively sorts
//          subarrays [low..p-1] and [p+1..high] in-place.
// EXAMPLE: quickSortInternal on [5, 2, 9, 1, 7] -> partitions at 7, sorts left [5, 2, 1] and right [9]
// TIME:    best O(n log n) / avg O(n log n) / worst O(n^2)      SPACE: O(log n) stack frames
// VIVA:    Skipping pivot position 'p' (using p-1 and p+1) guarantees progress toward termination.
template <typename T, typename Comp>
void quickSortInternal(std::vector<T>& arr, int low, int high, PivotStrategy strategy, Metrics& metrics, Comp comp) {
    // WHY: If subarray has 0 or 1 element, it is already sorted; stop recursion.
    if (low >= high) {
        return;
    }

    // WHY: Partition array around pivot; p is the final sorted index of the pivot.
    int p = partition(arr, low, high, strategy, metrics, comp);

    // WHY: Recursively sort elements strictly to the left of the pivot (elements < pivot).
    quickSortInternal(arr, low, p - 1, strategy, metrics, comp);

    // WHY: Recursively sort elements strictly to the right of the pivot (elements > pivot).
    quickSortInternal(arr, p + 1, high, strategy, metrics, comp);
}

} // namespace detail

// WHAT:    Entry point for Merge Sort with timer and metrics.
// IDEA:    Allocates an auxiliary vector of size n and launches recursive mergeSortInternal.
// EXAMPLE: mergeSort(docs, metrics) -> sorts documents in-place
// TIME:    best O(n log n) / avg O(n log n) / worst O(n log n)      SPACE: O(n) auxiliary buffer
// VIVA:    Records peakBytes = sizeof(T) * n to measure auxiliary memory allocated for merging.
template <typename T, typename Comp>
void mergeSort(std::vector<T>& arr, Metrics& metrics, Comp comp) {
    // WHY: Vectors of size 0 or 1 are already sorted.
    if (arr.size() <= 1) return;

    metrics.startTimer();
    // WHY: Record auxiliary buffer memory (sizeof(T) * n bytes) for theoretical space reporting.
    metrics.peakBytes = sizeof(T) * arr.size();
    std::vector<T> buffer(arr.size());

    // WHY: Launch recursive divide-and-conquer sorting.
    detail::mergeSortInternal(arr, buffer, 0, static_cast<int>(arr.size()) - 1, metrics, comp);
    metrics.stopTimer();
}

// WHAT:    Entry point for Quick Sort defaulting to Median-of-Three pivot strategy.
// IDEA:    Delegates to quickSortEx using PivotStrategy::MedianOfThree for guaranteed balance.
// EXAMPLE: quickSort(docs, metrics) -> sorts documents in-place
// TIME:    best O(n log n) / avg O(n log n) / worst O(n^2)      SPACE: O(log n) stack frames
// VIVA:    Uses MedianOfThree to prevent O(n^2) degradation on pre-sorted catalogs.
template <typename T, typename Comp>
void quickSort(std::vector<T>& arr, Metrics& metrics, Comp comp) {
    quickSortEx(arr, metrics, PivotStrategy::MedianOfThree, comp);
}

// WHAT:    Entry point for Quick Sort with customizable pivot strategy.
// IDEA:    Starts timer, invokes recursive quickSortInternal across full array index range [0..size-1].
// EXAMPLE: quickSortEx(docs, metrics, PivotStrategy::LastElement) -> demonstrates naive pivot
// TIME:    best O(n log n) / avg O(n log n) / worst O(n^2)      SPACE: O(log n) stack frames
// VIVA:    Operates completely in-place with zero auxiliary heap allocation.
template <typename T, typename Comp>
void quickSortEx(std::vector<T>& arr, Metrics& metrics, PivotStrategy strategy, Comp comp) {
    // WHY: Arrays of size 0 or 1 are already sorted.
    if (arr.size() <= 1) return;

    metrics.startTimer();
    // WHY: Launch recursive quicksort on the full range [0..size-1].
    detail::quickSortInternal(arr, 0, static_cast<int>(arr.size()) - 1, strategy, metrics, comp);
    metrics.stopTimer();
}

} // namespace dls
