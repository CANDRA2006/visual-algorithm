#include "algorithms/sorting/shell_sort.h"
#include "core/audio_manager.h"
#include <random>
#include <chrono>
#include <algorithm>

namespace vas {

void ShellSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void ShellSort::computeGaps()
{
    static const int ciura[] = { 1, 4, 10, 23, 57, 132, 301, 701 };
    m_gaps.clear();
    const int n = static_cast<int>(m_data.size());
    for (int g : ciura) {
        if (g < n) m_gaps.push_back(g);
    }
    std::reverse(m_gaps.begin(), m_gaps.end());
    if (m_gaps.empty()) m_gaps.push_back(1);
}

void ShellSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    computeGaps();
    m_gapIdx   = 0;
    m_i        = static_cast<int>(m_gaps[0]);
    m_j        = m_i;
    m_temp     = m_data[m_i];
    m_shifting = false;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
}

void ShellSort::reset()
{
    m_data = m_original;
    computeGaps();
    m_gapIdx   = 0;
    m_i        = static_cast<int>(m_gaps[0]);
    m_j        = m_i;
    m_temp     = m_data.empty() ? 0 : m_data[m_i];
    m_shifting = false;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void ShellSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOneStep();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void ShellSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        doOneStep();
}

void ShellSort::doOneStep()
{
    const int n = static_cast<int>(m_data.size());

    // Advance gap if current gap exhausted
    while (m_gapIdx < m_gaps.size()) {
        int gap = m_gaps[m_gapIdx];
        if (m_i < n) break;
        // Move to next gap
        m_gapIdx++;
        if (m_gapIdx >= m_gaps.size()) {
            // Done
            VAS_AUDIO.playSorted();
            AlgorithmStep done("Sorted!", sf::Color::Green, true);
            for (int k = 0; k < n; ++k) done.sortedIndices.push_back(k);
            emit(done);
            m_state = AlgorithmState::COMPLETED;
            return;
        }
        gap  = m_gaps[m_gapIdx];
        m_i  = gap;
        m_j  = m_i;
        if (m_i < n) m_temp = m_data[m_i];
        m_shifting = false;
    }

    if (m_gapIdx >= m_gaps.size()) {
        VAS_AUDIO.playSorted();
        AlgorithmStep done("Sorted!", sf::Color::Green, true);
        for (int k = 0; k < n; ++k) done.sortedIndices.push_back(k);
        emit(done);
        m_state = AlgorithmState::COMPLETED;
        return;
    }

    int gap = m_gaps[m_gapIdx];

    if (!m_shifting) {
        m_temp = m_data[m_i];
        m_j    = m_i;
        m_shifting = true;
    }

    int maxVal = *std::max_element(m_data.begin(), m_data.end());
    float norm = maxVal > 0 ? static_cast<float>(m_temp) / maxVal : 0.5f;

    if (m_j >= gap && m_data[m_j - gap] > m_temp) {
        m_stats.comparisons++;
        m_data[m_j] = m_data[m_j - gap];
        m_stats.swaps++;
        VAS_AUDIO.playSwap(norm, maxVal > 0 ? static_cast<float>(m_data[m_j]) / maxVal : 0.5f);

        AlgorithmStep step("Gap=" + std::to_string(gap) +
                           "  shifting " + std::to_string(m_data[m_j]),
                           sf::Color(0, 200, 220));
        step.highlightedIndices = { m_j, m_j - gap };
        emit(step);

        m_j -= gap;
    } else {
        m_stats.comparisons++;
        m_data[m_j] = m_temp;
        VAS_AUDIO.playCompare(norm);

        AlgorithmStep step("Gap=" + std::to_string(gap) +
                           "  inserted " + std::to_string(m_temp),
                           sf::Color(100, 220, 100));
        step.highlightedIndices = { m_j, m_i };
        emit(step);

        m_shifting = false;
        m_i++;
        if (m_i < n) m_temp = m_data[m_i];
    }
}

} // namespace vas
