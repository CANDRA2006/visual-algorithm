#pragma once
#ifndef VAS_ALGORITHM_BASE_H
#define VAS_ALGORITHM_BASE_H

/**
 * @file algorithm_base.h
 * @brief Abstract base class for all visualizable algorithms.
 *
 * Design notes:
 *  - AlgorithmState drives the step-pump in update().
 *  - AlgorithmStats is updated by concrete algorithms on every comparison/swap.
 *  - onStepChanged callback lets the visualizer react to algorithm events
 *    without tight coupling.
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <chrono>

namespace vas {

 
// AlgorithmState
 
enum class AlgorithmState { IDLE, RUNNING, PAUSED, COMPLETED };

 
// AlgorithmStats — collected during a run
 
struct AlgorithmStats {
    size_t comparisons = 0;
    size_t swaps       = 0;
    double elapsedMs   = 0.0;

    void reset() { comparisons = 0; swaps = 0; elapsedMs = 0.0; }
};

 
// AlgorithmStep — emitted per logical step
 
struct AlgorithmStep {
    std::string        description;
    std::vector<int>   highlightedIndices; ///< Indices to highlight
    std::vector<int>   sortedIndices;      ///< Indices that are "done" (green)
    sf::Color          highlightColor { sf::Color::Yellow };
    bool               isFinal { false };  ///< True when sort is complete

    explicit AlgorithmStep(const std::string& desc     = "",
                           const sf::Color&   color    = sf::Color::Yellow,
                           bool               isFinalStep = false)
        : description(desc), highlightColor(color), isFinal(isFinalStep) {}
};

 
// AlgorithmBase
 
class AlgorithmBase {
public:
    using StepCallback = std::function<void(const AlgorithmStep&)>;

    explicit AlgorithmBase(const std::string& name)
        : m_name(name) {}

    virtual ~AlgorithmBase() = default;

    // -- Lifecycle  -----
    virtual void initialize() = 0; ///< Called once before first run
    virtual void update()     = 0; ///< Called every frame while RUNNING
    virtual void reset()      = 0; ///< Restore data to pre-sort state
    virtual void stepOnce()   = 0; ///< Advance exactly one comparison (PAUSED mode)

    // -- State control  -
    void start()  {
        m_state = AlgorithmState::RUNNING;
        m_startTime = std::chrono::steady_clock::now();
    }
    void pause()  { m_state = AlgorithmState::PAUSED; }
    void resume() { m_state = AlgorithmState::RUNNING; }
    void stop()   { m_state = AlgorithmState::IDLE; }

    // -- Accessors  -----
    AlgorithmState      getState()          const { return m_state; }
    const std::string&  getName()           const { return m_name; }
    const AlgorithmStats& getStats()        const { return m_stats; }
    float               getAnimationSpeed() const { return m_animationSpeed; }

    /**
     * @param speed Steps executed per rendered frame (1.0 = default).
     *              Higher = faster animation.
     */
    void setAnimationSpeed(float speed) {
        m_animationSpeed = (speed > 0.0f) ? speed : 0.1f;
    }

    // -- Callback  ------
    StepCallback onStepChanged; ///< Set by visualizer

protected:
    AlgorithmState m_state       { AlgorithmState::IDLE };
    AlgorithmStats m_stats;
    float          m_animationSpeed { 1.0f };
    std::string    m_name;

    /// Step accumulator: concrete classes consume integer steps each frame.
    float m_stepAccumulator { 0.0f };

    std::chrono::steady_clock::time_point m_startTime;

    void updateElapsed() {
        auto now = std::chrono::steady_clock::now();
        m_stats.elapsedMs = std::chrono::duration<double, std::milli>(
            now - m_startTime).count();
    }

    void emit(const AlgorithmStep& step) const {
        if (onStepChanged) onStepChanged(step);
    }
};

} // namespace vas

#endif // VAS_ALGORITHM_BASE_H
