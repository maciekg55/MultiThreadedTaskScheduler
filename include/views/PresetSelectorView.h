#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class PresetSelectorView {
public:
    PresetSelectorView(sf::RenderWindow& window, sf::Font& font);

    void setPresets(const std::vector<std::string>& presets);
    void draw();
    void handleClick(sf::Vector2f mouse);

    bool hasSelection() const { return _pendingSelection; }
    std::string consumeSelection();

private:
    sf::RenderWindow& _window;
    sf::Font& _font;

    std::vector<std::string> _presets;
    int _selectedIndex = 0;
    bool _dropdownOpen = false;
    bool _pendingSelection = false;
    std::string _pendingName;

    sf::FloatRect _selectorRect;
    std::vector<sf::FloatRect> _dropdownRects;
};