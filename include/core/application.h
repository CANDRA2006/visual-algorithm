#pragma once
#ifndef VAS_APPLICATION_H
#define VAS_APPLICATION_H

/**
 * @file application.h
 * @brief Top-level application driver.
 *
 * Responsibilities:
 *  - Own the SFML RenderWindow.
 *  - Load shared resources (font).
 *  - Drive the main event/update/render loop.
 *  - Delegate all algorithm logic to SortingVisualizer.
 */

#include <SFML/Graphics.hpp>
#include <memory>
#include "visualizer/sorting_visualizer.h"

namespace vas {

class Application {
public:
    Application();

    /// Load resources and create subsystems. Returns false on hard failure.
    bool initialize();

    /// Enter the main loop — blocks until window closes.
    void run();

private:
    //  SFML  
    sf::RenderWindow              m_window;
    sf::Font                      m_font;
    sf::Clock                     m_clock;

    //  Subsystems  
    std::unique_ptr<SortingVisualizer> m_visualizer;

    //  Helpers  
    bool        loadFont();
    void        processEvents();
    void        handleKeyPress(sf::Keyboard::Scancode code);

    bool m_running { true };
};

} // namespace vas

#endif // VAS_APPLICATION_H
