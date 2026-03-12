#include "visualizer/sorting_visualizer.h"
#include <algorithm>

namespace vas {

SortingVisualizer::SortingVisualizer(sf::RenderWindow& window, sf::Font& font)
    : m_window(window), m_font(font) {
    m_algorithm = std::make_unique<BubbleSort>();
    
    m_algorithm->onStepChanged = [this](const AlgorithmStep& step) {
        m_currentStep = step.description;
    };
}

void SortingVisualizer::initialize() {
    m_algorithm->initialize();
}

void SortingVisualizer::update(float deltaTime) {
    m_algorithm->update();
}

void SortingVisualizer::render() {
    const auto& data = m_algorithm->getData();
    if (data.empty()) return;
    
    auto windowSize = m_window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    float barWidth = (windowWidth - 200.0f) / data.size() - 5;
    float maxValue = *std::max_element(data.begin(), data.end());
    float baseY = windowHeight - 150.0f;
    
    m_bars.clear();
    
    for (size_t i = 0; i < data.size(); ++i) {
        float barHeight = (data[i] / maxValue) * 300.0f;
        float xPos = 100.0f + i * (barWidth + 5);
        
        sf::RectangleShape bar(sf::Vector2f(barWidth, barHeight));
        bar.setPosition(sf::Vector2f(xPos, baseY - barHeight));
        
        // Color based on value
        int blueValue = 100 + (data[i] * 155) / 100;
        bar.setFillColor(sf::Color(100, 100, static_cast<sf::Uint8>(blueValue)));
        bar.setOutlineColor(sf::Color::White);
        bar.setOutlineThickness(1.0f);
        
        m_window.draw(bar);
        
        // Draw value text
        if (m_font.getInfo().family != "") {
            sf::Text text(std::to_string(data[i]), m_font, 12);
            text.setFillColor(sf::Color::White);
            text.setPosition(sf::Vector2f(
                xPos + (barWidth - text.getLocalBounds().size.x) / 2,
                baseY - barHeight - 20
            ));
            m_window.draw(text);
        }
    }
    
    // Draw algorithm info
    if (!m_currentStep.empty() && m_font.getInfo().family != "") {
        sf::Text infoText("Status: " + m_currentStep, m_font, 18);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(sf::Vector2f(20, 20));
        m_window.draw(infoText);
        
        // Draw algorithm name
        sf::Text nameText("Algorithm: " + m_algorithm->getName(), m_font, 18);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition(sf::Vector2f(20, 50));
        m_window.draw(nameText);
        
        // Draw controls
        sf::Text controls("Controls: [Space] Start/Pause | [R] Reset | [G] Generate | [P] Pause", m_font, 14);
        controls.setFillColor(sf::Color(200, 200, 200));
        controls.setPosition(sf::Vector2f(20, windowHeight - 50));
        m_window.draw(controls);
    }
}

void SortingVisualizer::generateRandomData() {
    m_algorithm->generateRandomData(20);
    m_algorithm->reset();
    m_currentStep = "New data generated";
}

void SortingVisualizer::start() {
    if (m_algorithm->getState() == AlgorithmState::IDLE || 
        m_algorithm->getState() == AlgorithmState::COMPLETED) {
        m_algorithm->reset();
        m_algorithm->start();
    } else if (m_algorithm->getState() == AlgorithmState::PAUSED) {
        m_algorithm->resume();
    } else if (m_algorithm->getState() == AlgorithmState::RUNNING) {
        m_algorithm->pause();
    }
}

void SortingVisualizer::pause() {
    if (m_algorithm->getState() == AlgorithmState::RUNNING) {
        m_algorithm->pause();
    }
}

void SortingVisualizer::reset() {
    m_algorithm->reset();
    m_currentStep = "Reset";
}

void SortingVisualizer::step() {
    m_algorithm->step();
}

void SortingVisualizer::setSpeed(float speed) {
    m_algorithm->setAnimationSpeed(speed);
}

} // namespace vas