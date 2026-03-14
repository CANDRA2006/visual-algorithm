#pragma once
#ifndef VAS_COUNTING_SORT_H
#define VAS_COUNTING_SORT_H

/**
 * @file counting_sort.h
 * @brief Counting Sort — O(n + k), non-comparison-based.
 *
 * Three-phase visualization:
 *  Phase 1 — COUNT:    fills frequency array (highlighted bar = counter cell)
 *  Phase 2 — PREFIX:   computes prefix sums over count array
 *  Phase 3 — OUTPUT:   places elements into sorted output, then copies back
 *
 * Visual highlight:
 *  - Counting phase: scanned element in yellow.
 *  - Prefix phase:   current cell in cyan.
 *  - Output phase:   placed element in green.
 */

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class CountingSort : public AlgorithmBase {
public:
    CountingSort() : AlgorithmBase("Counting Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneStep();

    enum class Phase { COUNT, PREFIX, OUTPUT, COPY_BACK };

    std::vector<int> m_data;
    std::vector<int> m_original;
    std::vector<int> m_count;
    std::vector<int> m_output;

    Phase  m_phase   { Phase::COUNT };
    int    m_idx     { 0 };
    int    m_minVal  { 0 };
    int    m_maxVal  { 0 };
};

} // namespace vas
#endif // VAS_COUNTING_SORT_H
