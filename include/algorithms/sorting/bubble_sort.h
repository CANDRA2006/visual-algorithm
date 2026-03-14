#pragma once
#ifndef VAS_BUBBLE_SORT_H
#define VAS_BUBBLE_SORT_H

/**
 * @file bubble_sort.h
 * @brief Optimized Bubble Sort with early-exit and per-step callback.
 */

#include "core/algorithm_base.h"
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace vas {

class BubbleSort : public AlgorithmBase {
public:
    BubbleSort() : AlgorithmBase("Bubble Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    // - AlgorithmBase interface  
    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneComparison();

    std::vector<int> m_data;
    std::vector<int> m_original;

    size_t m_i       { 0 };
    size_t m_j       { 0 };
    bool   m_swapped { false };

    std::vector<int> m_sortedTail; ///< Indices known to be in final position
};

} // namespace vas

#endif // VAS_BUBBLE_SORT_H
