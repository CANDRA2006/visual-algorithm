#pragma once
#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace vas {

enum class ButtonState {
    NORMAL,
    HOVERED,
    PRESSED,
    DISABLED
};

struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    ButtonState state;
    std::function<void()> onClick;
    
    Button(const std::string& label, const sf::Font& font, 
           const sf::Vector2f& position, const sf::Vector2f& size);
    
    bool contains(const sf::Vector2f& point) const;
    void update(const sf::Vector2f& mousePos);
    void render(sf::RenderWindow& window);
};

struct Slider {
    sf::RectangleShape track;
    sf::RectangleShape handle;
    sf::Text label;
    float minValue;
    float maxValue;
    float currentValue;
    bool isDragging;
    std::function<void(float)> onValueChanged;
    
    Slider(const std::string& labelText, const sf::Font& font,
           const sf::Vector2f& position, float width);
    
    void update(const sf::Vector2f& mousePos, bool mousePressed);
    void render(sf::RenderWindow& window);
    void setValue(float value);
};

class UIManager {
private:
    sf::RenderWindow& m_window;
    sf::Font& m_font;
    
    // UI Components
    std::vector<std::unique_ptr<Button>> m_buttons;
    std::vector<std::unique_ptr<Slider>> m_sliders;
    sf::Text m_titleText;
    sf::Text m_algorithmInfo;
    
    // Dropdown menu
    struct DropdownMenu {
        sf::RectangleShape background;
        sf::Text selectedText;
        std::vector<sf::Text> options;
        bool isOpen;
        int selectedIndex;
        
        DropdownMenu(const sf::Font& font);
        void update(const sf::Vector2f& mousePos, bool mousePressed);
        void render(sf::RenderWindow& window);
        void setPosition(const sf::Vector2f& pos);
    } m_algorithmDropdown;

public:
    UIManager(sf::RenderWindow& window, sf::Font& font);
    ~UIManager() = default;
    
    void initialize();
    void update(float deltaTime);
    void render();
    void handleEvent(const sf::Event& event);
    
    // Button management
    Button* addButton(const std::string& label, const sf::Vector2f& position,
                      const sf::Vector2f& size, std::function<void()> onClick);
    
    // Slider management
    Slider* addSlider(const std::string& label, const sf::Vector2f& position,
                      float width, float minVal, float maxVal, float initialVal,
                      std::function<void(float)> onValueChanged);
    
    // Getters
    int getSelectedAlgorithm() const { return m_algorithmDropdown.selectedIndex; }
};

} // namespace vas

#endif // UI_MANAGER_H