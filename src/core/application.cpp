#include "core/application.h"
#include <iostream>

namespace vas {

Application::Application() 
    : m_window(sf::VideoMode({1200, 800}), "Visual Algorithm Simulator", 
               sf::Style::Titlebar | sf::Style::Close)
    , m_running(true) {
    m_window.setFramerateLimit(60);
}

bool Application::initialize() {
    // Load font dari Windows
    std::vector<std::string> fontPaths = {
        "assets/fonts/Roboto-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/segoeui.ttf"
    };
    
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (m_font.loadFromFile(path)) {
            std::cout << "Font loaded: " << path << std::endl;
            fontLoaded = true;
            break;
        }
    }
    
    if (!fontLoaded) {
        std::cerr << "Warning: No font loaded" << std::endl;
    }
    
    // Initialize visualizer
    m_visualizer = std::make_unique<SortingVisualizer>(m_window, m_font);
    m_visualizer->initialize();
    m_visualizer->generateRandomData();
    
    return true;
}

void Application::run() {
    while (m_running && m_window.isOpen()) {
        float deltaTime = m_clock.restart().asSeconds();
        
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.is<sf::Event::Closed>()) {
                m_window.close();
            }
            
            // Keyboard shortcuts
            if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    m_visualizer->start();
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::P) {
                    m_visualizer->pause();
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                    m_visualizer->reset();
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::G) {
                    m_visualizer->generateRandomData();
                }
            }
        }
        
        m_visualizer->update(deltaTime);
        
        m_window.clear(sf::Color(30, 30, 30));
        m_visualizer->render();
        m_window.display();
    }
}

} // namespace vas