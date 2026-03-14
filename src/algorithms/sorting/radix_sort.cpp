#include "algorithms/sorting/radix_sort.h"
#include "core/audio_manager.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace vas {

void RadixSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void RadixSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    m_maxVal = *std::max_element(m_data.begin(), m_data.end());
    m_exp    = 1;
    m_output.resize(m_data.size(), 0);
    m_count.assign(10, 0);
    m_phase = Phase::COUNTING;
    m_idx   = 0;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
}

void RadixSort::reset()
{
    m_data   = m_original;
    m_maxVal = m_data.empty() ? 0 : *std::max_element(m_data.begin(), m_data.end());
    m_exp    = 1;
    m_output.resize(m_data.size(), 0);
    m_count.assign(10, 0);
    m_phase  = Phase::COUNTING;
    m_idx    = 0;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void RadixSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOneStep();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void RadixSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        doOneStep();
}

void RadixSort::doOneStep()
{
    const int n     = static_cast<int>(m_data.size());
    const float maxF = static_cast<float>(m_maxVal);

    // All digit passes complete?
    if (m_maxVal / m_exp == 0) {
        VAS_AUDIO.playSorted();
        AlgorithmStep done("Sorted!", sf::Color::Green, true);
        for (int k = 0; k < n; ++k) done.sortedIndices.push_back(k);
        emit(done);
        m_state = AlgorithmState::COMPLETED;
        return;
    }

    int digitPos = static_cast<int>(std::log10(static_cast<float>(m_exp))) + 1;

    switch (m_phase) {

    case Phase::COUNTING: {
        if (m_idx >= n) {
            m_phase = Phase::PREFIX;
            m_idx   = 1;
            AlgorithmStep s("Digit " + std::to_string(digitPos) +
                            " — counting done, building prefix sums",
                            sf::Color(0, 200, 220));
            emit(s);
            return;
        }
        int digit = (m_data[m_idx] / m_exp) % 10;
        m_count[static_cast<size_t>(digit)]++;
        m_stats.comparisons++;
        float norm = maxF > 0 ? static_cast<float>(m_data[m_idx]) / maxF : 0.5f;
        VAS_AUDIO.playCompare(norm);

        AlgorithmStep step("Digit " + std::to_string(digitPos) +
                           ": value " + std::to_string(m_data[m_idx]) +
                           "  digit=" + std::to_string(digit),
                           sf::Color(255, 140, 0));
        step.highlightedIndices = { m_idx };
        emit(step);
        m_idx++;
        break;
    }

    case Phase::PREFIX: {
        if (m_idx >= 10) {
            m_phase = Phase::OUTPUT;
            m_idx   = n - 1;
            AlgorithmStep s("Prefix sums ready — placing elements",
                            sf::Color(100, 220, 100));
            emit(s);
            return;
        }
        m_count[static_cast<size_t>(m_idx)] += m_count[static_cast<size_t>(m_idx - 1)];
        VAS_AUDIO.playCompare(static_cast<float>(m_idx) / 10.0f);

        AlgorithmStep step("Prefix[" + std::to_string(m_idx) + "]=" +
                           std::to_string(m_count[m_idx]),
                           sf::Color(0, 200, 220));
        emit(step);
        m_idx++;
        break;
    }

    case Phase::OUTPUT: {
        if (m_idx < 0) {
            m_phase = Phase::COPY_BACK;
            m_idx   = 0;
            return;
        }
        int digit  = (m_data[static_cast<size_t>(m_idx)] / m_exp) % 10;
        m_count[static_cast<size_t>(digit)]--;
        int outIdx = m_count[static_cast<size_t>(digit)];
        m_output[static_cast<size_t>(outIdx)] = m_data[static_cast<size_t>(m_idx)];
        m_stats.swaps++;

        float norm = maxF > 0 ? static_cast<float>(m_data[m_idx]) / maxF : 0.5f;
        VAS_AUDIO.playSwap(norm, static_cast<float>(outIdx) / n);

        AlgorithmStep step("Placing " + std::to_string(m_data[m_idx]) +
                           " → output[" + std::to_string(outIdx) + "]",
                           sf::Color(255, 140, 0));
        step.highlightedIndices = { m_idx };
        emit(step);
        m_idx--;
        break;
    }

    case Phase::COPY_BACK: {
        if (m_idx >= n) {
            // Advance to next digit
            m_exp    *= 10;
            m_count.assign(10, 0);
            m_phase   = Phase::COUNTING;
            m_idx     = 0;

            if (m_maxVal / m_exp == 0) {
                VAS_AUDIO.playSorted();
                AlgorithmStep done("Sorted!", sf::Color::Green, true);
                for (int k = 0; k < n; ++k) done.sortedIndices.push_back(k);
                emit(done);
                m_state = AlgorithmState::COMPLETED;
            } else {
                AlgorithmStep s("Pass " + std::to_string(digitPos) +
                                " complete — moving to digit " + std::to_string(digitPos + 1),
                                sf::Color(100, 100, 255));
                emit(s);
            }
            return;
        }
        m_data[static_cast<size_t>(m_idx)] = m_output[static_cast<size_t>(m_idx)];
        float norm = maxF > 0 ? static_cast<float>(m_data[m_idx]) / maxF : 0.5f;
        VAS_AUDIO.playCompare(norm);

        AlgorithmStep step("Copy back [" + std::to_string(m_idx) + "]=" +
                           std::to_string(m_data[m_idx]),
                           sf::Color::Green);
        step.highlightedIndices = { m_idx };
        emit(step);
        m_idx++;
        break;
    }
    }
}

} // namespace vas
