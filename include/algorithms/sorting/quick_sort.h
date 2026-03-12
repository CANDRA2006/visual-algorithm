#pragma once
#ifndef VAS_QUICK_SORT_H
#define VAS_QUICK_SORT_H

/**
 * @file quick_sort.h
 * @brief Iterative Quick Sort using an explicit stack — O(n log n) average.
 *
 * Uses median-of-three pivot selection to avoid worst-case on sorted input.
 * Iterative form allows pause/resume without recursion depth issues.
 *
 * Visual highlight:
 *  - Pivot shown in orange.
 *  - Left pointer shown in yellow, right pointer in cyan.
 *  - Swap positions shown in red.
 */

#include "core/algorithm_base.h"
#include <vector>
#include <stack>

namespace vas {

class QuickSort : public AlgorithmBase {
public:
    QuickSort() : AlgorithmBase("Quick Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOnePartitionStep();
    void pushSegment(int lo, int hi);
    int  medianOfThreePivot(int lo, int hi);

    std::vector<int> m_data;
    std::vector<int> m_original;

    // Partition state for current segment
    struct PartState {
        int lo, hi, pivot, pivotIdx, i, j;
    };

    std::stack<std::pair<int,int>> m_segments; ///< Pending (lo,hi) ranges
    PartState m_ps;
    bool      m_partitioning { false };
};

} // namespace vas

#endif // VAS_QUICK_SORT_H
