#pragma once
#ifndef VAS_RADIX_SORT_H
#define VAS_RADIX_SORT_H

/**
 * @file radix_sort.h
 * @brief LSD Radix Sort — O(d·(n + k)), non-comparison-based.
 *
 * Processes digits from least significant to most significant.
 * Each digit pass uses counting sort as a stable subroutine.
 *
 * Visual highlight:
 *  - Current digit position shown in step description.
 *  - Elements being bucketed shown in orange.
 *  - Elements being written back shown in green.
 */

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class RadixSort : public AlgorithmBase {
public:
    RadixSort() : AlgorithmBase("Radix Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 999);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneStep();

    enum class Phase { COUNTING, PREFIX, OUTPUT, COPY_BACK };

    std::vector<int> m_data;
    std::vector<int> m_original;
    std::vector<int> m_output;
    std::vector<int> m_count;

    Phase  m_phase  { Phase::COUNTING };
    int    m_exp    { 1 };      ///< Current digit exponent (1, 10, 100, …)
    int    m_maxVal { 0 };
    int    m_idx    { 0 };
};

} // namespace vas
#endif // VAS_RADIX_SORT_H
