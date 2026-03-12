#pragma once
#ifndef VAS_MERGE_SORT_H
#define VAS_MERGE_SORT_H

/**
 * @file merge_sort.h
 * @brief Iterative (bottom-up) Merge Sort — O(n log n) guaranteed.
 *
 * Uses iterative bottom-up approach so we can pause/resume at any merge step
 * without recursive stack management.
 *
 * Visual highlight:
 *  - Left half of current merge shown in yellow.
 *  - Right half shown in cyan.
 *  - Active write position shown in red.
 */

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class MergeSort : public AlgorithmBase {
public:
    MergeSort() : AlgorithmBase("Merge Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    // Iterative merge-sort state
    struct MergeState {
        size_t width  { 1 };   ///< Current merge width
        size_t left   { 0 };   ///< Start of left sub-array
        // Within a single merge(left, mid, right)
        size_t i      { 0 };   ///< Left runner
        size_t j      { 0 };   ///< Right runner
        size_t k      { 0 };   ///< Write position in aux
        size_t mid    { 0 };
        size_t right  { 0 };
        bool   merging { false }; ///< In the middle of a merge
    };

    void advanceMerge();
    void startNextSegment();

    std::vector<int> m_data;
    std::vector<int> m_original;
    std::vector<int> m_aux;    ///< Auxiliary buffer

    MergeState m_ms;
};

} // namespace vas

#endif // VAS_MERGE_SORT_H
