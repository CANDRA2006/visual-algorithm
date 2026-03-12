#include "algorithms/sorting/insertion_sort.h"
#include <random>
#include <chrono>

namespace vas {

void InsertionSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void InsertionSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original = m_data;
    m_i = 1; m_j = 0; m_key = 0; m_inserting = false;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
}

void InsertionSort::reset()
{
    m_data = m_original;
    m_i = 1; m_j = 0; m_key = 0; m_inserting = false;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void InsertionSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOneStep();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void InsertionSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        doOneStep();
}

void InsertionSort::doOneStep()
{
    const size_t n = m_data.size();

    if (m_i >= n) {
        AlgorithmStep done("Sorted!", sf::Color::Green, true);
        for (size_t k = 0; k < n; ++k) done.sortedIndices.push_back(static_cast<int>(k));
        emit(done);
        m_state = AlgorithmState::COMPLETED;
        return;
    }

    if (!m_inserting) {
        // Start inserting m_data[m_i]
        m_key       = m_data[m_i];
        m_j         = static_cast<int>(m_i) - 1;
        m_inserting = true;
    }

    if (m_j >= 0 && m_data[m_j] > m_key) {
        m_stats.comparisons++;
        m_data[m_j + 1] = m_data[m_j];
        m_stats.swaps++;

        AlgorithmStep step("Shifting " + std::to_string(m_data[m_j]),
                           sf::Color(240, 200, 40));
        step.highlightedIndices = { m_j, m_j + 1 };
        for (int k = 0; k < static_cast<int>(m_i) - 1; ++k)
            step.sortedIndices.push_back(k);
        emit(step);

        m_j--;
    } else {
        m_data[m_j + 1] = m_key;
        m_inserting = false;

        AlgorithmStep place("Inserted " + std::to_string(m_key) +
                            " at index " + std::to_string(m_j + 1),
                            sf::Color(100, 220, 100));
        for (size_t k = 0; k <= m_i; ++k)
            place.sortedIndices.push_back(static_cast<int>(k));
        emit(place);

        m_i++;
    }
}

} // namespace vas
