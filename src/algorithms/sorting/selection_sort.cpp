#include "algorithms/sorting/selection_sort.h"
#include <random>
#include <chrono>
#include <algorithm>

namespace vas {

void SelectionSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void SelectionSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    m_i = 0; m_j = 1; m_minIdx = 0;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
}

void SelectionSort::reset()
{
    m_data = m_original;
    m_i = 0; m_j = 1; m_minIdx = 0;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void SelectionSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOneComparison();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void SelectionSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        doOneComparison();
}

void SelectionSort::doOneComparison()
{
    const size_t n = m_data.size();

    if (m_i >= n - 1) {
        AlgorithmStep done("Sorted!", sf::Color::Green, true);
        for (size_t k = 0; k < n; ++k) done.sortedIndices.push_back(static_cast<int>(k));
        emit(done);
        m_state = AlgorithmState::COMPLETED;
        return;
    }

    if (m_j >= n) {
        // End of scan — swap minimum into position m_i
        if (m_minIdx != m_i) {
            std::swap(m_data[m_i], m_data[m_minIdx]);
            m_stats.swaps++;
        }

        AlgorithmStep swapStep("Placed min at index " + std::to_string(m_i),
                               sf::Color(220, 60, 60));
        for (size_t k = 0; k <= m_i; ++k)
            swapStep.sortedIndices.push_back(static_cast<int>(k));
        emit(swapStep);

        m_i++;
        m_j = m_i + 1;
        m_minIdx = m_i;
        return;
    }

    // Compare m_j with current minimum
    m_stats.comparisons++;
    if (m_data[m_j] < m_data[m_minIdx]) m_minIdx = m_j;

    AlgorithmStep step;
    step.highlightedIndices = { static_cast<int>(m_j), static_cast<int>(m_minIdx) };
    step.highlightColor     = sf::Color(240, 200, 40);
    step.description        = "Scanning... current min=" + std::to_string(m_data[m_minIdx]);
    for (size_t k = 0; k < m_i; ++k)
        step.sortedIndices.push_back(static_cast<int>(k));
    emit(step);

    m_j++;
}

} // namespace vas
