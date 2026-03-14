#include "algorithms/sorting/bubble_sort.h"
#include <random>
#include <chrono>
#include <numeric>

namespace vas {

 
// Data generation
 
void BubbleSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

 
// initialize
 
void BubbleSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    m_i = 0; m_j = 0; m_swapped = false;
    m_sortedTail.clear();
    m_stats.reset();
    m_stepAccumulator = 0.0f;
}

 
// reset — restores original data
 
void BubbleSort::reset()
{
    m_data = m_original;
    m_i = 0; m_j = 0; m_swapped = false;
    m_sortedTail.clear();
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;

    // Emit a neutral step so the visualizer clears highlights
    emit(AlgorithmStep("Ready — press Space to start"));
}

 
// update — called every rendered frame
 
void BubbleSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;

    m_stepAccumulator += m_animationSpeed;

    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOneComparison();
        m_stepAccumulator -= 1.0f;
    }

    updateElapsed();
}

 
// stepOnce — advance exactly one comparison (PAUSED mode)
 
void BubbleSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED ||
        m_state == AlgorithmState::IDLE)
    {
        doOneComparison();
    }
}

 
// doOneComparison — the core step unit
 
void BubbleSort::doOneComparison()
{
    const size_t n = m_data.size();

    // All passes done?
    if (m_i >= n - 1) {
        AlgorithmStep done("Sorted!", sf::Color::Green, true);
        for (size_t k = 0; k < n; ++k) done.sortedIndices.push_back(static_cast<int>(k));
        emit(done);
        m_state = AlgorithmState::COMPLETED;
        return;
    }

    // End of current pass?
    if (m_j >= n - m_i - 1) {
        // Mark the last element of this pass as sorted
        const int sortedIdx = static_cast<int>(n - m_i - 1);
        m_sortedTail.push_back(sortedIdx);

        if (!m_swapped) {
            // Early exit — array is already sorted
            AlgorithmStep done("Early exit — array is sorted!", sf::Color::Green, true);
            for (size_t k = 0; k < n; ++k) done.sortedIndices.push_back(static_cast<int>(k));
            emit(done);
            m_state = AlgorithmState::COMPLETED;
            return;
        }

        m_i++;
        m_j = 0;
        m_swapped = false;

        AlgorithmStep passStep("Pass " + std::to_string(m_i) + " complete",
                               sf::Color(70, 130, 180));
        passStep.sortedIndices = m_sortedTail;
        emit(passStep);
        return;
    }

    //  Normal comparison step 
    m_stats.comparisons++;

    AlgorithmStep step;
    step.highlightedIndices = { static_cast<int>(m_j), static_cast<int>(m_j + 1) };
    step.sortedIndices = m_sortedTail;

    if (m_data[m_j] > m_data[m_j + 1]) {
        std::swap(m_data[m_j], m_data[m_j + 1]);
        m_swapped = true;
        m_stats.swaps++;
        step.description  = "Swap " + std::to_string(m_data[m_j + 1]) +
                            " ↔ "  + std::to_string(m_data[m_j]);
        step.highlightColor = sf::Color(220, 60, 60); // Red
    } else {
        step.description  = "Compare [" + std::to_string(m_j) + "] and [" +
                            std::to_string(m_j + 1) + "] — no swap";
        step.highlightColor = sf::Color(240, 200, 40); // Yellow
    }

    emit(step);
    m_j++;
}

} // namespace vas
