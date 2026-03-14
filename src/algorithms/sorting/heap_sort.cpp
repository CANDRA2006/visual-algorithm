#include "algorithms/sorting/heap_sort.h"
#include "core/audio_manager.h"
#include <random>
#include <chrono>
#include <algorithm>

namespace vas {

void HeapSort::generateData(size_t size, int lo, int hi)
{
    m_data.resize(size);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine rng(seed);
    std::uniform_int_distribution<int> dist(lo, hi);
    for (auto& v : m_data) v = dist(rng);
    m_original = m_data;
}

void HeapSort::initialize()
{
    if (m_data.empty()) generateData(30);
    m_original  = m_data;
    m_heapSize  = static_cast<int>(m_data.size());
    m_buildIdx  = m_heapSize / 2 - 1;
    m_phase     = Phase::BUILD_HEAP;
    m_heapifying = false;
    m_hRoot = m_buildIdx;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
}

void HeapSort::reset()
{
    m_data = m_original;
    m_heapSize  = static_cast<int>(m_data.size());
    m_buildIdx  = m_heapSize / 2 - 1;
    m_phase     = Phase::BUILD_HEAP;
    m_heapifying = false;
    m_hRoot = m_buildIdx;
    m_stats.reset();
    m_stepAccumulator = 0.0f;
    m_state = AlgorithmState::IDLE;
    emit(AlgorithmStep("Ready — press Space to start"));
}

void HeapSort::update()
{
    if (m_state != AlgorithmState::RUNNING) return;
    m_stepAccumulator += m_animationSpeed;
    while (m_stepAccumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
        doOneStep();
        m_stepAccumulator -= 1.0f;
    }
    updateElapsed();
}

void HeapSort::stepOnce()
{
    if (m_state == AlgorithmState::PAUSED || m_state == AlgorithmState::IDLE)
        doOneStep();
}

void HeapSort::doOneStep()
{
    const int n = static_cast<int>(m_data.size());

    // Phase 1: Build max-heap (bottom-up heapify) 
    if (m_phase == Phase::BUILD_HEAP) {
        if (!m_heapifying) {
            if (m_buildIdx < 0) {
                // Max-heap built, start extraction
                m_phase    = Phase::EXTRACT;
                m_heapSize = n;
                m_heapifying = false;
                AlgorithmStep s("Max-heap built — starting extraction", sf::Color(100, 220, 100));
                emit(s);
                return;
            }
            m_hRoot    = m_buildIdx;
            m_heapifying = true;
        }

        // One step of sift-down (heapify)
        m_hLeft  = 2 * m_hRoot + 1;
        m_hRight = 2 * m_hRoot + 2;
        m_hLargest = m_hRoot;

        m_stats.comparisons++;
        if (m_hLeft < n && m_data[m_hLeft] > m_data[m_hLargest])
            m_hLargest = m_hLeft;

        m_stats.comparisons++;
        if (m_hRight < n && m_data[m_hRight] > m_data[m_hLargest])
            m_hLargest = m_hRight;

        AlgorithmStep step("Building heap — heapify at " + std::to_string(m_hRoot),
                           sf::Color(255, 140, 0));
        step.highlightedIndices = { m_hRoot, m_hLeft < n ? m_hLeft : m_hRoot,
                                    m_hRight < n ? m_hRight : m_hRoot };

        int maxVal = *std::max_element(m_data.begin(), m_data.end());
        float norm = maxVal > 0 ? static_cast<float>(m_data[m_hRoot]) / maxVal : 0.5f;
        VAS_AUDIO.playCompare(norm);

        if (m_hLargest != m_hRoot) {
            std::swap(m_data[m_hRoot], m_data[m_hLargest]);
            m_stats.swaps++;
            float nA = maxVal > 0 ? static_cast<float>(m_data[m_hRoot]) / maxVal : 0.5f;
            float nB = maxVal > 0 ? static_cast<float>(m_data[m_hLargest]) / maxVal : 0.5f;
            VAS_AUDIO.playSwap(nA, nB);
            m_hRoot = m_hLargest; // continue sifting down
        } else {
            // Sift-down done for this node
            m_buildIdx--;
            m_heapifying = false;
        }

        emit(step);
        return;
    }

    //  Phase 2: Extract elements from heap 
    if (m_phase == Phase::EXTRACT) {
        if (m_heapSize <= 1) {
            // Done
            VAS_AUDIO.playSorted();
            AlgorithmStep done("Sorted!", sf::Color::Green, true);
            for (int k = 0; k < n; ++k) done.sortedIndices.push_back(k);
            emit(done);
            m_state = AlgorithmState::COMPLETED;
            return;
        }

        // Move root (max) to end
        std::swap(m_data[0], m_data[m_heapSize - 1]);
        m_stats.swaps++;
        m_heapSize--;

        int maxVal = *std::max_element(m_data.begin(), m_data.end());
        float nA = maxVal > 0 ? static_cast<float>(m_data[0]) / maxVal : 0.5f;
        VAS_AUDIO.playSwap(nA, 1.0f);

        // Sift-down root of reduced heap
        int root = 0;
        while (true) {
            int left    = 2 * root + 1;
            int right   = 2 * root + 2;
            int largest = root;

            m_stats.comparisons++;
            if (left < m_heapSize && m_data[left] > m_data[largest])   largest = left;
            m_stats.comparisons++;
            if (right < m_heapSize && m_data[right] > m_data[largest]) largest = right;

            if (largest == root) break;
            std::swap(m_data[root], m_data[largest]);
            m_stats.swaps++;
            root = largest;
        }

        AlgorithmStep step("Extracted max, heap size = " + std::to_string(m_heapSize),
                           sf::Color(220, 60, 60));
        step.highlightedIndices = { 0 };
        for (int k = m_heapSize; k < n; ++k) step.sortedIndices.push_back(k);
        emit(step);
    }
}

} // namespace vas
