#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "visualizer/sorting_visualizer.h"

namespace vas {

class Application {
private:
    sf::RenderWindow m_window;
    sf::Font m_font;
    std::unique_ptr<SortingVisualizer> m_visualizer;
    bool m_running;
    sf::Clock m_clock;

public:
    Application();
    bool initialize();
    void run();
};

} // namespace vas

#endif