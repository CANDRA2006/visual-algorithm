#include "core/application.h"
#include "core/logger.h"
#include <iostream>
#include <array>

namespace vas {

Application::Application()
    : m_window(sf::VideoMode({1280, 800}),
               "Visual Algorithm Simulator v2.1",
               sf::Style::Titlebar | sf::Style::Close)
{
    m_window.setFramerateLimit(60);
    VAS_LOG_INFO("Application created (1280x800 @ 60fps)");
}

bool Application::initialize()
{
    if (!loadFont()) {
        VAS_LOG_WARN("Continuing without a font - text will not be rendered.");
    }

    m_visualizer = std::make_unique<SortingVisualizer>(m_window, m_font);
    m_visualizer->initialize();
    m_visualizer->generateRandomData();

    VAS_LOG_INFO("Application initialized successfully");
    return true;
}

void Application::run()
{
    VAS_LOG_INFO("Entering main loop");

    while (m_running && m_window.isOpen()) {
        const float dt = m_clock.restart().asSeconds();
        processEvents();
        m_visualizer->update(dt);

        m_window.clear(sf::Color(20, 20, 30));
        m_visualizer->render();
        m_window.display();
    }

    VAS_LOG_INFO("Main loop exited");
}

bool Application::loadFont()
{
    static const std::array<const char*, 8> k_fontPaths = {
        "assets/fonts/Roboto-Regular.ttf",
        "assets/fonts/OpenSans-Regular.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/Library/Fonts/Arial.ttf"
    };

    for (const char* path : k_fontPaths) {
        if (m_font.openFromFile(path)) {
            VAS_LOG_INFO(std::string("Font loaded: ") + path);
            return true;
        }
    }

    VAS_LOG_ERROR("No font found in any known path");
    return false;
}

void Application::processEvents()
{
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            m_running = false;
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            handleKeyPress(kp->scancode);
        }
    }
}

void Application::handleKeyPress(sf::Keyboard::Scancode code)
{
    using SC = sf::Keyboard::Scancode;

    switch (code) {
        case SC::Space:
            m_visualizer->start();
            break;

        case SC::P:
            m_visualizer->pause();
            break;

        case SC::Period:
        case SC::Right:
            m_visualizer->stepOnce();
            break;

        case SC::R:
            m_visualizer->reset();
            break;

        case SC::G:
            m_visualizer->generateRandomData();
            break;

        case SC::Equal:
            m_visualizer->setSpeed(m_visualizer->getSpeed() + 0.5f);
            break;

        case SC::Hyphen:
            m_visualizer->setSpeed(std::max(0.1f, m_visualizer->getSpeed() - 0.5f));
            break;

        case SC::RBracket:
            m_visualizer->setArraySize(m_visualizer->getArraySize() + 5);
            break;

        case SC::LBracket:
            if (m_visualizer->getArraySize() > 5)
                m_visualizer->setArraySize(m_visualizer->getArraySize() - 5);
            break;

        //  Algorithm selection: 1-9 
        case SC::Num1: m_visualizer->selectAlgorithm(0); break;
        case SC::Num2: m_visualizer->selectAlgorithm(1); break;
        case SC::Num3: m_visualizer->selectAlgorithm(2); break;
        case SC::Num4: m_visualizer->selectAlgorithm(3); break;
        case SC::Num5: m_visualizer->selectAlgorithm(4); break;
        case SC::Num6: m_visualizer->selectAlgorithm(5); break; // Heap Sort
        case SC::Num7: m_visualizer->selectAlgorithm(6); break; // Shell Sort
        case SC::Num8: m_visualizer->selectAlgorithm(7); break; // Counting Sort
        case SC::Num9: m_visualizer->selectAlgorithm(8); break; // Radix Sort

        // Audio toggle 
        case SC::M:
            m_visualizer->toggleAudio();
            break;

        case SC::Escape:
            m_window.close();
            m_running = false;
            break;

        default:
            break;
    }
}

} // namespace vas