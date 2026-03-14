#pragma once
#ifndef VAS_HEAP_SORT_H
#define VAS_HEAP_SORT_H

/**
 * @file heap_sort.h
 * @brief Heap Sort — O(n log n) guaranteed, in-place.
 *
 * Visual highlight:
 *  - Heapify comparisons shown in orange.
 *  - Root extraction shown in red.
 *  - Sorted suffix grows green from right.
 */

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class HeapSort : public AlgorithmBase {
public:
    HeapSort() : AlgorithmBase("Heap Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneStep();

    enum class Phase { BUILD_HEAP, EXTRACT };

    std::vector<int> m_data;
    std::vector<int> m_original;

    Phase  m_phase     { Phase::BUILD_HEAP };
    int    m_heapSize  { 0 };
    int    m_buildIdx  { 0 };   ///< current root during build phase (counts down)
    // Heapify state
    int    m_hRoot     { 0 };
    int    m_hLargest  { 0 };
    int    m_hLeft     { 0 };
    int    m_hRight    { 0 };
    bool   m_heapifying{ false };
};

} // namespace vas
#endif // VAS_HEAP_SORT_H
