#pragma once
#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

#include "core/algorithm_base.h"
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

namespace vas {

class BubbleSort : public AlgorithmBase {
private:
    std::vector<int> m_data;
    std::vector<int> m_originalData;
    size_t m_i;
    size_t m_j;
    bool m_swapped;

public:
    BubbleSort() : AlgorithmBase("Bubble Sort"), m_i(0), m_j(0), m_swapped(false) {}
    
    void initialize() override {
        if (m_data.empty()) generateRandomData(20);
        m_originalData = m_data;
        m_i = 0;
        m_j = 0;
        m_swapped = false;
    }
    
    void update() override {
        if (m_state != AlgorithmState::RUNNING) return;
        
        static float accumulator = 0;
        accumulator += m_animationSpeed;
        
        while (accumulator >= 1.0f && m_state == AlgorithmState::RUNNING) {
            if (m_i < m_data.size() - 1) {
                if (m_j < m_data.size() - m_i - 1) {
                    AlgorithmStep step;
                    step.highlightedIndices = {static_cast<int>(m_j), static_cast<int>(m_j + 1)};
                    
                    if (m_data[m_j] > m_data[m_j + 1]) {
                        std::swap(m_data[m_j], m_data[m_j + 1]);
                        m_swapped = true;
                        step.description = "Swapping " + std::to_string(m_data[m_j]) + 
                                         " and " + std::to_string(m_data[m_j + 1]);
                        step.highlightColor = sf::Color::Red;
                    } else {
                        step.description = "No swap needed";
                        step.highlightColor = sf::Color::Yellow;
                    }
                    
                    if (onStepChanged) onStepChanged(step);
                    m_j++;
                } else {
                    if (!m_swapped) {
                        AlgorithmStep finalStep("Array is sorted!");
                        finalStep.highlightColor = sf::Color::Green;
                        for (size_t k = 0; k < m_data.size(); ++k) {
                            finalStep.highlightedIndices.push_back(k);
                        }
                        if (onStepChanged) onStepChanged(finalStep);
                        m_state = AlgorithmState::COMPLETED;
                    } else {
                        m_i++;
                        m_j = 0;
                        m_swapped = false;
                        
                        AlgorithmStep passStep("Pass " + std::to_string(m_i) + " completed");
                        passStep.highlightColor = sf::Color::Blue;
                        if (onStepChanged) onStepChanged(passStep);
                    }
                }
            } else {
                m_state = AlgorithmState::COMPLETED;
            }
            accumulator -= 1.0f;
        }
    }
    
    void reset() override {
        m_data = m_originalData;
        m_i = 0;
        m_j = 0;
        m_swapped = false;
        m_state = AlgorithmState::IDLE;
    }
    
    void step() override {
        if (m_state == AlgorithmState::PAUSED) {
            // Implement step-by-step
            if (m_i < m_data.size() - 1) {
                if (m_j < m_data.size() - m_i - 1) {
                    if (m_data[m_j] > m_data[m_j + 1]) {
                        std::swap(m_data[m_j], m_data[m_j + 1]);
                    }
                    m_j++;
                } else {
                    m_i++;
                    m_j = 0;
                }
            }
        }
    }
    
    void generateRandomData(size_t size) {
        m_data.resize(size);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<int> distribution(10, 100);
        
        for (auto& val : m_data) {
            val = distribution(generator);
        }
        m_originalData = m_data;
    }
    
    const std::vector<int>& getData() const { return m_data; }
};

} // namespace vas

#endif