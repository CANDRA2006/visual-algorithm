#include "algorithms/sorting/counting_sort.h"
#include "core/audio_manager.h"
#include <random>
#include <chrono>
#include <algorithm>

namespace vas {

void CountingSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void CountingSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    m_minVal = *std::min_element(m_data.begin(), m_data.end());
    m_maxVal = *std::max_element(m_data.begin(), m_data.end());
    const int range = m_maxVal - m_minVal + 1;
    m_count.assign(static_cast<size_t>(range), 0);
    m_output.resize(m_data.size(), 0);
    m_phase = Phase::COUNT;
    m_idx   = 0;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
}

void CountingSort::reset()
{
    m_data = m_original;
    m_minVal = m_data.empty() ? 0 : *std::min_element(m_data.begin(), m_data.end());
    m_maxVal = m_data.empty() ? 0 : *std::max_element(m_data.begin(), m_data.end());
    const int range = m_maxVal - m_minVal + 1;
    m_count.assign(static_cast<size_t>(range), 0);
    m_output.resize(m_data.size(), 0);
    m_phase = Phase::COUNT;
    m_idx   = 0;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void CountingSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOneStep();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void CountingSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        doOneStep();
}

void CountingSort::doOneStep()
{
    const int n       = static_cast<int>(m_data.size());
    const int range   = static_cast<int>(m_count.size());
    const float maxF  = static_cast<float>(m_maxVal);

    switch (m_phase) {

    case Phase::COUNT: {
        if (m_idx >= n) {
            m_phase = Phase::PREFIX;
            m_idx   = 1;
            AlgorithmStep s("Counting done — computing prefix sums", sf::Color(0, 200, 220));
            emit(s);
            return;
        }
        int val = m_data[m_idx];
        m_count[static_cast<size_t>(val - m_minVal)]++;
        m_stats.comparisons++;
        float norm = maxF > 0 ? static_cast<float>(val) / maxF : 0.5f;
        VAS_AUDIO.playCompare(norm);

        AlgorithmStep step("Counting: value " + std::to_string(val) +
                           "  count=" + std::to_string(m_count[val - m_minVal]),
                           sf::Color(240, 200, 40));
        step.highlightedIndices = { m_idx };
        emit(step);
        m_idx++;
        break;
    }

    case Phase::PREFIX: {
        if (m_idx >= range) {
            m_phase = Phase::OUTPUT;
            m_idx   = n - 1;
            AlgorithmStep s("Prefix sums done — writing output", sf::Color(100, 220, 100));
            emit(s);
            return;
        }
        m_count[static_cast<size_t>(m_idx)] += m_count[static_cast<size_t>(m_idx - 1)];
        float norm = static_cast<float>(m_idx) / static_cast<float>(range);
        VAS_AUDIO.playCompare(norm);

        AlgorithmStep step("Prefix sum at bucket " + std::to_string(m_idx) +
                           " = " + std::to_string(m_count[m_idx]),
                           sf::Color(0, 200, 220));
        emit(step);
        m_idx++;
        break;
    }

    case Phase::OUTPUT: {
        if (m_idx < 0) {
            m_phase = Phase::COPY_BACK;
            m_idx   = 0;
            AlgorithmStep s("Output ready — copying back", sf::Color(100, 220, 100));
            emit(s);
            return;
        }
        int val = m_data[static_cast<size_t>(m_idx)];
        int bucket = val - m_minVal;
        m_count[static_cast<size_t>(bucket)]--;
        int outIdx = m_count[static_cast<size_t>(bucket)];
        m_output[static_cast<size_t>(outIdx)] = val;
        m_stats.swaps++;

        float norm = maxF > 0 ? static_cast<float>(val) / maxF : 0.5f;
        VAS_AUDIO.playSwap(norm, static_cast<float>(outIdx) / n);

        AlgorithmStep step("Placing " + std::to_string(val) +
                           " at output[" + std::to_string(outIdx) + "]",
                           sf::Color(100, 220, 100));
        step.highlightedIndices = { m_idx };
        emit(step);
        m_idx--;
        break;
    }

    case Phase::COPY_BACK: {
        if (m_idx >= n) {
            VAS_AUDIO.playSorted();
            AlgorithmStep done("Sorted!", sf::Color::Green, true);
            for (int k = 0; k < n; ++k) done.sortedIndices.push_back(k);
            emit(done);
            m_state = AlgorithmState::COMPLETED;
            return;
        }
        m_data[static_cast<size_t>(m_idx)] = m_output[static_cast<size_t>(m_idx)];
        float norm = maxF > 0 ? static_cast<float>(m_data[m_idx]) / maxF : 0.5f;
        VAS_AUDIO.playCompare(norm);

        AlgorithmStep step("Copy back [" + std::to_string(m_idx) + "] = " +
                           std::to_string(m_data[m_idx]),
                           sf::Color::Green);
        step.highlightedIndices = { m_idx };
        for (int k = 0; k < m_idx; ++k) step.sortedIndices.push_back(k);
        emit(step);
        m_idx++;
        break;
    }
    }
}

} // namespace vas
