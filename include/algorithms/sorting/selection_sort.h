#pragma once
#ifndef VAS_SELECTION_SORT_H
#define VAS_SELECTION_SORT_H

/**
 * @file selection_sort.h
 * @brief Selection Sort — O(n²) comparisons, O(n) swaps.
 *
 * Visual highlight:
 *  - Current minimum shown in cyan.
 *  - Element being scanned shown in yellow.
 *  - Swapped element shown in red.
 *  - Sorted prefix grows green from left.
 */

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class SelectionSort : public AlgorithmBase {
public:
    SelectionSort() : AlgorithmBase("Selection Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneComparison();

    std::vector<int> m_data;
    std::vector<int> m_original;

    size_t m_i      { 0 }; ///< Start of unsorted region
    size_t m_j      { 0 }; ///< Scanner
    size_t m_minIdx { 0 }; ///< Current minimum index
};

} // namespace vas

#endif // VAS_SELECTION_SORT_H
