#pragma once
#ifndef VAS_SHELL_SORT_H
#define VAS_SHELL_SORT_H

/**
 * @file shell_sort.h
 * @brief Shell Sort using Ciura gap sequence — O(n log² n) typical.
 *
 * Visual highlight:
 *  - Elements being compared across gap shown in cyan.
 *  - Shift operations shown in yellow.
 *  - Current gap displayed in step description.
 */

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class ShellSort : public AlgorithmBase {
public:
    ShellSort() : AlgorithmBase("Shell Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneStep();
    void computeGaps();

    std::vector<int> m_data;
    std::vector<int> m_original;

    // Ciura gap sequence (reversed so largest gap first)
    std::vector<int> m_gaps;
    size_t m_gapIdx  { 0 };   ///< index into m_gaps
    int    m_i       { 0 };   ///< outer loop
    int    m_j       { 0 };   ///< inner loop
    int    m_temp    { 0 };   ///< value being inserted
    bool   m_shifting{ false };
};

} // namespace vas
#endif // VAS_SHELL_SORT_H
