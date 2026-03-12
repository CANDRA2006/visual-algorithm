#pragma once
#ifndef ALGORITHM_BASE_H
#define ALGORITHM_BASE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

namespace vas {

enum class AlgorithmState {
    IDLE, RUNNING, PAUSED, COMPLETED
};

struct AlgorithmStep {
    std::string description;
    std::vector<int> highlightedIndices;
    sf::Color highlightColor;
    
    AlgorithmStep(const std::string& desc = "", const sf::Color& color = sf::Color::Yellow)
        : description(desc), highlightColor(color) {}
};

class AlgorithmBase {
protected:
    AlgorithmState m_state;
    std::string m_name;
    float m_animationSpeed;

public:
    AlgorithmBase(const std::string& name) : m_state(AlgorithmState::IDLE), m_name(name), m_animationSpeed(1.0f) {}
    virtual ~AlgorithmBase() = default;
    
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void reset() = 0;
    virtual void step() = 0;
    
    void start() { m_state = AlgorithmState::RUNNING; }
    void pause() { m_state = AlgorithmState::PAUSED; }
    void resume() { m_state = AlgorithmState::RUNNING; }
    void stop() { m_state = AlgorithmState::IDLE; }
    
    AlgorithmState getState() const { return m_state; }
    std::string getName() const { return m_name; }
    void setAnimationSpeed(float speed) { m_animationSpeed = speed; }
    float getAnimationSpeed() const { return m_animationSpeed; }
    
    std::function<void(const AlgorithmStep&)> onStepChanged;
};

} // namespace vas

#endif