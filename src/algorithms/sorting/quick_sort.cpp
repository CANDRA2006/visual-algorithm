#include "algorithms/sorting/quick_sort.h"
#include <random>
#include <chrono>
#include <algorithm>

namespace vas {

void QuickSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void QuickSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    while (!m_segments.empty()) m_segments.pop();
    m_partitioning = false;
    m_stats.reset();
    m_stepAccumulator = 0.0f;

    if (m_data.size() > 1)
        m_segments.push({ 0, static_cast<int>(m_data.size()) - 1 });

    // Start first partition
    if (!m_segments.empty()) {
        auto [lo, hi] = m_segments.top();
        m_segments.pop();
        m_ps.lo       = lo;
        m_ps.hi       = hi;
        m_ps.pivotIdx = medianOfThreePivot(lo, hi);
        m_ps.pivot    = m_data[m_ps.pivotIdx];
        // Move pivot to end
        std::swap(m_data[m_ps.pivotIdx], m_data[hi]);
        m_ps.pivotIdx = hi;
        m_ps.i        = lo - 1;
        m_ps.j        = lo;
        m_partitioning = true;
    }
}

void QuickSort::reset()
{
    m_data = m_original;
    while (!m_segments.empty()) m_segments.pop();
    m_partitioning = false;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void QuickSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOnePartitionStep();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void QuickSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        doOnePartitionStep();
}

int QuickSort::medianOfThreePivot(int lo, int hi)
{
    int mid = lo + (hi - lo) / 2;
    if (m_data[lo] > m_data[mid]) std::swap(m_data[lo], m_data[mid]);
    if (m_data[lo] > m_data[hi])  std::swap(m_data[lo], m_data[hi]);
    if (m_data[mid] > m_data[hi]) std::swap(m_data[mid], m_data[hi]);
    return mid; // median is now at mid
}

void QuickSort::doOnePartitionStep()
{
    if (!m_partitioning) {
        if (m_segments.empty()) {
            AlgorithmStep done("Sorted!", sf::Color::Green, true);
            const size_t n = m_data.size();
            for (size_t k = 0; k < n; ++k) done.sortedIndices.push_back(static_cast<int>(k));
            emit(done);
            m_state = AlgorithmState::COMPLETED;
            return;
        }
        auto [lo, hi] = m_segments.top();
        m_segments.pop();
        m_ps.lo       = lo;
        m_ps.hi       = hi;
        m_ps.pivotIdx = medianOfThreePivot(lo, hi);
        m_ps.pivot    = m_data[m_ps.pivotIdx];
        std::swap(m_data[m_ps.pivotIdx], m_data[hi]);
        m_ps.pivotIdx = hi;
        m_ps.i        = lo - 1;
        m_ps.j        = lo;
        m_partitioning = true;
        return;
    }

    if (m_ps.j < m_ps.hi) {
        m_stats.comparisons++;
        if (m_data[m_ps.j] <= m_ps.pivot) {
            m_ps.i++;
            std::swap(m_data[m_ps.i], m_data[m_ps.j]);
            m_stats.swaps++;
        }

        AlgorithmStep step("Pivot=" + std::to_string(m_ps.pivot) +
                           "  j=" + std::to_string(m_ps.j),
                           sf::Color(240, 200, 40));
        step.highlightedIndices = { m_ps.j, m_ps.pivotIdx };
        if (m_ps.i >= m_ps.lo) step.highlightedIndices.push_back(m_ps.i);
        emit(step);

        m_ps.j++;
    } else {
        // Place pivot
        std::swap(m_data[m_ps.i + 1], m_data[m_ps.hi]);
        int p = m_ps.i + 1;

        AlgorithmStep placed("Pivot " + std::to_string(m_ps.pivot) +
                             " placed at index " + std::to_string(p),
                             sf::Color(220, 60, 60));
        placed.highlightedIndices = { p };
        emit(placed);

        // Push sub-arrays
        if (p - 1 > m_ps.lo) m_segments.push({ m_ps.lo, p - 1 });
        if (p + 1 < m_ps.hi) m_segments.push({ p + 1, m_ps.hi });

        m_partitioning = false;
    }
}

} // namespace vas
