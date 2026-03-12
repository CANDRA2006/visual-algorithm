#include "algorithms/sorting/merge_sort.h"
#include <random>
#include <chrono>
#include <algorithm>

namespace vas {

void MergeSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void MergeSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    m_aux = m_data;
    m_ms = MergeState{};
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    startNextSegment();
}

void MergeSort::reset()
{
    m_data = m_original;
    m_aux  = m_data;
    m_ms   = MergeState{};
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void MergeSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        advanceMerge();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void MergeSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        advanceMerge();
}

//  startNextSegment  
void MergeSort::startNextSegment()
{
    const size_t n = m_data.size();

    while (true) {
        if (m_ms.width >= n) {
            // Sort complete
            AlgorithmStep done("Sorted!", sf::Color::Green, true);
            for (size_t k = 0; k < n; ++k) done.sortedIndices.push_back(static_cast<int>(k));
            emit(done);
            m_state = AlgorithmState::COMPLETED;
            return;
        }

        if (m_ms.left >= n) {
            // Advance to next width
            m_ms.width *= 2;
            m_ms.left   = 0;
        } else {
            // Set up a merge of [left, mid) and [mid, right)
            m_ms.mid   = std::min(m_ms.left + m_ms.width, n);
            m_ms.right = std::min(m_ms.left + 2 * m_ms.width, n);

            if (m_ms.mid >= m_ms.right) {
                // No right half — skip
                m_ms.left = m_ms.right;
                continue;
            }

            // Copy segment to aux
            std::copy(m_data.begin() + static_cast<std::ptrdiff_t>(m_ms.left),
                      m_data.begin() + static_cast<std::ptrdiff_t>(m_ms.right),
                      m_aux.begin()  + static_cast<std::ptrdiff_t>(m_ms.left));

            m_ms.i        = m_ms.left;
            m_ms.j        = m_ms.mid;
            m_ms.k        = m_ms.left;
            m_ms.merging  = true;
            return;
        }
    }
}

//  advanceMerge  ----
void MergeSort::advanceMerge()
{
    if (!m_ms.merging) {
        startNextSegment();
        return;
    }

    if (m_state == AlgorithmState::COMPLETED) return;

    // One write step of the merge
    if (m_ms.i < m_ms.mid && m_ms.j < m_ms.right) {
        m_stats.comparisons++;
        if (m_aux[m_ms.i] <= m_aux[m_ms.j]) {
            m_data[m_ms.k] = m_aux[m_ms.i++];
        } else {
            m_data[m_ms.k] = m_aux[m_ms.j++];
            m_stats.swaps++;
        }
    } else if (m_ms.i < m_ms.mid) {
        m_data[m_ms.k] = m_aux[m_ms.i++];
    } else if (m_ms.j < m_ms.right) {
        m_data[m_ms.k] = m_aux[m_ms.j++];
    } else {
        // Merge complete for this segment
        m_ms.merging = false;
        m_ms.left    = m_ms.right;
        startNextSegment();
        return;
    }

    AlgorithmStep step("Merging [" + std::to_string(m_ms.left) +
                       "–" + std::to_string(m_ms.mid - 1) + "] and [" +
                       std::to_string(m_ms.mid) + "–" +
                       std::to_string(m_ms.right - 1) + "]",
                       sf::Color(240, 200, 40));

    // Left half = yellow, right half = cyan, write pos = red
    for (size_t x = m_ms.left; x < m_ms.mid; ++x)
        step.highlightedIndices.push_back(static_cast<int>(x));
    step.highlightedIndices.push_back(static_cast<int>(m_ms.k));

    emit(step);
    m_ms.k++;
}

} // namespace vas
