#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * @brief Dropdown widget in the stats bar for choosing a task preset.
 *
 * Displays the currently selected preset name and opens an upward dropdown
 * on click. A confirmed selection is surfaced via consumeSelection().
 */
class PresetSelectorView {
public:
    /// @brief Construct the preset selector bound to the given window and font.
    PresetSelectorView(sf::RenderWindow& window, sf::Font& font);

    /// @brief Populate the dropdown with the given preset names.
    /// @param presets List of preset names without extension.
    void setPresets(const std::vector<std::string>& presets);

    /// @brief Draw the selector widget to the window.
    void draw();

    /// @brief Handle a mouse click to open/close the dropdown or confirm a selection.
    /// @param mouse Cursor position in window coordinates.
    void handleClick(sf::Vector2f mouse);

    /// @brief Return true if the user has confirmed a preset selection.
    bool hasSelection() const { return _pendingSelection; }

    /// @brief Retrieve and clear the pending preset selection name.
    /// @return The selected preset name.
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
