#pragma once
#ifndef VAS_INSERTION_SORT_H
#define VAS_INSERTION_SORT_H

/**
 * @file insertion_sort.h
 * @brief Insertion Sort — O(n²) worst, O(n) best (nearly sorted input).
 *
 * Visual highlight:
 *  - Key element (being inserted) shown in magenta.
 *  - Elements being shifted right shown in yellow.
 *  - Sorted prefix (left side) drawn in green.
 */

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class InsertionSort : public AlgorithmBase {
public:
    InsertionSort() : AlgorithmBase("Insertion Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneStep();

    std::vector<int> m_data;
    std::vector<int> m_original;

    size_t m_i   { 1 };  ///< Outer index — next element to insert
    int    m_j   { 0 };  ///< Inner index — shifting position
    int    m_key { 0 };  ///< Value being inserted
    bool   m_inserting { false }; ///< True while shifting inner loop
};

} // namespace vas

#endif // VAS_INSERTION_SORT_H
