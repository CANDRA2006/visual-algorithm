#pragma once
#ifndef SORTING_VISUALIZER_H
#define SORTING_VISUALIZER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "algorithms/sorting/bubble_sort.h"

namespace vas {

class SortingVisualizer {
private:
    sf::RenderWindow& m_window;
    std::unique_ptr<BubbleSort> m_algorithm;
    sf::Font& m_font;
    std::vector<sf::RectangleShape> m_bars;
    std::string m_currentStep;

public:
    SortingVisualizer(sf::RenderWindow& window, sf::Font& font);
    void initialize();
    void update(float deltaTime);
    void render();
    void generateRandomData();
    void start();
    void pause();
    void reset();
    void step();
    void setSpeed(float speed);
};

} // namespace vas

#endif